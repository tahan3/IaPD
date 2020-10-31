#pragma comment (lib, "Setupapi.lib")
#include <iostream>
#include <Windows.h>
#include <setupapi.h>
#include <regstr.h>

using namespace std;

int main() {

	string gate = "-----------------------------------------------------------------------------------------------------------------------\n\n";
	const int buf = 100;
	int i = 0;

	setlocale(LC_ALL, "Russian");

	HDEVINFO device_info; 
	device_info = SetupDiGetClassDevs(NULL, "USB", 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);

	SP_DEVINFO_DATA device_info_data;
	device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);

	cout << gate;

	TCHAR device_id[buf], device_name[buf], vendor_name[buf]; 

	
	while (SetupDiEnumDeviceInfo(device_info, i, &device_info_data)) { 

		SetupDiGetDeviceInstanceId(device_info, &device_info_data, device_id, sizeof(device_id), NULL);
		SetupDiGetDeviceRegistryProperty(device_info, &device_info_data, SPDRP_DEVICEDESC, NULL, (PBYTE)device_name, sizeof(device_name), NULL);
		SetupDiGetDeviceRegistryProperty(device_info, &device_info_data, SPDRP_MFG, NULL, (PBYTE)vendor_name, sizeof(vendor_name), NULL);

		string id(device_id);
		cout << "Device number: " << i << endl;
		cout << "Device name: " << device_name << endl;
		cout << "Device id: " << id.substr(17, 4) << endl;
		cout << "Vendor name: " << vendor_name << endl;
		cout << "Vendor id: " << id.substr(8, 4) << endl;
		cout << gate;
		i++;
	}
}