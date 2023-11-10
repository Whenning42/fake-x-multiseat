import time

from cffi import FFI

mpxi_ffi = FFI()
mpxi_ffi.cdef(
    """
    typedef ... Display;
    typedef long Window;

    Display* open_display(char* display_name);
    void make_cursor(Display* display, Window client_connection_window, char* cursor_name);
    void delete_cursor(Display* display, char* cursor);

    void key_event(Display* display, unsigned int keycode, bool is_press);
    void move_mouse(Display* display, int x, int y);
    void button_event(Display* display, unsigned int button, bool is_press);
"""
)

mpxi = mpxi_ffi.dlopen("lib_mpx_input.so")

display = mpxi.open_display(mpxi_ffi.NULL)
c_name = "test_ptr".encode()
mpxi.delete_cursor(display, c_name)
mpxi.make_cursor(display, 0, c_name)
print("Make cursor")
for i in range(10):
    mpxi.move_mouse(display, 100 * i, 100 * i)
    time.sleep(1)
    print("Moving")
mpxi.delete_cursor(display, c_name)
