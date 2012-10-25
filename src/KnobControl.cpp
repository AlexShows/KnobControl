// KnobControl.cpp
// Author: Alex Shows
// License: Released under the MIT License (http://opensource.org/licenses/mit-license.php)
// Description: Connect to a Griffin PowerMate, read packets, and do something interesting with them

#include "KnobControl.h"

int _tmain(int argc, _TCHAR* argv[])
{
	HIDComm* pHC = new HIDComm(POWERMATE_VID, POWERMATE_PID);
	
/*
	BOOL readSuccess = false;
	DWORD numBytes = KC_PACKET_SIZE;
	DWORD bytesRead = 0;
	BYTE readBuf[KC_PACKET_SIZE];
	ZeroMemory(readBuf, KC_PACKET_SIZE);

	// Read a report from the device and interpret
	while(1)
	{
		readSuccess = ReadFile(HIDHandle, &readBuf, numBytes,&bytesRead, NULL);
		
		if(readSuccess)
		{
			// Do something interesting with the packet read from the device
		}

		// TODO: Set a break condition for this infinite loop
	}
*/
	
	return 0;
}
