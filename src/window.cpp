#include <cstdint>
#include <cstdlib>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xproto.h>
#include "window.hpp"


Window::Window(int width, int height):
        closed_(true), width_(width), height_(height), connection_(xcb_connect(nullptr, nullptr)), 
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
    xcb_create_pixmap(connection_,
                      screen->root_depth, 
                      canvas_, 
                      window_, 
                      width, 
                      height);
    xcb_create_gc(connection_, gcontext_, canvas_, 0, nullptr);

    // will be freed by function xcb_image_destory
    std::uint8_t* data = (std::uint8_t*) malloc(4 * width * height);
    image_ = xcb_image_create_native(connection_, 
                                     width, height, 
                                     XCB_IMAGE_FORMAT_Z_PIXMAP, 
                                     screen->root_depth,
                                     data,
                                     width * height * 4,
                                     data);
}

Window::~Window() {
    xcb_image_destroy(image_);
    xcb_free_pixmap(connection_, canvas_);
    xcb_disconnect(connection_);
}

void Window::draw(const std::uint8_t* data, int x, int y, int width, int height) const noexcept {
        if(x >= width_ || y >= height_)
            return;

        width -= std::max(0, x + width - width_);
        height -= std::max(0, y + height - height_);

        int cur = 4 * (width_ * y + x);
        for(std::uint32_t i = 0, dp; i < 4 * width * height; i += 4, cur += 4) {
            dp = *(std::uint32_t*) (data + cur);
            *(std::uint32_t*) (image_->data + cur) = (dp & 0xff00ff00 | ((dp >> 16) & 0xFF) | ((dp << 16) & 0xFF0000));
        }
        xcb_image_put(connection_, canvas_, gcontext_, image_, 0, 0, 0);
        xcb_copy_area(connection_, canvas_, window_, gcontext_, x, y, x, y, width, height);
        xcb_flush(connection_);
    }