#include "sqlite3_operation.h"
#include "asfort_manage.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int add_face_data(char *face_pic, char *face_name)
{
    Connection_Sqlite3DataBase();
    MHandle m_hEngine;
    MRESULT deres;
    MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION;
    deres = ASFInitEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_ALL_OUT, NSCALE, FACENUM, mask, &m_hEngine);
    if (deres != MOK)
        printf("ASFInitEngine fail: %d\n", deres);
    else
        printf("ASFInitEngine sucess: %d\n", deres);

    Mat tempImage = imread(face_pic);
    cv::Mat image_mini;
    resize(tempImage, image_mini, cv::Size(tempImage.cols - tempImage.cols % 4, tempImage.rows));

    ASVLOFFSCREEN getoffscreen = {0};
    ColorSpaceConversion(image_mini.cols, image_mini.rows, ASVL_PAF_RGB24_B8G8R8, image_mini.data, getoffscreen);
    ASF_MultiFaceInfo getMultiFaceInfo = {0};

    MRESULT detect_res = ASFDetectFacesEx(m_hEngine, &getoffscreen, &getMultiFaceInfo);
    ASF_SingleFaceInfo getFaceInfo;
    ASF_FaceFeature getFaceFeature = {0};
    MInt32 faceNum;
    if (detect_res == MOK)
    {
        getFaceInfo.faceRect = getMultiFaceInfo.faceRect[0];
        getFaceInfo.faceOrient = getMultiFaceInfo.faceOrient[0];
        faceNum = getMultiFaceInfo.faceNum;
        MRESULT res = ASFFaceFeatureExtractEx(m_hEngine, &getoffscreen, &getFaceInfo, &getFaceFeature);
        if (res != MOK)
        {
            printf("Get Face Feature failed\n");
        }
        else
        {
            //string name = "Harry";
            insert_face_data_toDataBase(face_name, getFaceFeature.feature, getFaceFeature.featureSize);
            printf("Get Face Feature success\n");
        }
    }
    else
    {
        printf("FAILED\n");
    }

    waitKey(0);
}

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Please input format like ./asfort_face_insert face_path face_name\n");
        return -1;
    }

    init_asfort_device(APPID, SDKKEY);    

    add_face_data(argv[1], argv[2]);

    return 0;
}

