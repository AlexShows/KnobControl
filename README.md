KnobControl
===========

A Windows-based application to attach to a Griffin PowerMate knob, listen for HID packets and respond with various behaviors. 

The PowerMate shows up a generic HID device and successfully loads the generic HID driver on Windows.
Thus we can open the device and read packets from it. 
This project will first need to dissect the packets and determine their structure.
Then it will need to do something useful with that information, like adjusting volume or cycling through a menu, etc.
