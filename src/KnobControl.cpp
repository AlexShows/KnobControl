// KnobControl.cpp
// Author: Alex Shows
// License: Released under the MIT License (http://opensource.org/licenses/mit-license.php)
// Description: Connect to a Griffin PowerMate, read packets, and do something interesting with them

#include "KnobControl.h"

HANDLE AttachToPnP(const GUID* mGUID);
HANDLE GetDeviceHandle(HANDLE mHandle, const GUID* mGUID);
void PrintLastError(DWORD dwLastError);

int _tmain(int argc, _TCHAR* argv[])
{
	/*********************
	HID Init
	**********************
	Initialize HID
	**********************/

	GUID hidGUID;
	HANDLE PnPHandle = INVALID_HANDLE_VALUE;
	HANDLE HIDHandle = INVALID_HANDLE_VALUE;
	bool connected = false;
	wchar_t sManuf[ KC_STRING_SIZE ] = L"";
	wchar_t sProd[ KC_STRING_SIZE ] = L"";

	HidD_GetHidGuid(&hidGUID);
	PnPHandle = AttachToPnP(&hidGUID);

	if(PnPHandle != INVALID_HANDLE_VALUE)
	{
		HIDHandle = GetDeviceHandle(PnPHandle, &hidGUID);

		if(HIDHandle != INVALID_HANDLE_VALUE)
		{
			cout << "Successfully connected to the device using VID/PID match" << endl;
			connected = true;
		}
		else
		{
			cout << "Failed to connect to the device using VID/PID match" << endl;
			return -1;
		}

		if(HidD_GetManufacturerString(HIDHandle, sManuf, KC_STRING_SIZE))
			printf("%ws, ", sManuf);
		else
		{
			cout << "Unable to Get Manufacturer String" << endl;
			PrintLastError(::GetLastError());
		}

		if(HidD_GetProductString(HIDHandle, sProd, KC_STRING_SIZE))
			printf("%ws\n", sProd);
		else
		{
			cout << "Unable to Get Product String" << endl;
			PrintLastError(::GetLastError());
		}
	}

	BOOL readSuccess = false;
	DWORD numBytes = KC_PACKET_SIZE;
	DWORD bytesRead = 0;
	BYTE readBuf[KC_PACKET_SIZE];
	ZeroMemory(readBuf, KC_PACKET_SIZE);

	/* Read a report from the device and interpret */
	while(1)
	{
		readSuccess = ReadFile(HIDHandle, &readBuf, numBytes,&bytesRead, NULL);
		
		if(readSuccess)
		{
			// Do something interesting with the packet read from the device
		}

		// TODO: Set a break condition for this infinite loop
	}

	/*********************
	HID CLEANUP
	**********************
	This is the cleanup routine for the HID attach functions.
	**********************/
	if(HIDHandle != INVALID_HANDLE_VALUE)
		CloseHandle(HIDHandle);
		
	if(PnPHandle != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(PnPHandle);
	
	return 0;
}

/*********************
	Attach To PnP
**********************/
HANDLE AttachToPnP(const GUID* mGUID)
{
	/* Attach to the Plug and Play node and get devices */
	HANDLE pnp = SetupDiGetClassDevs(mGUID, 
				NULL, NULL, 
				DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);

	if(pnp == INVALID_HANDLE_VALUE)
		printf("Error attaching to PnP node");

	return pnp;
}

/********************
Using the GUID and Plug'n'Play handle, 
loop through the available HID device 
handles until a match is found 
**********************/
HANDLE GetDeviceHandle(HANDLE mPnPHandle, const GUID* mGUID)
{
	HANDLE hDevice = NULL;
	struct{ 
		DWORD cbSize; 
		char DevicePath[KC_STRING_SIZE];
	} MyHIDDeviceData; /* Device class location for opening */

	HIDD_ATTRIBUTES HIDAttributes; /* Attributes of the HID device */
	SP_INTERFACE_DEVICE_DATA DeviceInterfaceData; /* holds device interface data for the current device */ 

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
		DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);

		/* Test for a device at this index */
		success = SetupDiEnumDeviceInterfaces(mPnPHandle, 
			NULL, 
			mGUID, 
			iHIDdev, 
			&DeviceInterfaceData);

		if(success)
		{
			cout << "Successfully Enumerated Device Interface for index " << iHIDdev << endl;
			
			/* Found a device, so get the name */
			MyHIDDeviceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			success = SetupDiGetDeviceInterfaceDetail(mPnPHandle, 
						&DeviceInterfaceData, 
						(PSP_INTERFACE_DEVICE_DETAIL_DATA)&MyHIDDeviceData, 
						KC_STRING_SIZE, 
						&bRet, 
						NULL);

			if(!success)
			{
				cout << "Unable to Get Device Interface Detail for HID device index " << iHIDdev << endl;
				PrintLastError(::GetLastError());
			}
			else
			{
				/* Open the device */
				hDevice = CreateFile((LPCSTR)&MyHIDDeviceData.DevicePath, 
					GENERIC_READ|GENERIC_WRITE,
					FILE_SHARE_READ|FILE_SHARE_WRITE, 
					&SecurityAttributes, 
					OPEN_EXISTING, 
					0, NULL);

				if(hDevice != INVALID_HANDLE_VALUE)
				{
					/* Get the information about this HID */
					success = HidD_GetAttributes(hDevice, &HIDAttributes);

					/* If it matches, return the handle */
					if (success && HIDAttributes.VendorID == MATCHING_VID && HIDAttributes.ProductID == MATCHING_PID)
						return hDevice;

					printf("Device found does not match: VID 0x%x, PID 0x%x\n", HIDAttributes.VendorID, HIDAttributes.ProductID);	
						
					CloseHandle(hDevice);
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

/*********************
Get the Last Error and print 
it out to the console
**********************/
void PrintLastError(DWORD dwLastError)
{
	char lpBuffer[256] = _T("?");
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastError, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), lpBuffer, 255, NULL);
	printf("-:- ERROR 0x%x: ", dwLastError);
	printf(lpBuffer);
}