#include "video.hpp"
using namespace Video;

void Videos::star(bool op, unsigned weight, unsigned height){
    std::string url = "0";
    if (!op) url = isPhone() ? PHONE : WIFI;

    cap.open(url, cv::CAP_FFMPEG);
    cap.read(last_frame);
    cv::resize(last_frame, last_frame, cv::Size(WEIGHT, HEIGHT));
}

void Videos::show(unsigned weight, unsigned height) {
    cv::namedWindow("swap face");
    while(true) {
        if (count % 3 == 0) {
            if (!frame_queue.empty()) {
                last_frame = frame_queue.front();
                frame_queue.pop();
            } else {
                cap.read(last_frame);
            }

            cv::imshow("swap face", last_frame);
        }

        if (cv::waitKey(1) == 27) {
            close();
            break;
        }
    }
}

void Videos::close() {
    cap.release();
    last_frame.release();
    while(!frame_queue.empty()) frame_queue.pop();
}

bool Videos::isPhone(){
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