#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <opencv2/dnn.hpp>
#include <sstream>

#include "arcsoft_face_sdk.h"
#include "amcomdef.h"
#include "asvloffscreen.h"
#include "merror.h"
#include "opencv_queue.h"
#include "asfort_manage.h"
#include "sqlite3_operation.h"
#include "map_manage.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/time.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
#include <libavutil/opt.h>
#include <libavutil/fifo.h>
#include <libavutil/imgutils.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
}

#define WIDTH 640
#define HEIGHT 480
#define CAMERA_DEV "/dev/video0"
#define CAMERA_FMT AV_PIX_FMT_YUYV422
#define ENCODE_FMT AV_PIX_FMT_YUV420P

using namespace dnn;

static pthread_mutex_t workmutex = PTHREAD_MUTEX_INITIALIZER;

OPENCV_QUEUE *opencv_queue = NULL;
AVFifoBuffer *m_videoFifo = NULL;

float confThreshold = 0.5; //置信度阈值
float nmsThreshold = 0.4;  //非最大抑制阈值

float LiveScore = 0.0;

void showDshowDeviceOption(char *devName)
{
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVDictionary *options = NULL;
    av_dict_set(&options, "list_options", "true", 0);
    AVInputFormat *iformat = av_find_input_format("dshow");
    avformat_open_input(&pFormatCtx, devName, iformat, &options);
    avformat_free_context(pFormatCtx);
}

void *read_camera_thread(void *args)
{
    pthread_detach(pthread_self());
    avdevice_register_all();
    AVInputFormat *in_fmt = av_find_input_format("video4linux2");
    if (in_fmt == NULL)
    {
        printf("can't find_input_format\n");
        // return;
    }

    // 设置摄像头的分辨率
    AVDictionary *option = NULL;
    char video_size[10];
    sprintf(video_size, "%dx%d", WIDTH, HEIGHT);
    av_dict_set(&option, "video_size", video_size, 0);

    AVFormatContext *fmt_ctx = NULL;
    if (avformat_open_input(&fmt_ctx, CAMERA_DEV, in_fmt, &option) < 0)
    {
        printf("can't open_input_file\n");
        // return;
    }
    else
    {
        printf("Success Open Camera\n");
    }
    // printf device info
    av_dump_format(fmt_ctx, 0, CAMERA_DEV, 0);

    struct SwsContext *sws_ctx;
    // 图像格式转换：CAMERA_FMT --> ENCODE_FMT
    sws_ctx = sws_getContext(WIDTH, HEIGHT, CAMERA_FMT,
                             WIDTH, HEIGHT, ENCODE_FMT, 0, NULL, NULL, NULL);

    uint8_t *yuy2buf[4];
    int yuy2_linesize[4];
    int yuy2_size = av_image_alloc(yuy2buf, yuy2_linesize, WIDTH, HEIGHT, CAMERA_FMT, 1);

    uint8_t *yuv420pbuf[4];
    int yuv420p_linesize[4];
    int yuv420p_size = av_image_alloc(yuv420pbuf, yuv420p_linesize, WIDTH, HEIGHT, ENCODE_FMT, 1);

    // 初始化packet，存放编码数据
    AVPacket *camera_packet = av_packet_alloc();

    // 初始化frame，存放原始数据
    int y_size = WIDTH * HEIGHT;
    int frame_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, WIDTH, HEIGHT, 1);

    for (;;)
    {
        // 摄像头获取图像数据
        av_read_frame(fmt_ctx, camera_packet);
        memcpy(yuy2buf[0], camera_packet->data, camera_packet->size);
        // 图像格式转化
        sws_scale(sws_ctx, (const uint8_t **)yuy2buf, yuy2_linesize,
                  0, HEIGHT, yuv420pbuf, yuv420p_linesize);
        av_packet_unref(camera_packet);

        if (av_fifo_space(m_videoFifo) >= frame_size)
        {
            pthread_mutex_lock(&workmutex);
            av_fifo_generic_write(m_videoFifo, yuv420pbuf[0], y_size, NULL);
            av_fifo_generic_write(m_videoFifo, yuv420pbuf[1], y_size / 4, NULL);
            av_fifo_generic_write(m_videoFifo, yuv420pbuf[2], y_size / 4, NULL);
            pthread_mutex_unlock(&workmutex);
        }
    }

    av_packet_free(&camera_packet);
    avformat_close_input(&fmt_ctx);
    sws_freeContext(sws_ctx);
    av_freep(yuy2buf);
    av_freep(yuv420pbuf);
}

