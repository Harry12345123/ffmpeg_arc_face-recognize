#include "asfort_manage.h"
#include <iostream>
#include <time.h>

using namespace std;

void timestampToTime(char *timeStamp, char *dateTime, int dateTimeSize)
{
    time_t tTimeStamp = atoll(timeStamp);
    struct tm *pTm = gmtime(&tTimeStamp);
    strftime(dateTime, dateTimeSize, "%Y-%m-%d %H:%M:%S", pTm);
}

void CutIplImage(IplImage *src, IplImage *dst, int x, int y)
{
   CvSize size = cvSize(dst->width, dst->height);             //区域大小
   cvSetImageROI(src, cvRect(x, y, size.width, size.height)); //设置源图像ROI
   cvCopy(src, dst);                                          //复制图像
   cvResetImageROI(src);                                      //源图像用完后，清空ROI
}

int init_asfort_device(MPChar appid, MPChar sdkkey)
{
    ASF_ActiveFileInfo activeFileInfo;
    MRESULT res = MOK;
    res = ASFGetActiveFileInfo(&activeFileInfo);
    if (res != MOK)
    {
        printf("ASFGetActiveFileInfo fail: %d\n", res);
    }
    else
    {
        char startDateTime[32];
        timestampToTime(activeFileInfo.startTime, startDateTime, 32);
        printf("startTime: %s\n", startDateTime);
        char endDateTime[32];
        timestampToTime(activeFileInfo.endTime, endDateTime, 32);
        printf("endTime: %s\n", endDateTime);
    }

    res = ASFOnlineActivation(appid, sdkkey);
    if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res)
        printf("ASFOnlineActivation fail: %d\n", res);
    else
        printf("ASFOnlineActivation sucess: %d\n", res);


    return 0;
}




#if 1
//图像颜色格式转换
int ColorSpaceConversion(IplImage* image, MInt32 format, ASVLOFFSCREEN& offscreen)
{
    switch (format)
    {
    case ASVL_PAF_RGB24_B8G8R8:
        offscreen.u32PixelArrayFormat = (unsigned int)format;
        offscreen.i32Width = image->width;
        offscreen.i32Height = image->height;
        offscreen.pi32Pitch[0] = image->widthStep;
        offscreen.ppu8Plane[0] = (MUInt8*)image->imageData;
        break;
    case ASVL_PAF_GRAY:
        offscreen.u32PixelArrayFormat = (unsigned int)format;
        offscreen.i32Width = image->width;
        offscreen.i32Height = image->height;
        offscreen.pi32Pitch[0] = image->widthStep;
        offscreen.ppu8Plane[0] = (MUInt8*)image->imageData;
        break;
    default:
        return 0;
    }
    return 1;
}

int ColorSpaceConversion(MInt32 width, MInt32 height, MInt32 format, MUInt8* imgData, ASVLOFFSCREEN& offscreen)
{
        offscreen.u32PixelArrayFormat = (unsigned int)format;
        offscreen.i32Width = width;
        offscreen.i32Height = height;

        switch (offscreen.u32PixelArrayFormat)
        {
        case ASVL_PAF_RGB24_B8G8R8:
                offscreen.pi32Pitch[0] = offscreen.i32Width * 3;
                offscreen.ppu8Plane[0] = imgData;
                break;
        case ASVL_PAF_I420:
                offscreen.pi32Pitch[0] = width;
                offscreen.pi32Pitch[1] = width >> 1;
                offscreen.pi32Pitch[2] = width >> 1;
                offscreen.ppu8Plane[0] = imgData;
                offscreen.ppu8Plane[1] = offscreen.ppu8Plane[0] + offscreen.i32Height*offscreen.i32Width;
                offscreen.ppu8Plane[2] = offscreen.ppu8Plane[0] + offscreen.i32Height*offscreen.i32Width * 5 / 4;
                break;
        case ASVL_PAF_NV12:
        case ASVL_PAF_NV21:
                offscreen.pi32Pitch[0] = offscreen.i32Width;
                offscreen.pi32Pitch[1] = offscreen.pi32Pitch[0];
                offscreen.ppu8Plane[0] = imgData;
                offscreen.ppu8Plane[1] = offscreen.ppu8Plane[0] + offscreen.pi32Pitch[0] * offscreen.i32Height;
                break;
        case ASVL_PAF_YUYV:
        case ASVL_PAF_DEPTH_U16:
                offscreen.pi32Pitch[0] = offscreen.i32Width * 2;
                offscreen.ppu8Plane[0] = imgData;
                break;
        case ASVL_PAF_GRAY:
                offscreen.pi32Pitch[0] = offscreen.i32Width;
                offscreen.ppu8Plane[0] = imgData;
                break;
        default:
                return 0;
        }
        return 1;
}


void PicCutOut(IplImage *src, IplImage *dst, int x, int y)
{
    if (!src || !dst)
    {
        return;
    }

    CvSize size = cvSize(dst->width, dst->height);             //区域大小
    cvSetImageROI(src, cvRect(x, y, size.width, size.height)); //设置源图像ROI
    cvCopy(src, dst);                                          //复制图像
    cvResetImageROI(src);                                      //源图像用完后，清空ROI
}

MRESULT PreDetectFace(IplImage* image, ASF_MultiFaceInfo *MultiFaces, bool isRGB, MHandle tempEngine)
{
    if (!image) 
    {   
      cout << "图片为空" << endl;    
      return -1; 
    }

    IplImage* cutImg = NULL;
    MRESULT res = MOK;
    cutImg = cvCreateImage(cvSize(image->width - (image->width % 4), image->height), IPL_DEPTH_8U, image->nChannels);
    PicCutOut(image, cutImg, 0, 0);
    ASVLOFFSCREEN offscreen = { 0 };
    ColorSpaceConversion(cutImg, ASVL_PAF_RGB24_B8G8R8, offscreen);
    res = ASFDetectFacesEx(tempEngine, &offscreen, MultiFaces);
    if (res != MOK || MultiFaces->faceNum < 1)
    {
        cvReleaseImage(&cutImg);
        return -1;
    }
    return res;
}

MRESULT PreExtractFeature(IplImage *image, ASF_FaceFeature *feature, ASF_SingleFaceInfo *faceRect, MHandle tempEngine)
{
    if(!image || image->imageData == NULL)
      return -1;
    
    IplImage* cutImg = cvCreateImage(cvSize(image->width - (image->width % 4), image->height), IPL_DEPTH_8U, image->nChannels);
    PicCutOut(image, cutImg, 0, 0);
    if(!cutImg)
    {
        cvReleaseImage(&cutImg); 
        return -1;
    }

    MRESULT res = MOK;
    ASF_FaceFeature detectFaceFeature = {0};
    ASVLOFFSCREEN offscreen = { 0 };
    ColorSpaceConversion(cutImg, ASVL_PAF_RGB24_B8G8R8, offscreen);
    res = ASFFaceFeatureExtractEx(tempEngine, &offscreen, faceRect, &detectFaceFeature);
    if(!detectFaceFeature.feature)
    {
      return -1;
    }
    feature->featureSize = detectFaceFeature.featureSize;
    feature->feature = (MByte *) malloc(detectFaceFeature.featureSize);
    memset(feature->feature, 0, detectFaceFeature.featureSize);
    memcpy(feature->feature, detectFaceFeature.feature, detectFaceFeature.featureSize);
    return 0;
}
#endif
