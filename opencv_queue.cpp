#include "opencv_queue.h"
#include <sys/time.h>

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

Mat OPENCV_QUEUE::getMatQueue_TimeOut()
{
    pthread_mutex_lock(&matMutex);
    if (matPictureQueue.size() == 0)
    {
        struct timeval timeval_now;
        struct timespec timespec_out;
        gettimeofday(&timeval_now, NULL);
        timespec_out.tv_sec = timeval_now.tv_sec + 1;
        timespec_out.tv_nsec = timeval_now.tv_usec * 1000;
        int ret = pthread_cond_timedwait(&matCond, &matMutex, &timespec_out);
        if (ret != 0)
        {
            pthread_mutex_unlock(&matMutex);
            //return NULL;
        }
    }

    Mat item = matPictureQueue.front();
    matPictureQueue.pop();
    pthread_mutex_lock(&matMutex);
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
