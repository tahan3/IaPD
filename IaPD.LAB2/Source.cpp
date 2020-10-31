#include <windows.h>
#include <iostream>
#include <ntddscsi.h>

using namespace std;

const int GB = 1024;
const int KB = 100;
const int BYTE_SIZE = 8;

const char* busType[] = { "UNKNOWN", 
							"SCSI", 
							"ATAPI", 
							"ATA", 
							"1394", 
							"SSA", 
							"FIBRE", 
							"USB", 
							"RAID", 
							"ISCSI", 
							"SAS", 
							"SATA", 
							"SD", 
							"MMC", 
							"VIRTUAL", 
							"FILEBACKEDVIRTUAL", 
							"SPACES", 
							"NVME", 
							"SCM", 
							"UFS" };

void getAtaPioDmaSupportStandarts(HANDLE diskHandle)
{
	UCHAR identifyDataBuffer[512 + sizeof(ATA_PASS_THROUGH_EX)] = { 0 };

	ATA_PASS_THROUGH_EX& PTE = *(ATA_PASS_THROUGH_EX*)identifyDataBuffer;
	PTE.Length = sizeof(PTE);
	PTE.TimeOutValue = 10; 
	PTE.DataTransferLength = 512; 
	PTE.DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);
	PTE.AtaFlags = ATA_FLAGS_DATA_IN; 

	IDEREGS* ideRegs = (IDEREGS*)PTE.CurrentTaskFile;
	ideRegs->bCommandReg = 0xEC; 

	if (!DeviceIoControl(
		diskHandle,
		IOCTL_ATA_PASS_THROUGH,
		&PTE,
		sizeof(identifyDataBuffer),
		&PTE,
		sizeof(identifyDataBuffer),
		NULL,
		NULL
		))
	{
		cout << GetLastError() << std::endl;
		return;
	}

	WORD* data = (WORD*)(identifyDataBuffer + sizeof(ATA_PASS_THROUGH_EX));
	short ataSupportByte = data[80];
	int i = 2 * BYTE_SIZE;
	int bitArray[2 * BYTE_SIZE];

	unsigned short dmaSupportedBytes = data[63];
	int i2 = 2 * BYTE_SIZE;

	while (i2--)
	{
		bitArray[i2] = dmaSupportedBytes & 32768 ? 1 : 0;
		dmaSupportedBytes = dmaSupportedBytes << 1;
	}

	cout << "DMA support: ";
	for (int i = 0; i < 8; i++)
	{
		if (bitArray[i] == 1)
		{
			cout << "DMA" << i;
			if (i != 2) cout << ", ";
		}
	}
	cout << endl;

	unsigned short pioSupportedBytes = data[64];
	int i3 = 2 * BYTE_SIZE;

	while (i3--)
	{
		bitArray[i3] = pioSupportedBytes & 32768 ? 1 : 0;
		pioSupportedBytes = pioSupportedBytes << 1;
	}

	cout << "PIO support: ";
	for (int i = 0; i < 2; i++)
	{
		if (bitArray[i] == 1)
		{
			cout << "PIO" << i + 3;
			if (i != 1) cout << ", ";
		}
	}
	cout << endl << endl;
}

HANDLE initialize() {
	wchar_t name[] = L"\\\\.\\PhysicalDrive0";
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	hDevice = CreateFileW((LPCWSTR)name,
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | 
		FILE_SHARE_WRITE, 
		NULL,             
		OPEN_EXISTING,    
		0,                
		NULL);          

	if (hDevice == INVALID_HANDLE_VALUE)   
	{
		cout << GetLastError() << endl;
		return INVALID_HANDLE_VALUE;
	}
	return hDevice;
}

void getDevInfo(HANDLE hDevice) {
	

	STORAGE_PROPERTY_QUERY storagePropertyQuery; 
	storagePropertyQuery.QueryType = PropertyStandardQuery; 
	storagePropertyQuery.PropertyId = StorageDeviceProperty; 

	STORAGE_DEVICE_DESCRIPTOR* deviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)calloc(1000, 1);
	deviceDescriptor->Size = 1000;

	
	DeviceIoControl(hDevice,
					IOCTL_STORAGE_QUERY_PROPERTY,
					&storagePropertyQuery,
					sizeof(storagePropertyQuery),
					deviceDescriptor,
					1000,//
					NULL,//.
					NULL);

	if (deviceDescriptor->ProductIdOffset)
		cout << "Product ID:\t" << (char*)deviceDescriptor + deviceDescriptor->ProductIdOffset << endl;
	if (deviceDescriptor->VendorIdOffset)
		cout << "Vendor ID:\t" << (char*)deviceDescriptor + deviceDescriptor->VendorIdOffset << endl;
	if (deviceDescriptor->SerialNumberOffset)
		cout << "Serial number:\t" << (char*)deviceDescriptor + deviceDescriptor->SerialNumberOffset << endl;
	if (deviceDescriptor->ProductRevisionOffset)
		cout << "FW version:\t" << (char*)deviceDescriptor + deviceDescriptor->ProductRevisionOffset << endl;
	cout << "Interface type:\t" << busType[deviceDescriptor->BusType] << endl << endl;
}

void getMemInfo() {
	//SPACE
	ULARGE_INTEGER freeBytesAvailableToCaller;
	ULARGE_INTEGER totalNumberOfBytes;        
	ULARGE_INTEGER totalNumberOfFreeBytes;

	if (!GetDiskFreeSpaceEx("C:", &freeBytesAvailableToCaller, &totalNumberOfBytes, &totalNumberOfFreeBytes)) cout << GetLastError() << endl;

	totalNumberOfBytes.QuadPart = totalNumberOfBytes.QuadPart / (GB*GB);
	totalNumberOfFreeBytes.QuadPart = totalNumberOfFreeBytes.QuadPart / (GB * GB);

	cout << "Free space:\t" << totalNumberOfFreeBytes.QuadPart * 1.0 / GB << " GB" << endl;
	cout << "Used space:\t" << (totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart) * 1.0 / GB << " GB" << endl;
	cout << "Total space:\t" << totalNumberOfBytes.QuadPart * 1.0 / GB << " GB" << endl;

}

int main()
{
	HANDLE hDevice = initialize();
	if (hDevice == INVALID_HANDLE_VALUE) return 0;

	getDevInfo(hDevice);
	getAtaPioDmaSupportStandarts(hDevice);
	getMemInfo();
	CloseHandle(hDevice);

	return 0;
}