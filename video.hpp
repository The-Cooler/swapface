#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <queue>
/***************************************************************************************
*               时间：2025-4-25
*               
*               实现：摄像头获取
*
*               错误：
*               
*               日志：暂未完成
*
***************************************************************************************/

namespace Video{
    /***********************************命名空间变量*************************************/
    const unsigned WEIGHT = 1280u;
    const unsigned HEIGHT = 800u;
    const std::string_view PHONE = "";
    const std::string_view WIFI = "";


    class Video{
    /***********************************成员变量*************************************/
    public:
        cv::Mat last_frame;
        std::queue<cv::Mat> frame_queue;
        cv::VideoCapture cap;
    /***********************************成员函数*************************************/
    public:
        void star(bool op = false, unsigned weight = WEIGHT, unsigned height = HEIGHT);
        void show(unsigned weight = WEIGHT, unsigned height = HEIGHT);
    private:
        void close();
    };
}