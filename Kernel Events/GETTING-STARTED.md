# Getting Started

## Test Environment
A Linux system running a kernel version 4.19 or later would work out of the box with tablets released since 2016. If you have a system running an older kernel, the chances are you'll need to [update the kernel driver](https://github.com/linuxwacom/input-wacom/wiki/Installing-input-wacom-from-source).

A Wacom tablet with pen, touch, and ExpressKeys to show kernel events.

## Install the Wacom tablet driver and verify tablet operation
In order to get the complete features that Wacom tablet provides, updating to the latest Wacom kernel driver is recommended.

## Build/run the sample application
Commands to build and run the sample applications are added to the first line of the source code as comments.

## See Also
See the [analyzing kernel events](https://github.com/linuxwacom/input-wacom/wiki/Analysing-kernel-events) page to make sure your kernel driver is reporting the correct events.

If you see issues with the kernel events or don't get any events at all, check if the [latest kernel driver](https://github.com/linuxwacom/input-wacom/wiki/Installing-input-wacom-from-source) is installed and the [proper device is registered](https://github.com/linuxwacom/input-wacom/wiki/Testing-Tablet-Detection).

Kernel Events - FAQs: See the [How do I know if my device is supported or not](https://developer-support.wacom.com/hc/en-us/articles/12845589645207-Kernel-Events) section.

