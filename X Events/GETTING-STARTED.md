# Getting Started 

## Test Environment
A Linux system running a kernel version 4.19 or later and an Xorg X server 1.8 or later would work out of the box with most tablets released in 2016 or later. If you have a system running an older kernel, the chances are you'll need to update [the kernel driver](https://github.com/linuxwacom/input-wacom/wiki/Installing-input-wacom-from-source) and the [Wacom X driver](https://github.com/linuxwacom/xf86-input-wacom/wiki/Building-The-Driver).

A Wacom tablet with pen, touch, and expresskeys to show X Input events.

## Build/run the sample application
Commands to build and run the sample applications are added at the beginning of the source code, as comments.

## See Also
If you updated Wacom X driver and you still don't get X Input events in the sample programs, the kernel driver may be at fault.

[Testing Tablet Detection](https://github.com/linuxwacom/input-wacom/wiki/Testing-Tablet-Detection) - Verifies the kernel driver is loaded and tablet is registered

[Analyzing kernel events](https://github.com/linuxwacom/input-wacom/wiki/Analysing-kernel-events) - Evaluates the correct kernel events are reported

Kernel Events FAQs - See the [How do I know if my device is supported or not](https://developer-support.wacom.com/hc/en-us/articles/12845589645207-Kernel-Events) section.