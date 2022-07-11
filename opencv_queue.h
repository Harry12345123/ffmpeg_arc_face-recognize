#ifndef __OPENCV_QUEUE__
#define __OPENCV_QUEUE__

#include <pthread.h>
#include <queue>
#include <list>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class OPENCV_QUEUE
{
public:
    //声明一个队列
    std::queue<Mat> matPictureQueue;
    //线程锁
    pthread_mutex_t matMutex;
    //条件变量
    pthread_cond_t matCond;

public:
    OPENCV_QUEUE();

    ~OPENCV_QUEUE();

    OPENCV_QUEUE * queue;

   /* //入队函数
    int putAVPacket(Mat mat);
    //出队函数
    int getAVPacket(Mat mat);
    //队列大小
    int getAvPactekQueueSize();*/

//==========Mat Queueu ==================

    //入队函数
    int putMatQueue(Mat mat);
    //出队函数
    Mat getMatQueue();
    //队列大小
    int getMatQueueSize();

};

// std::queue<Mat> matPictureQueue;

#endif