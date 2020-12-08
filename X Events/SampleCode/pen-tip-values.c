/* print raw values from a Wacom pen tip to the terminal

To compile:
	gcc -o pen-tip-values pen-tip-values.c -lX11 -lXi

To run the sample
	./find-pressure

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

static void print_valuators(Display *display, XIAnyClassInfo **classes, int num_classes)
{
    int i;

    for (i = 0; i < num_classes; i++) {
        if (classes[i]->type == XIValuatorClass) {
                    XIValuatorClassInfo *v = (XIValuatorClassInfo*)classes[i];
                    printf("Valuator %d: '%s'\n", v->number, (v->label) ?  XGetAtomName(display, v->label) : "No label");
                    printf("\tRange: %f - %f\n", v->min, v->max);
                    printf("\tResolution: %d units/m\n", v->resolution);
                    printf("\tMode: %s\n", v->mode == XIModeAbsolute ? "absolute": "relative");
                    if (v->mode == XIModeAbsolute)
                        printf("\tCurrent value: %f\n", v->value);
        }
    }
}

static void device_info(Display *display, int deviceid)
{
	XIDeviceInfo *info, *dev;
	int i, ndevices;

	dev = XIQueryDevice(display, deviceid, &ndevices);

	printf("Device Name: '%s' (%d)\n", dev->name, dev->deviceid);
	print_valuators(display, dev->classes, dev->num_classes);
}

static int find_stylus(Display *display, int deviceid)
{
        XIDeviceInfo *info, *dev;
        int ndevices;
        int i;
        char * word;
        int stylus = 0;

        info = XIQueryDevice(display, deviceid, &ndevices);

        for(i = 0; i < ndevices; i++) {
                dev = &info[i];

                word = strtok (dev->name," ");
                while (1) {
                        word = strtok (NULL, " ");
                        if (!word)
				break;
				if (strcmp("stylus", word) == 0)
				stylus = dev->deviceid;
                }
        }

        if (stylus) {
                printf("Selected Device with ID %i\n", stylus);
		device_info(display, stylus);
	} else {
                printf("no tablet connected\n");
	}

	return stylus;
}


static Window create_win(Display *dpy)
{
	XIEventMask mask;
	int stylus;

	Window win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 1200,
		800, 0, 0, WhitePixel(dpy, 0));

	stylus	= find_stylus(dpy, XIAllDevices);
	if (stylus)
    		mask.deviceid = stylus;
	else
		exit(0);

	mask.mask_len = XIMaskLen(XI_RawMotion);
    	mask.mask = calloc(mask.mask_len, sizeof(char));
    	memset(mask.mask, 0, mask.mask_len);
	XISetMask(mask.mask, XI_RawMotion);

	XISelectEvents(dpy, DefaultRootWindow(dpy), &mask, 1);

	free(mask.mask);
	XMapWindow(dpy, win);
	XSync(dpy, True);
	return win;
}


static void print_rawmotion(XIRawEvent *event)
{
    int i;
    double *valuator = event->valuators.values;

    printf("    device: %d event type: %d\n", event->deviceid, event->evtype);

    for (i = 0; i < event->valuators.mask_len * 8; i++) {
	if (XIMaskIsSet(event->valuators.mask, i)) {
            printf(" raw valuator %d: %f\n", i, *valuator);
            valuator++;
        }
    }
}

int main (int argc, char **argv)
{
    Display *dpy;
    int xi_opcode, event, error;
    Window win;
    XEvent ev;

    dpy = XOpenDisplay(NULL);

	/* Do error checks here*/
    if (!dpy) {
        fprintf(stderr, "Failed to open display.\n");
        return -1;
    }

    if (!XQueryExtension(dpy, "XInputExtension", &xi_opcode, &event, &error)) {
           printf("X Input extension not available.\n");
              return -1;
    }

    win = create_win(dpy);

    while(1) {
        XGenericEventCookie *cookie = &ev.xcookie;

        XNextEvent(dpy, &ev);
        if (cookie->type != GenericEvent ||
            cookie->extension != xi_opcode ||
            !XGetEventData(dpy, cookie))
            continue;

        if (cookie->evtype == XI_RawMotion)
                print_rawmotion(cookie->data);

        XFreeEventData(dpy, cookie);
    }

    XCloseDisplay(dpy);
    return 0;
}

