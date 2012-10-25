// HIDComm.cpp : Class implementation for the HIDComm class, enabling communication with HID devices
// Author : Alex Shows
// License : Released under the MIT License (http://opensource.org/licenses/mit-license.php)
// Support : Current implementation is specific to Win32
// TODO : Expand class to Win64, then to Linux

// One of the odd things about Win64 is there is apparently no HID.lib for Win64,
// so how would a 64-bit application be able to access these HID functions to communicate?
// Perhaps in the future this should be some kind of service or IPC enabling communications
// from a 64-bit application into a 32-bit process that calls into the HID stack?

#include "HIDComm.h"

/*********************
Base constructor, only attaches to PnP HID subsystem
but doesn't attempt to connect to a device
**********************/
HIDComm::HIDComm()
{
	Reset();	
}

/*********************
Another constructor which attaches to PnP HID subsystem
and attempts to connect to the device specified by VID & PID
**********************/
HIDComm::HIDComm(unsigned short vendorID, unsigned short productID)
{
	Reset();

	if(bInitHID)
		hDevice = ConnectToDevice(vendorID, productID);
}

/*********************
Clean up
**********************/
HIDComm::~HIDComm()
{
	HC_SAFE_DELETE(pStrManuf);
	HC_SAFE_DELETE(pStrProd);
	
	if(hDevice != INVALID_HANDLE_VALUE)
		CloseHandle(hDevice);
		
	if(hPnP != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hPnP);
}

/*********************
Reset the object state in preparation for connecting to a device
**********************/
void HIDComm::Reset()
{
	// Reset may have been called before these were initialized
	HC_SAFE_DELETE(pStrManuf);
	HC_SAFE_DELETE(pStrProd);
	
	if(hDevice != INVALID_HANDLE_VALUE)
		CloseHandle(hDevice);
		
	if(hPnP != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hPnP);
		
	bInitHID = false;
	bDeviceConnected = false;
	bReadWrite = false;
	hPnP = INVALID_HANDLE_VALUE;
	hDevice = INVALID_HANDLE_VALUE;
	pStrManuf = new wchar_t[HIDCOMM_STRING_SIZE];
	pStrProd = new wchar_t[HIDCOMM_STRING_SIZE];
	usVendorID = 0;
	usProductID = 0;
	usVersion = 0;
	
	// First try to get the HID GUID
	HidD_GetHidGuid(&hidGUID);
	
	// Attach to the Plug and Play node and get devices
	hPnP = SetupDiGetClassDevs(&hidGUID, NULL, NULL, 
				DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);	
	
	if(hPnP != INVALID_HANDLE_VALUE)
		bInitHID = true;
}

