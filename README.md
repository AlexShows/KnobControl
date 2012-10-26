KnobControl
===========

A Windows-based application to attach to a Griffin PowerMate knob, listen for HID packets and respond with various behaviors. 

The PowerMate is enumerated by Windows as a USB Input Device.
It successfully loads a generic USB Input Device Driver on Windows 7.
There is a special driver from Griffin, but it doesn't support Windows 7.
The goal of this project is to open the device, read from it, and do something useful with that information, like adjusting volume or cycling through a menu, etc.

Important Findings
==================

It seems that the Griffin PowerMate driver is only available for Windows XP, Vista and some older versions of Mac OS.
On Windows 7, the OS loads a HID driver that takes exclusive access over the device.
I need to find some workaround for this, either a custom driver for Win7, or other method of accessing the device.

License
=======
KnobControl is released under the MIT License:
http://opensource.org/licenses/MIT