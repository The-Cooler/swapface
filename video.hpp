#pragma once
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <queue>
#include <iostream>
#include <windows.h>
#include <wlanapi.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#pragma comment(lib, "wlanapi.lib")
/***************************************************************************************
*               时间：2025-4-25
*               
*               实现：摄像头获取
*
*               错误：暂无
*               
*               日志：暂未完成
*
***************************************************************************************/

namespace Video{
    /***********************************命名空间变量*************************************/
    const unsigned                  WEIGHT = 1280u;
    const unsigned                  HEIGHT = 800u;
    const std::string_view          PHONE = "http://10.242.10.156:8080/videofeed?fflags=nobuffer&flags=low_delay";
    const std::string_view          WIFI = "http://192.168.1.103:8080/videofeed?fflags=nobuffer&flags=low_delay";
    extern unsigned                 count;

    class Videos{
    /***********************************成员变量*************************************/
    public:
        cv::Mat                     last_frame;
        std::queue<cv::Mat>         frame_queue;
        cv::VideoCapture            cap;
    private:
        std::thread                 capture_thread;
        std::thread                 display_thread;
        std::mutex                  frame_mutex;
        std::condition_variable     frame_ready;
        std::atomic<bool>           is_running{false};
        std::atomic<bool>           is_frame_ready{false};
        unsigned                    target_fps = 30;
    /***********************************成员函数*************************************/
    public:
        void star(bool op = false, unsigned weight = WEIGHT, unsigned height = HEIGHT);
        void show(unsigned weight = WEIGHT, unsigned height = HEIGHT);
    private:
        void close();
        bool isPhone();
        void capture_loop();
        void display_loop(unsigned weight, unsigned height);
    };
}