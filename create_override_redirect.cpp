// Create an X11 window with the override redirect attribute set to True.
#include <X11/Xlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
  Display* display = XOpenDisplay(NULL);
  XSetWindowAttributes attributes;
  attributes.override_redirect = True;
  attributes.background_pixel = XBlackPixel(display, 0);
  Window window = XCreateWindow(display, DefaultRootWindow(display), 0, 0, 200, 200, 0, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWOverrideRedirect, &attributes);
  printf("Window ID: %x\n", window);
  XMapWindow(display, window);
  XFlush(display);
  sleep(100);
  XCloseDisplay(display);
  return 0;
}