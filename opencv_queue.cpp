#include "opencv_queue.h"

OPENCV_QUEUE::OPENCV_QUEUE()
{
    pthread_mutex_init(&matMutex, NULL);
    pthread_cond_init(&matCond, NULL);
}

OPENCV_QUEUE ::~OPENCV_QUEUE()
{
    pthread_mutex_destroy(&matMutex);
    pthread_cond_destroy(&matCond);
}

int OPENCV_QUEUE::putMatQueue(Mat mat)
{
    pthread_mutex_lock(&matMutex);
    matPictureQueue.push(mat);
    pthread_cond_broadcast(&matCond);
    pthread_mutex_unlock(&matMutex);
}

Mat OPENCV_QUEUE::getMatQueue()
{
    pthread_mutex_lock(&matMutex);
    while (matPictureQueue.size() == 0)
    {
        pthread_cond_wait(&matCond, &matMutex);
    }
    Mat item = matPictureQueue.front();
    matPictureQueue.pop();
    pthread_mutex_unlock(&matMutex);
    return item;
}

int OPENCV_QUEUE::getMatQueueSize()
{
    unsigned int count = 0;
    pthread_mutex_lock(&matMutex);
    count = matPictureQueue.size();
    pthread_mutex_unlock(&matMutex);
    return count;
}
