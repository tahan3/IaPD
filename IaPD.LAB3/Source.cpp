#pragma comment (lib, "SetupAPI.lib")
#pragma comment (lib, "Kernel32.lib")
#pragma comment(lib, "PowrProf.lib")

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <powrprof.h>
#include <conio.h>

using namespace std;

int main() {
	_SYSTEM_POWER_STATUS power_status;
	while (1) {
		system("cls");

		if (!GetSystemPowerStatus(&power_status)) {
			cout << GetLastError();
			exit(-1);
		}

		cout << "----------Battery information----------" << endl;

		if (!(int)power_status.ACLineStatus) {
			cout << "The device is not connected to power source" << endl;
		}
		else cout << "The device is connected to power source" << endl;

		cout << "Battery status: ";
		switch ((int)power_status.BatteryFlag) {
		case 1: cout << "The battery capacity is at more than 66 percent(High) " << endl; break;
		case 2: cout << "The battery capacity is at less than 33 percent(Low) " << endl; break;
		case 4: cout << "The battery capacity is at less than five percent(Critical) " << endl; break;
		case 8: cout << "Charging " << endl; break;
		case 128: cout << "No system battery " << endl; break;
		case 255: cout << "Unable to read the battery flag information:" << endl; break;
		}

		cout << "Battery life percentage: " << (int)power_status.BatteryLifePercent << "%" << endl;

		if (!(int)power_status.SystemStatusFlag) {//энергосбережение
			cout << "Battery saver is off" << endl;
		}
		else cout << "Battery saver is on" << endl;

		if ((int)power_status.BatteryLifeTime != -1) {//оставшееся количество секунд работы
			cout << "Battery life remained: " << (int)power_status.BatteryLifeTime / 3600 << " h" << (int)power_status.BatteryLifeTime % 3600/60<<"min"<< (int)power_status.BatteryLifeTime % 3600 % 60<<"sec" << endl;
		}
		else cout << "Remaining battery life is unknown or the device is connected to power source" << endl;

		if ((int)power_status.BatteryFullLifeTime != -1) {//кол-во от полной
			cout << "Battery full life time:" << (int)power_status.BatteryFullLifeTime << " minutes" << endl;
		}
		else if ((int)power_status.BatteryLifeTime != -1) {
			cout << "Battery full life time: "
				<< (int)power_status.BatteryLifeTime * 100 / (int)power_status.BatteryLifePercent / 60 << " minutes" << endl;
		}
		else cout << "Battery full life time is unknown or the device is connected to power source" << endl;
		
		cout << endl << "1.Sleep" << endl << "2.Hibernation mode" << endl << "3.Exit." << endl;
		if (_kbhit()) {
			switch (_getch()) {
			case '1':
				SetSuspendState(true, false, false);
				break;
			case '2':
				SetSuspendState(false, false, false);
				break;
			case '3':
				return 0;
			}
		}
		Sleep(10);
	}
}