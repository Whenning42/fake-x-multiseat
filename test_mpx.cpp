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

void send_input_focus(Display* display, Window window) {
  XEvent event;
  event.type = FocusIn;
  event.xfocus.display = display;
  event.xfocus.window = window;
  event.xfocus.mode = NotifyNormal;
  event.xfocus.detail = NotifyNonlinear;
  XSendEvent(display, window, True, FocusChangeMask, &event);
}

void run(std::vector<int> cursors, const std::vector<int>& windows) {
  Display* display = XOpenDisplay(NULL);
  const int W = 640;
  const int H = 320;
  Window root = XDefaultRootWindow(display);
  for (int i = 0; i < windows.size(); ++i) {
    XISetClientPointer(display, None, cursors[i]);
    XISetClientPointer(display, windows[i], cursors[i]);
    XSync(display, 0);
    XSetInputFocus(display, windows[i], RevertToParent, CurrentTime);
    send_input_focus(display, windows[i]);
    XTestFakeButtonEvent(display, 1, True, CurrentTime);
    XTestFakeButtonEvent(display, 1, False, CurrentTime);
    XSync(display, 0);
  }

  int n = 0;
  while (true) {
    n += 1;
    for (int i = 0; i < cursors.size(); ++i) {
      float theta = 2 * M_PI * n / 200.0;
      if (i % 2 == 0) {
        theta *= -1;
      }

      float x = W / 2 + W / 4 * cos(theta);
      float y = 880 + H / 2 + H / 4 * sin(theta);
      x += 640 * i;
      int dir = theta / (2 * M_PI) * 8;
      dir = dir % 8;
      if (dir < 0) {
        dir += 8;
      }

      XISetClientPointer(display, None, cursors[i]);
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
        XTestFakeKeyEvent(display, 38, True, CurrentTime);
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
        XTestFakeKeyEvent(display, 38, False, CurrentTime);
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

  std::vector<int> windows;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.substr(0, 2) == "0x") {
      std::stringstream ss;
      ss << std::hex << arg;
      int window;
      ss >> window;
      windows.push_back(window);
    }
  }
  run(get_cursors(), windows);
  return 0;
}
