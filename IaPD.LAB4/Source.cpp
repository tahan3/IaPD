#include <stdio.h>
#include <Windows.h>
#include <setupapi.h>
#include <locale.h> 
#include <iostream>
#include <wdmguid.h>
#include <devguid.h>
#include <iomanip>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#pragma comment(lib, "setupapi.lib")

using namespace std;
using namespace cv;

#define ESC 0x1b
#define P 0x50
#define R 0x52
#define H 0x48
#define I 0x49

void help() {
    cout << "ESC - exit" << endl
        << "P - take photo" << endl
        << "R - record video" << endl
        << "H - hide" << endl
        << "I - info" << endl;
}

void webcamInfo() {
    SP_DEVINFO_DATA DeviceInfoData = { 0 };//
    HDEVINFO DeviceInfoSet = SetupDiGetClassDevs(&GUID_DEVCLASS_CAMERA, "USB", NULL, DIGCF_PRESENT);
    if (DeviceInfoSet == INVALID_HANDLE_VALUE) {
        cout << GetLastError();
        return;
    }
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    SetupDiEnumDeviceInfo(DeviceInfoSet, 0, &DeviceInfoData);
    TCHAR deviceID[256];
    TCHAR deviceName[256];
    TCHAR deviceMfg[256];
    SetupDiGetDeviceInstanceId(DeviceInfoSet, &DeviceInfoData, deviceID, sizeof(deviceID), NULL);
    SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)deviceName, sizeof(deviceName), NULL);
    SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_MFG, NULL, (PBYTE)deviceMfg, sizeof(deviceMfg), NULL);

    string id(deviceID);
    cout << "Device name: " << deviceName << endl;
    cout << "Device id: " << id.substr(17, 4) << endl;
    cout << "Vendor name: " << deviceMfg << endl;
    cout << "Vendor id: " << id.substr(8, 4) << endl << endl;
    SetupDiDestroyDeviceInfoList(DeviceInfoSet);
}

void recordVideo() {
    VideoCapture camera(0);
    Mat frame;
    int frame_width = camera.get(CAP_PROP_FRAME_WIDTH);
    int frame_height = camera.get(CAP_PROP_FRAME_HEIGHT); 
    VideoWriter video("vidos.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 24, Size(frame_width, frame_height));

    time_t timeStart;
    time(&timeStart);
    while (time(NULL) <= timeStart + 4)//5 sec duration
    {
        camera >> frame;
        if (frame.empty())
            break;
        video.write(frame);
    }
    camera.release();
    video.release();
    destroyAllWindows();
    system("cls");
    help();
}

void takePhoto() {
    VideoCapture camera(0);
    Mat frame;
    camera.read(frame);
    imwrite("photo.jpg", frame);
}

bool hide() {
    static bool visible = true;
    if (visible) visible = false;
    else visible = true;
    return visible;
}

LRESULT CALLBACK LLKeyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        switch (wParam) {
        case WM_KEYUP:
        case WM_SYSKEYUP:
            DWORD key = ((LPKBDLLHOOKSTRUCT)lParam)->vkCode;
            if (key == ESC) {//ESC
                PostQuitMessage(0);
            }
            if (key == P) {//P
                takePhoto();
            }
            if (key == R) {//R
                recordVideo();
            }
            if (key == I) {//I
                system("cls");
                help();
                webcamInfo();
            }
            if (key == H) {//hide
                ShowWindow(GetConsoleWindow(), hide());
            }
            break;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main()
{
    setlocale(LC_ALL, "rus");
    help();
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, LLKeyProc, GetModuleHandle(NULL), 0);
    while (WaitMessage()) {
        MSG msg;
        while (!GetMessage(&msg, NULL, NULL, NULL)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
            {
                UnhookWindowsHookEx(hook);
                return 0;
            }
        }
    }
}