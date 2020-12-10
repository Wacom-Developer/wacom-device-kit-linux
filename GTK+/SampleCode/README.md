# GTK+3 Tablet Demos

This project builds tablet-related demos of GTK+3 functions. Demo code
has been copied from the `gtk3-demo` program, version 3.24. The source
code for the entire demo may be found at the following URL:

https://gitlab.gnome.org/GNOME/gtk/-/tree/master/demos/gtk-demo


## Build

This program requires GTK+3 development headers, libraries, and all
associated dependencies (e.g. pango, glib2, etc.).

To compile, simply run `make` after installing the above dependencies.

## Run

To run, simply run `./demo` after compiling. A window with three buttons
will appear. Click any of the buttons to launch a specific demo.


## Study

The code for each demo is included. Feel free to study and modify the
demo code as desired.

  * `main.c`: Program entrypoint. Initializes GTK+ and creates the
    window with buttons.

  * `drawingarea.c`: Demonstration of how to create a drawing area
    and respond to input. This demo is not pressure-sensitive.

  * `paint.c`: Demonstration of how to provide a pressure-sensitive
    drawing area which lets you paint in different colors.

  * `event_axes.c`: Demostration of how to receive additional device
    state information, e.g. tilt, rotation, etc.


## License

Please see the COPYING file for license information.
