/* List Wacom devices running on the system

To compile:
	gcc -o xinput-list-wacom-devices xinput-list-wacom-devices.c -lX11 -lXi

To run the sample:
	./xinput-list-wacom-devices
*/

#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

int main() {

	int ndevices, i, opcode, event, error;
	XIDeviceInfo *devices, device;

	/* Connect to the X server */
	Display *display = XOpenDisplay(NULL);

	/* Check for XInput */
	if (!XQueryExtension(display, "XInputExtension", &opcode, &event, &error)) {
		printf("X Input extension not available.\n");
		return -1;
	}

	/* Is XInput 2 available? */
	int major = 2, minor = 0;
	if (XIQueryVersion(display, &major, &minor) == BadRequest) {
		printf("XInput 2 not available\n");
		return -1;
	}

	devices = XIQueryDevice(display, XIAllDevices, &ndevices);

	for (i = 0; i < ndevices; i++) {
		device = devices[i];

		if (strncmp("Wacom", device.name, 5) != 0)
			continue;

		printf("id: %d - %s \n", device.deviceid, device.name);
	}

	XIFreeDeviceInfo(devices);
}
