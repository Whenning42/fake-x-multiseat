// Test how multiple Noita windows handle MPX.

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <regex>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <csignal>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <X11/extensions/XTest.h>


std::vector<int> get_cursors() {
  Display* display = XOpenDisplay(NULL);
  std::regex r("test_ptr\\d\\d\\d pointer");

  int num_devices;
  std::vector<int> cursors;
  XIDeviceInfo* devices = XIQueryDevice(display, XIAllDevices, &num_devices);
  for (int i = 0; i < num_devices; ++i) {
    XIDeviceInfo& dev = devices[i];
    if (std::regex_match(dev.name, r)) {
      cursors.push_back(dev.deviceid);
    }
  }
  return cursors;
}

void clear_cursors() {
  Display* display = XOpenDisplay(NULL);

  std::vector<int> cursors = get_cursors();
  for (int cursor : cursors) {
    XIRemoveMasterInfo remove;
    remove.type = XIRemoveMaster;
    remove.deviceid = cursor;
    remove.return_mode = XIAttachToMaster;
    remove.return_pointer = 2;
    remove.return_keyboard = 3;
    XIChangeHierarchy(display, (XIAnyHierarchyChangeInfo*)&remove, 1);
  }
  XSync(display, 0);
}

std::string cursor_name(int i) {
  std::ostringstream oss;
  oss << "test_ptr" << std::setfill('0') << std::setw(3) << i;
  return oss.str();
}

void make_cursors(int n) {
  Display* display = XOpenDisplay(NULL);
  for (int i = 0; i < n; ++i ) {
    XIAddMasterInfo add;
    std::string name = cursor_name(i);
    add.type = XIAddMaster;
    add.name = &name[0];
    add.send_core = 1;
    add.enable = 1;
    XIChangeHierarchy(display, (XIAnyHierarchyChangeInfo*)&add, 1);
  }
  XSync(display, 0);
}

void exit_handler(int sig) {
  clear_cursors();
  exit(0);
}

long random(long min, long max) {
  return min + (rand() % (max - min));
}

void run(std::vector<int> cursors) {
  Display* display = XOpenDisplay(NULL);
  std::vector<int> windows = {0x4200003, 0x3a00003};
  const int W = 640;
  const int H = 320;
  Window root = XDefaultRootWindow(display);
  for (int i = 0; i < windows.size(); ++i) {
    XISetClientPointer(display, None, cursors[i]);
    XISetClientPointer(display, windows[i], cursors[i]);
    XSync(display, 0);
    // XTestFakeButtonEvent(display, 1, True, CurrentTime);
    // XTestFakeButtonEvent(display, 1, False, CurrentTime);
    XSetInputFocus(display, windows[i], RevertToParent, CurrentTime);
    XSync(display, 0);

    // XISetClientPointer(display, None, cursors[i]);
    // XISetClientPointer(display, windows[i], cursors[i]);
    // XSetInputFocus(display, windows[i], RevertToParent, CurrentTime);
    // XFocusInEvent focus_in;
    // focus_in.type = FocusIn;
    // focus_in.display = display;
    // focus_in.window = windows[i];
    // focus_in.mode = NotifyNormal;
    // focus_in.detail = NotifyNonlinear;
    // XSendEvent(display, windows[i], False, 0, (XEvent*)&focus_in);
    // XTestFakeButtonEvent(display, 1, True, CurrentTime);
    // XFlush(display);
    // usleep(50000);

    // printf("Trying to open device %d\n", cursors[i]);
    // devices.push_back(XOpenDevice(display, dids[i]));
  }
  return;

  int n = 0;
  while (true) {
    n += 1;
    for (int i = 0; i < cursors.size(); ++i) {
      // XISetClientPointer(display, None, cursors[i]);
      // XISetClientPointer(display, root, cursors[i]);
      // XISetClientPointer(display, windows[i], cursors[i]);
      // XTestFakeButtonEvent(display, 1, True, CurrentTime);
      // XTestFakeButtonEvent(display, 1, False, CurrentTime);
      // XFlush(display);
      // XSetInputFocus(display, windows[i], RevertToParent, CurrentTime);


      float theta = 2 * M_PI * n / 200.0;
      if (i % 2 == 0) {
        theta *= -1;
      }

      float x = W / 2 + W / 4 * cos(theta);
      float y = H / 2 + H / 4 * sin(theta);
      x += 640 * i;
      int dir = theta / (2 * M_PI) * 8;
      dir = dir % 8;
      if (dir < 0) {
        dir += 8;
      }

      // XEnterWindowEvent enter;
      // enter.type = EnterNotify;
      // enter.display = display;
      // enter.window = windows[i];
      // enter.root = root;
      // enter.subwindow = None;
      // enter.time = CurrentTime;
      // enter.x = x;
      // enter.y = y;
      // enter.x_root = x;
      // enter.y_root = y;
      // enter.mode = NotifyGrab;
      // enter.detail = NotifyInferior;
      // enter.same_screen = True;
      // enter.focus = True;
      // enter.state = 0;
      // XSendEvent(display, windows[i], False, 0, (XEvent*)&enter);

      XIWarpPointer(display, cursors[i], None, root, 0, 0, 0, 0, x, y);
      XDevice dev;
      dev.device_id = 0;
      dev.classes = NULL;
      dev.num_classes = 0;
      XSync(display, 0);
      if (dir < 4) {
        XKeyEvent event;
        event.type = KeyPress;
        event.display = display;
        event.root = root;
        event.window = windows[i];
        event.subwindow = None;
        event.time = CurrentTime;
        event.x = x;
        event.y = y;
        event.x_root = x;
        event.y_root = y;
        event.state = 0;
        event.keycode = 38;
        event.same_screen = True;
        // XSetInputFocus(display, windows[i], RevertToParent, CurrentTime);
        // XSendEvent(display, windows[i], True, 0, (XEvent*)&event);
        // XTestFakeMotionEvent(display, 0, x, y, CurrentTime);
        XTestFakeKeyEvent(display, 38, True, CurrentTime);
        // XTestFakeDeviceKeyEvent(display, &dev, 38, True, 0, 0, CurrentTime);
        XFlush(display);
        printf("Window %d, press\n", i);
      } else {
        XKeyEvent event;
        event.type = KeyRelease;
        event.display = display;
        event.root = root;
        event.window = windows[i];
        event.subwindow = None;
        event.time = CurrentTime;
        event.x = x;
        event.y = y;
        event.x_root = x;
        event.y_root = y;
        event.state = 0;
        event.keycode = 38;
        event.same_screen = True;
        // XSetInputFocus(display, windows[i], RevertToParent, CurrentTime);
        // XSendEvent(display, windows[i], True, 0, (XEvent*)&event);
        // XTestFakeMotionEvent(display, 0, x, y, CurrentTime);
        XTestFakeKeyEvent(display, 38, False, CurrentTime);
        // XTestFakeDeviceKeyEvent(display, &dev, 38, False, 0, 0, CurrentTime);
        XFlush(display);
        printf("Window %d, release\n", i);
      }
      XFlush(display);
    }
    usleep(16000);
  }
}

int main(int argc, char** argv) {
  struct sigaction sig_int_handler;
  sig_int_handler.sa_handler = exit_handler;
  sigemptyset(&sig_int_handler.sa_mask);
  sig_int_handler.sa_flags = 0;
  sigaction(SIGINT, &sig_int_handler, NULL);

  clear_cursors();
  make_cursors(2);
  run(get_cursors());
  return 0;
}
