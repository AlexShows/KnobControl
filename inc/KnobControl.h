// KnobControl.h : Header with includes for KnobControl cpp
// Author : Alex Shows
// License: Released under the MIT License (http://opensource.org/licenses/mit-license.php)

#include <stdio.h>
#include <tchar.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <SetupAPI.h> // needs setupapi.lib

// The extern C is important here
extern "C"{
#include "hidsdi.h"			// needs hid.lib from the Win DDK
}

#include <iostream>
using namespace std;

#define MATCHING_VID 0x077D // Griffin PowerMate VID
#define MATCHING_PID 0x0410 // Griffin PowerMate PID
#define KC_STRING_SIZE 512 // Found some devices that fail for 256, using 512 now
#define KC_MAX_DEVICES 20 // How deep should we search into the PnP chain?
#define KC_PACKET_SIZE 20 // TODO: Need to determine packet size and revisit this