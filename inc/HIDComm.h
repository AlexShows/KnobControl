// HIDComm.h : Header for the HIDComm class, enabling communication with HID devices
// Author : Alex Shows
// License : Released under the MIT License (http://opensource.org/licenses/mit-license.php)
// Support : Current implementation is specific to Win32
// TODO : Expand class to Win64, then to Linux

#pragma once
#ifndef HIDCOMM_H
#define HIDCOMM_H

#include <stdio.h>
#include <tchar.h>

#include <iostream>
using namespace std;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <SetupAPI.h> // needs setupapi.lib

// The extern C is important here...
extern "C"{
#include "hidsdi.h"			// needs hid.lib from the Win DDK
}

#define HC_SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;

#define HIDCOMM_STRING_SIZE 512 // Found some devices that fail for 256, using 512 now
#define HIDCOMM_PACKET_SIZE 20 // TODO: Need to get away defining this, determine and allocate dynamically

class HIDComm {

	public:
		HIDComm(); // Initialize connection to PnP and wait for more information before connecting to the device
		HIDComm(unsigned short vendorID, unsigned short productID); // Initialize connection and attempt to connect to device by VID/PID
		~HIDComm();
		HANDLE ConnectToDevice(unsigned short vendorD, unsigned short productID); // Connect to a device based on VID/PID
		bool IsInitialized(){return bInitHID;};
		bool IsDeviceConnected(){return bDeviceConnected;};
		bool IsReadWriteSupported(){return bReadWrite;};
	private:
		void Reset(); // Resets the state of the object, expecting to then be reconnected to a device
		GUID hidGUID; // GUID for the HID subsystem (for attaching to PnP)
		HANDLE hPnP; // Handle to the Plug-n-Play subsystem
		HANDLE hDevice; // Handle to the particular HID device for the object created and initalized with this class
		bool bInitHID; // Has the HID PnP system been connected to successfully?
		bool bDeviceConnected; // Has the device been found and connected to?
		bool bReadWrite; // Read/Write supported by the particular HID device (i.e. OS doesn't have exclusive access)
		wchar_t* pStrManuf; // Manufacturer string for the particular HID device
		wchar_t* pStrProd; // Product string for the particular HID device
		unsigned short usVendorID;
		unsigned short usProductID;
		unsigned short usVersion;
		
		void PrintLastError(DWORD dwLastError);

};

#endif /* HIDCOMM_H */