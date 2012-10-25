KnobControl
===========

A Windows-based application to attach to a Griffin PowerMate knob, listen for HID packets and respond with various behaviors. 

The PowerMate shows up a generic HID device and successfully loads the generic HID driver on Windows.
Thus we can open the device and read packets from it. 
This project will first need to dissect the packets and determine their structure.
Then it will need to do something useful with that information, like adjusting volume or cycling through a menu, etc.

Important Findings
==================

It seems that the Griffin PowerMate driver is only available for Windows XP, Vista and some older versions of Mac OS.
On Windows 7, the OS loads a HID driver that takes exclusive access over the device.
I need to find some workaround for this, either a custom driver for Win7, or other method of accessing the device.

License
=======
KnobControl is released under the MIT License:
http://opensource.org/licenses/MIT