A minimal demo of simultaneous independent mouse and keyboard input in X11. This 
is accomplished via Multi-Pointer X (MPX) and running the client apps outside of the
window manager, since no window managers support MPX.

Client apps are run outside of the window manager via a custom X11 proxy that sets the
override-redirect flag on XCreateWindow() and XChangeWindowAttributes() calls.

The demo assumes you have a Steam installation of Noita in the default directory. To
test against another app, update the `run_apps.sh` script.

To run the Noita demo:
```
$ make
$ ./run_apps.sh
```
