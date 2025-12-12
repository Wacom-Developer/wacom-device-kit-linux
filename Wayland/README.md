# Readme

## Finding Wacom Devices

The example code for this section is in the subfolder "SampleCode"

Compile and run it as such:

```
$ gcc client.c zwp-tablet-v2-client-protocol.c xdg-shell-protocol.c -o client     $(pkg-config --cflags --libs wayland-client)
$ ./client
```
The protocol files, which are necessary to compile the code, are included. If you should need to generate the protocol files yourself, see the documentation for "wayland-scanner" (eg https://wayland-book.com/libwayland/wayland-scanner.html).

The code for this example is found in client.c. At the beginning of that code are the Wayland buffer and surface code which are largely adapted from the url referred to at the beginning of that file.

The central tablet features of this code are the ```struct zwp_tablet_tool_v2_listener``` tablet_tool_listener and other similar listeners. Note that there are separate listeners for pad events (pad ring, pad strip, etc). These listeners are defined in the generated protocol files. Be careful, different versions of the protocol have different versions of these structs. To receive tablet events, create all the functions required by these listeners and assign them in their respective structs.

The application is prepared to receive input in ```setup_tablet_input()``` which is called from ```main``` with the Wayland ```client_state```.
