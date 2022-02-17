#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <cstdint>
#include "window.hpp"

Window::Window(int width, int height):
        width_(width), height_(height), connection_(xcb_connect(nullptr, nullptr)),
        window_(xcb_generate_id(connection_)), gcontext_(xcb_generate_id(connection_)),
        canvas_(xcb_generate_id(connection_)), image_(nullptr) {
    xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(connection_)).data;
    std::uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    std::uint32_t value_mask = XCB_EVENT_MASK_EXPOSURE          | XCB_EVENT_MASK_BUTTON_PRESS   |
                               XCB_EVENT_MASK_BUTTON_RELEASE    | XCB_EVENT_MASK_POINTER_MOTION |
                               XCB_EVENT_MASK_KEY_PRESS;
    std::uint32_t values[] = { screen->black_pixel, value_mask };
    xcb_create_window(connection_,
                      screen->root_depth,
                      window_,
                      screen->root,
                      0, 0,
                      width, height,
                      1,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual,
                      mask,
                      values);
}