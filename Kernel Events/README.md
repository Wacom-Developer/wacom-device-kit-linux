# Readme

## Application Details
There are 6 applications in the zipped file. Each shows a group of the kernel events that a Wacom tablet may report.

* **supported-event-types.c** – Displays all kernel event types a Wacom tablet supports. This program only prints the raw kernel events. To get a graphic view of the multi-touch kernel events, please refer to https://github.com/whot/mtview.

* **pressure.c** – Displays pen raw coordinate and pressure values for both the tip and the eraser.

* **devices.c** – Shows how to find the Wacom devices that are registered by the running kernel. It also shows the node numbers (dev/input/event#) associated with the devices.

* **ioctl.c** – Is an example for retrieving a tool's last coordinate and pressure values posted from the kernel.

* **event-log.c** – Shows how to get the raw pen and expresskey kernel events.

* **find-leds.c** – Checks if a tablet supports LEDs or not. If it does, the code shows how to retrieve their modes;

## See Also
[Tool and Pen Compatibility](https://github.com/linuxwacom/input-wacom/wiki/Tool-and-Pen-Compatibility) - Details of what features a tablet and its tools support

[Wacom Linux kernel driver wiki](https://github.com/linuxwacom/input-wacom/wiki) - A community project for everything related to running a Wacom device on Linux kernel

## Where To Get Help
If you have questions about this demo, please visit our support page: https://developer.wacom.com/developer-dashboard/support. 

## License
The sample programs are licensed under the MIT License: https://choosealicense.com/licenses/mit, unless another license is specifically attached to the source.