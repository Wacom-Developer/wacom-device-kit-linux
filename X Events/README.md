# Readme

## Finding Wacom Devices

The example code for this section is ```xinput-list-wacom-devices.c```.

Compile and run it as such:

```
$ gcc -o xinput-list-wacom-devices xinput-list-wacom-devices.c -lX11 -lXi
$ ./xinput-list-wacom-devices
```

To work with X events from a Wacom tablet you'll need to first see which tablet devices are available. To do this we'll just use ```XIQueryDevice()``` with the ```XIAllDevices const``` value.
```
        devices = XIQueryDevice(display, XIAllDevices, &ndevices);

        for (i = 0; i < ndevices; i++) {
                device = devices[i];

                if (strncmp("Wacom", device.name, 5) != 0)
                        continue;

                printf("id: %d - %s \n", device.deviceid, device.name);
        }

```
Then, for our purposes here, we'll just check to see if the ```device.name``` begins with Wacom.

```
$ gcc -o xinput-list-wacom-devices xinput-list-wacom-devices.c -lX11 -lXi
$ ./xinput-list-wacom-devices
id: 16 - Wacom Intuos Pro M Pen stylus
id: 17 - Wacom Intuos Pro M Pen eraser
id: 18 - Wacom Intuos Pro M Pen cursor
id: 19 - Wacom Intuos Pro M Pad pad
id: 20 - Wacom Intuos Pro M Finger touch
```

As you can see ```XIQueryDevice``` lets us know what Wacom devices we have available. In this case the Intuos Pro has a stylus, eraser, cursor, pad and touch.

## Getting X Events


In our next demo application we will focus on getting events from the Pen stylus device (id 16 in earlier parts of this tutorial). This approach can easily be extended to the other device types (e.g. Touch, Pad).  

To work with the X Server we'll need to first get the display and then create an X window with XCreateSimpleWindow.
```
    dpy = XOpenDisplay(NULL);
    Window win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 1200,
                                     800, 0, 0, WhitePixel(dpy, 0));
```

Next, to get events we'll want to select a device to get the events from. To find a Wacom device we'll iterate over the X server's list of devices until we find a device that ends with the substring "stylus", a quick and dirty way to get on to the next step. Of course there are other (better) ways to find the stylus and this method will only find the last stylus listed. You'll want to do more sophisticated device selection in your application.

To get the list of devices we'll call XIQuerryDeivce with the display, the constant XIAllDevices, and an int which will serve as a return value for the number of devices.

Our event mask requests XI_RawMotion events so we'll also set that part of the mask here as well:
```
        stylus  = find_stylus(dpy, XIAllDevices);
        if (stylus)
                mask.deviceid = stylus;
...
        XISetMask(mask.mask, XI_RawMotion);

        XISelectEvents(dpy, DefaultRootWindow(dpy), &mask, 1);
```

After that we send the mask to ```XISelectEvents()``` and register for events on the ```DefaultRootWindow```.

Now that we've requested the events we just need to process them once they are sent to us. We'll read the events in a loop.
```
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
```
>Note: These events make use of the [Xlib cookie](http://who-t.blogspot.com/2009/07/xlib-cookie-events.html).

To compile this program and run it use the following command:


```gcc -o find-pressure find-pressure.c -lX11 -lXi && ./find-pressure```

Here is a single example event output by this program:
```
 device: 16 event type: 17
 raw valuator 0: 21519.000000
 raw valuator 1: 11600.000000
 raw valuator 2: 65536.000000
 raw valuator 3: 16.000000
 raw valuator 4: 12.000000
 raw valuator 5: -900.000000
```


## Device info
This section continues to use the example code from the previous section: <code>find-pressure.c</code>.

To know what these valuators represent we'll need to take a step back and look at how we get the information about a device.

Once we've selected a device to monitor we'll check the labels for its valuators. To do this we'll check the label of each XIValuatorClass in a loop.  For most common devices you should see Absolute X, Absolute Y, Absolute Pressure, Absolute Tilt X, Absolute Tilt Y, and Absolute Wheel (the Wheel valuator is only used by certain styli).	

Finally we'll read the values of the valuators as the device is moved around on the tablet. To read a tablet event, we'll need to get the XEvent cookie.

```XGenericEventCookie *cookie = &ev.xcookie;```



Once we have the cookie we'll parse and display the valuators that are in the cookie's ```XIRawEvent``` data:
```
    for (i = 0; i < event->valuators.mask_len * 8; i++) {

        if (XIMaskIsSet(event->valuators.mask, i)) {

            printf(" raw valuator %d: %f\n", i, *valuator);

            valuator++;

        }

    }
```

## Touch and Pad
<a name="multi-touch-sample"></a>
### Multi-Touch
The multitouch.c file is a graphic X Input 2 based program. It shows how ownership work multi-touch events. It also displays the touch events from different fingers.

### Pad Events
For the pad, if you wanted to directly read button presses, you'd have to have the compositor ungrab the pad device. The feasibility of this is low for applications distributed to users. It would be more realistic when the OS is controlled by the developer. GNOME, for example, maps the ExpressKeys to keys and key combinations.

The ExpressKey ring's valuators will be available with the "Abs Wheel" valuator. The Wheel has a resolution of 1 and a range of 0-71. Pad strips are available on a few devices in the same manner.

