// KnobControl.h : Header with includes for KnobControl cpp
// Author : Alex Shows

#pragma once

#include "targetver.h"

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