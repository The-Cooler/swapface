#include "video.hpp"
using namespace Video;

unsigned Video::count = 0;

void Videos::star(bool op, unsigned weight, unsigned height){
/***************************************************************************************
*               时间：2025-4-26
*               
*               实现：连接摄像头，初始化last_frame
*
*               错误：暂无
*               
*               日志：暂无
*
***************************************************************************************/
    if (!op) cap.open(std::string{ isPhone() ? PHONE : WIFI });
    else cap.open(0);
    cap.read(last_frame);
    cv::resize(last_frame, last_frame, cv::Size(weight, height));
}

void Videos::capture_loop() {
    cv::Mat frame;
    while (is_running) {
        if (cap.read(frame)) {
            std::lock_guard<std::mutex> lock(frame_mutex);
            last_frame = frame;
            is_frame_ready = true;
            frame_ready.notify_one();
        }
        // 控制帧率
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000 / target_fps));
    }
}

void Videos::display_loop(unsigned weight, unsigned height) {
    cv::namedWindow("swap face", cv::WINDOW_FREERATIO);
    cv::resizeWindow("swap face", weight, height);
    
    while (is_running) {
        {
            std::unique_lock<std::mutex> lock(frame_mutex);
            frame_ready.wait(lock, [this] { return is_frame_ready || !is_running; });
            
            if (!is_running) break;
            
            cv::imshow("swap face", last_frame);
            is_frame_ready = false;
        }
        
        if (cv::waitKey(1) == 27) {
            is_running = false;
            break;
        }
    }
}

void Videos::show(unsigned weight, unsigned height) {
/***************************************************************************************
*               时间：2025-4-26
*               
*               实现：显示视频
*
*               错误：暂无
*               
*               日志：连接手机摄像头非常卡顿，已使用多线程修复
*
***************************************************************************************/
    is_running = true;
    is_frame_ready = false;
    
    // 启动捕获线程
    capture_thread = std::thread(&Videos::capture_loop, this);
    
    // 启动显示线程
    display_thread = std::thread(&Videos::display_loop, this, weight, height);
    
    // 等待显示线程结束
    display_thread.join();
    
    // 停止捕获线程
    is_running = false;
    capture_thread.join();
    
    close();
}

void Videos::close() {
/***************************************************************************************
*               时间：2025-4-26
*               
*               实现：关闭所有流
*
*               错误：暂无
*               
*               日志：暂无
*
***************************************************************************************/
    is_running = false;
    if (capture_thread.joinable()) capture_thread.join();
    if (display_thread.joinable()) display_thread.join();
    cap.release();
    last_frame.release();
    while(!frame_queue.empty()) frame_queue.pop();
}

bool Videos::isPhone(){
/***************************************************************************************
*               时间：2025-4-26
*               
*               实现：判断是否连接手机
*
*               错误：暂无
*               
*               日志：暂无
*
***************************************************************************************/
    HANDLE hClient = NULL;
    DWORD dwVersion = 0;
    bool ret = true;
    if (WlanOpenHandle(WLAN_API_VERSION, NULL, &dwVersion, &hClient) != ERROR_SUCCESS) {
        return false;
    }

    PWLAN_INTERFACE_INFO_LIST pInterfaceList = NULL;
    if (WlanEnumInterfaces(hClient, NULL, &pInterfaceList) != ERROR_SUCCESS) {
        WlanCloseHandle(hClient, NULL);
        return false;
    }

    bool isHotspot = false;
    for (DWORD i = 0; i < pInterfaceList->dwNumberOfItems; i++) {
        PWLAN_INTERFACE_INFO pInterface = &pInterfaceList->InterfaceInfo[i];
        PWLAN_CONNECTION_ATTRIBUTES pConnectInfo = NULL;
        DWORD connectInfoSize = 0;
        WLAN_OPCODE_VALUE_TYPE opCode;

        if (WlanQueryInterface(hClient, &pInterface->InterfaceGuid,
                              wlan_intf_opcode_current_connection, NULL,
                              &connectInfoSize, (PVOID*)&pConnectInfo, &opCode) == ERROR_SUCCESS) {

            std::string ssid(
                (char*)pConnectInfo->wlanAssociationAttributes.dot11Ssid.ucSSID,
                pConnectInfo->wlanAssociationAttributes.dot11Ssid.uSSIDLength
            );
            char op = 'y';
            std::cout << "当前连接WIFI名称为：" << ssid << std::endl;
            std::cout << "是否连接手机摄像头(y or n)：" << std::endl;
            std::cin >> op;
            if (op == 'y' || op == 'Y') ret = true;
            else ret = false;  
            break;
        }
    }
    return ret;
}