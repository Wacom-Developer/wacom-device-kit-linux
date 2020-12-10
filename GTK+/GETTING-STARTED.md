sc-rm-supp-com-overview# Getting Started 

## Test Environment
This sample code is a C application which should be compatible with any of of the platforms GTK+3 is available for. The build script assumes a Linux system which has the GTK+3 development headers, libraries, and all dependencies (e.g. glib) installed. In addition, a basic C development environment (GCC or Clang, make, and pkg-config) must be available.

To test the application, use a Wacom tablet supported by your installed driver.

## Install the Wacom tablet driver and verify tablet operation
In order to run a sample application, it may be necessary to install a Wacom tablet driver. Users of Microsoft Windows and Apple macOS should obtain the driver from the Wacom driver support web page at: https://www.wacom.com/support/product-support/drivers. Users of Linux-based desktops should have a driver pre-installed; if your tablet does not work out of the box (see below) you may need to install or update a driver component as outlined at [Building the Driver Overview](https://developer-docs.wacom.com/wacom-device-api/docs/bldg-driver-overview).

Once the driver has installed, and you have rebooted your system, check your tablet driver installation by doing the following:

1. Attach one of the supported Wacom tablets as listed above.
1. Windows and macOS users should open the Wacom Tablet Properties app or the Wacom Desktop Center to determine if your tablet is recognized by the driver.
1. Use a tablet pen to see if you can move the system cursor.
1. If all of the above checks out, proceed to the next step to build/run the sample application

## Build/run the sample application
To build the sample application:

1. Open a command-line terminal.
1. Navigate to the sample code directory.
1. Run make to start the build.
1. Run ./demo to launch the application.

## See Also
[GTK+ Overview](https://developer-docs.wacom.com/wacom-device-api/docs/gtk-overview) - Information about use of the GTK+ tablet APIs.

## Where to get help
If you have questions about the sample application or any of the setup process, please visit our Support page at: https://developer.wacom.com/developer-dashboard/support