/*********************
Function assumes that the HID subystem is initialized, and
it attempts first to connect (without read/write) to the 
device specified, and if it finds a match, then attempts to 
connect with read/write permissions

If there's an error, it returns INVALID_HANDLE_VALUE,
otherwise it will return a handle to the device and set 
the read/write flag if available
**********************/
HANDLE HIDComm::ConnectToDevice(unsigned short vendorID, unsigned short productID)
{
	// Because this is a public function, it may have been called after
	// a device is already connected, in which case we reset
	if(bDeviceConnected || hDevice != INVALID_HANDLE_VALUE)
		Reset();
		
	if(!bInitHID)
		return INVALID_HANDLE_VALUE;

	HANDLE currDev; // Local handle used for stepping through HID devices until a match is found
			
	struct{ 
		DWORD cbSize; 
		char DevicePath[HIDCOMM_STRING_SIZE];
	} MyHIDDeviceData; /* Device class location for opening */
	MyHIDDeviceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
	
	HIDD_ATTRIBUTES HIDAttributes; /* Attributes of the HID device */
	SP_INTERFACE_DEVICE_DATA DeviceInterfaceData; /* holds device interface data for the current device */ 
	DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);

	int iHIDdev = 0;
	BOOL success, endOfList = false;
	ULONG bRet; /* how many bytes were returned from the device interface detail request? */

	/* Security attributes for opening the device for raw file I/O */
	SECURITY_ATTRIBUTES SecurityAttributes;
	SecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES); 
	SecurityAttributes.lpSecurityDescriptor = NULL; 
	SecurityAttributes.bInheritHandle = false; 

	/* Loop through each HID device by index, searching for the device of interest */
	while (!endOfList)
	{
		/* Note: this function returns a handle from _inside_ this loop if it finds a matching VID/PID */
		
		/* Test for a device at this index */
		success = SetupDiEnumDeviceInterfaces(hPnP, 
			NULL, 
			&hidGUID, 
			iHIDdev, 
			&DeviceInterfaceData);

		/* Can we enumerate the device? */
		if(success)
		{
			cout << "Successfully Enumerated Device Interface for index " << iHIDdev << endl;
			
			/* Found a device, so get the device details like vendor ID and product ID */
			success = SetupDiGetDeviceInterfaceDetail(hPnP, 
						&DeviceInterfaceData, 
						(PSP_INTERFACE_DEVICE_DETAIL_DATA)&MyHIDDeviceData, 
						HIDCOMM_STRING_SIZE, 
						&bRet, 
						NULL);

			if(!success)
			{
				cout << "Unable to Get Device Interface Detail for HID device index " << iHIDdev << endl;
				PrintLastError(::GetLastError());
			}
			else
			{
				/* First, try to open the device without read/write or sharing specified, 
				just to get the attributes */
				currDev = CreateFile((LPCSTR)&MyHIDDeviceData.DevicePath, 
					0,
					FILE_SHARE_READ|FILE_SHARE_WRITE, 
					&SecurityAttributes, 
					OPEN_EXISTING, 
					0, NULL);

				if(currDev != INVALID_HANDLE_VALUE)
				{
					/* Get the information about this HID */
					success = HidD_GetAttributes(currDev, &HIDAttributes);

					/* If it matches, mark the match and attempt to get a new handle with read/write access */
					if (success && HIDAttributes.VendorID == vendorID && HIDAttributes.ProductID == productID)
					{
						cout << "Found a match for the vendor and product ID provided!" << endl;
						bDeviceConnected = true; // While it IS connected, we may not have read/write access
						usVendorID = HIDAttributes.VendorID;
						usProductID = HIDAttributes.ProductID;
						usVersion = HIDAttributes.VersionNumber;
						HidD_GetManufacturerString(currDev, pStrManuf, HIDCOMM_STRING_SIZE);
						HidD_GetProductString(currDev, pStrProd, HIDCOMM_STRING_SIZE);
						
						wcout << "Manufacturer is " << pStrManuf << "." << endl;
						wcout << "Product is " << pStrProd << "." << endl;
						
						/* Next, try to open the same device with read/write permissions */
						HANDLE hRW = CreateFile((LPCSTR)&MyHIDDeviceData.DevicePath, 
									GENERIC_READ|GENERIC_WRITE,
									FILE_SHARE_READ|FILE_SHARE_WRITE, 
									&SecurityAttributes, 
									OPEN_EXISTING, 
									0, NULL);
						
						/* If it's not invalid, we can close the other other handle,
						get more information about the device and return the read/write handle */
						if(hRW != INVALID_HANDLE_VALUE)
						{
							cout << "Read/write access is possible with this device." << endl;	
							bReadWrite = true;
							CloseHandle(currDev);
							return hRW;
						}
						
						/* If we're returning the current device here, it's because even though we
						can connect to it and get the VID/PID and see it matches, we can't get 
						read/write access, thus the RW flag should be false but init and device connected
						flags should be true */
						cout << "Unable to get read/write access to the device." << endl;
						return currDev;
					}

					/* Otherwise, the device doesn't match the VID/PID specific */
					printf("Device found does not match: VID 0x%x, PID 0x%x\n", HIDAttributes.VendorID, HIDAttributes.ProductID);	
						
					/* Close this handle and try the next one */
					CloseHandle(currDev);
					
				} /* fi valid handle */
				else
				{
					cout << "Invalid handle attempting to create file for HID device index " << iHIDdev << endl;
					PrintLastError(::GetLastError());
				}
			} /* fi successful get device interface detail */

		} /* fi successful enum interfaces */
		else
		{
			/* We might be here because there are no more devices in the list... */
			DWORD dwLastErr = ::GetLastError();
			if(dwLastErr == ERROR_NO_MORE_ITEMS)
			{
				cout << "No more items in the list!" << endl;
				endOfList = true;
			}
			else
			{
				cout << "Unable to Enumerate Interfaces for HID device index " << iHIDdev << endl;
				PrintLastError(dwLastErr);
			}
		}
		
		iHIDdev++;

	} /* end while device not found and not the end of the list */

	return INVALID_HANDLE_VALUE;	
}

/**********************
Using the last error code, print the error to stdout
***********************/
void HIDComm::PrintLastError(DWORD dwLastError)
{
	char lpBuffer[256] = _T("?");
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastError, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), lpBuffer, 255, NULL);
	printf("-:- ERROR 0x%x: ", dwLastError);
	printf(lpBuffer);
}
