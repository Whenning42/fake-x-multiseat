// Sets the X11 override redirect attribute on a window

#include <X11/Xlib.h>

const int window = 0x3400003;

int main(int argc, char** argv) {
  Display* display = XOpenDisplay(NULL);
  XSetWindowAttributes attributes;
  attributes.override_redirect = True;
  XChangeWindowAttributes(display, window, CWOverrideRedirect, &attributes);
  XFlush(display);
  XCloseDisplay(display);
  return 0;
}
