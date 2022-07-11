#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    Mat image1 = imread("./chepai_04.png");
    if (!image1.data)
    {
        printf("第一张图片读取失败!\n");
        return -1;
    }

    Mat imageROI = image1(Rect(0, 0, image1.cols - image1.cols % 4, image1.rows)); 
    image1.copyTo(imageROI);
    cv::imshow("测试窗口", image1);
    waitKey(0);
    return 0;
}