void *process_asfort_recognize_thread(void *args)
{
    pthread_detach(pthread_self());

    S_THREAD_MAP thread_map;
    get_thread_map(0, &thread_map);

    map<string, ASF_FaceFeature> database_face_map = thread_map.thread_map;
    map<string, ASF_FaceFeature>::iterator database_iter;

    MHandle m_hEngine;
    MRESULT deres;
    MRESULT res = MOK;
    MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER | ASF_LIVENESS | ASF_IR_LIVENESS;
    deres = ASFInitEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_ALL_OUT, NSCALE, FACENUM, mask, &m_hEngine);
    if (deres != MOK)
        printf("ASFInitEngine fail: %d\n", deres);
    else
        printf("ASFInitEngine sucess: %d\n", deres);

    int y_size = WIDTH * HEIGHT;
    int buffer_size = WIDTH * HEIGHT * 3 * 2;
    int frame_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, WIDTH, HEIGHT, 1);
    uint8_t *out_buffer_yuv420 = (uint8_t *)av_malloc(frame_size);

    ASF_FaceFeature detectFaceFeature = {0};
    ASF_MultiFaceInfo detectedFaces = {0};
    ASF_SingleFaceInfo tempFaceInfo = {0};
    MFloat confidenceValue;
    MFloat maxScore = 0.0;
    MFloat score = 0.0;
    string predict;

    bool is_recognize = false;

    ASF_LivenessInfo livenessInfo = {0, 0};

    while (1)
    {
        is_recognize = false;

        if (av_fifo_size(m_videoFifo) >= frame_size)
        {
            static int i = 0;
            int video_fifo_size = av_fifo_size(m_videoFifo);
            pthread_mutex_lock(&workmutex);
            av_fifo_generic_read(m_videoFifo, out_buffer_yuv420, frame_size, NULL);
            pthread_mutex_unlock(&workmutex);

            int width = WIDTH, height = HEIGHT;
            cv::Mat tmp_img = cv::Mat::zeros(height * 3 / 2, width, CV_8UC1);
            Mat mainRgbImage;

            memcpy(tmp_img.data, out_buffer_yuv420, frame_size);
            cv::cvtColor(tmp_img, mainRgbImage, COLOR_YUV2BGR_I420);

            cv::Mat image_mini;
            resize(mainRgbImage, image_mini, cv::Size(mainRgbImage.cols - mainRgbImage.cols % 4, mainRgbImage.rows));

            ASVLOFFSCREEN getoffscreen = {0};
            ColorSpaceConversion(image_mini.cols, image_mini.rows, ASVL_PAF_RGB24_B8G8R8, image_mini.data, getoffscreen);
            ASF_MultiFaceInfo getMultiFaceInfo = {0};

            MRESULT detect_res = ASFDetectFacesEx(m_hEngine, &getoffscreen, &detectedFaces);
            if (detect_res == MOK)
            {
                if (detectedFaces.faceNum > 0)
                {
                    for (int i = 0; i < detectedFaces.faceNum; i++)
                    {
                        tempFaceInfo.faceOrient = detectedFaces.faceOrient[i];
                        tempFaceInfo.faceRect = detectedFaces.faceRect[i];

                        rectangle(mainRgbImage, Point(detectedFaces.faceRect[i].left, detectedFaces.faceRect[i].top), Point(detectedFaces.faceRect[i].right, detectedFaces.faceRect[i].bottom), Scalar(255, 0, 255), 3);

                        res = ASFFaceFeatureExtractEx(m_hEngine, &getoffscreen, &tempFaceInfo, &detectFaceFeature);
                        if (res == MOK)
                        {
                            for (database_iter = database_face_map.begin(); database_iter != database_face_map.end(); database_iter++)
                            {
                                res = ASFFaceFeatureCompare(m_hEngine, &database_iter->second, &detectFaceFeature, &confidenceValue);

                                // printf("Confidence_Value = %lf\n", confidenceValue);
                                if (confidenceValue >= 0.8)
                                {
                                    // predict = database_iter->first;
                                    is_recognize = true;
                                    break;
                                }
                                else
                                {
                                    is_recognize = false;
                                    continue;
                                }

                                // printf("Name = %s\n", predict.c_str());

                                /*if (confidenceValue > maxScore)
                                {
                                    maxScore = confidenceValue;
                                    predict = database_iter->first;
                                    score = maxScore;
                                }*/

                                /*if (score >= 0.8)
                                    break;*/
                            }

                            if (is_recognize == true)
                            {
                                predict = database_iter->first;
                            }

                            if (is_recognize == false)
                            {
                                // predict= "UnKnown";
                                predict = "";
                            }

                            cv::putText(mainRgbImage, predict, cv::Point(detectedFaces.faceRect[i].left, detectedFaces.faceRect[i].top), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 2);
                        }
                    }
                }
            }
            opencv_queue->putMatQueue(mainRgbImage);
            tmp_img.release();
        }
    }
    av_fifo_free(m_videoFifo);
    av_free(out_buffer_yuv420);
    ASFUninitEngine(m_hEngine);
}

void *show_opencv_thread(void *args)
{
    pthread_detach(pthread_self());
    while (1)
    {
        if (opencv_queue->getMatQueueSize() > 0)
        {
            Mat tmp = opencv_queue->getMatQueue();
            cv::imshow("bgr_show", tmp);
            waitKey(1);
            tmp.release();
        }
    }
}

void init_face_data()
{
    Connection_Sqlite3DataBase();
    int task_id = 0;
    S_THREAD_MAP thread_map;
    map<string, ASF_FaceFeature> maps = QueryFaceFeature();
    thread_map.thread_map = maps;
    set_thread_map(task_id, &thread_map);
}

int main(int argc, char *argv[])
{

    int ret;
    m_videoFifo = av_fifo_alloc(30 * av_image_get_buffer_size(AV_PIX_FMT_YUV420P, 640, 480, 1));

    opencv_queue = new OPENCV_QUEUE();
    init_asfort_device(APPID, SDKKEY);
    init_face_data();

    pthread_t pid;
    ret = pthread_create(&pid, NULL, read_camera_thread, NULL);
    if (ret != 0)
    {
        printf("Create Read_Camera_Thread Failed\n");
    }

    ret = pthread_create(&pid, NULL, process_asfort_recognize_thread, NULL);
    if (ret != 0)
    {
        printf("Create process_asfort_recognize_thread Failed\n");
    }

    ret = pthread_create(&pid, NULL, show_opencv_thread, NULL);
    if (ret != 0)
    {
        printf("Create show_opencv_thread Failed\n");
    }

    while (1)
    {
        sleep(1000);
    }

    return 0;
}