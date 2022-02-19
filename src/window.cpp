#include <cstddef>
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
    std::uint32_t value_mask = XCB_EVENT_MASK_EXPOSURE         | XCB_EVENT_MASK_BUTTON_PRESS   |
                               XCB_EVENT_MASK_BUTTON_RELEASE   | XCB_EVENT_MASK_BUTTON_MOTION  |
                               XCB_EVENT_MASK_POINTER_MOTION   | XCB_EVENT_MASK_KEY_PRESS;
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
    while(!waited_events_.empty()) {
        free(waited_events_.front());
        waited_events_.pop();
    }
}

void Window::display() noexcept {
    xcb_map_window(connection_, window_);
    xcb_flush(connection_);
    xcb_generic_event_t *ev;
    while((ev = xcb_wait_for_event(connection_))) {
        if((ev->response_type & ~0x80) == XCB_EXPOSE) {
            free(ev);
            break;
        }
    }
    closed_ = false;
    
    std::thread([this]() {
        xcb_generic_event_t* event;
        if((event = xcb_wait_for_event(connection_)) != nullptr) free(event);
        if((event = xcb_wait_for_event(connection_)) != nullptr) free(event);
        while((event = xcb_wait_for_event(connection_)) != nullptr) {
            if((event->response_type & ~0x80) != XCB_NO_EXPOSURE) {
                waited_events_.push(event);
            }else {
                free(event);
            }
        }
        closed_ = true;
    }).detach();
}

void Window::handleEvent(Camera& camera) noexcept {
    Vector3f z = Vector3f{camera.view - camera.target}.normalized();
    Vector3f x = camera.up.cross(z).normalized();
    Vector3f y = camera.up.normalized();

    xcb_generic_event_t* event;
    if((event = waitEventNonBlock()) != nullptr) {
        switch (event->response_type & ~0x80) {
            // w: 25, s: 39, a: 38, d: 40, q: 24, e: 26, shift: 50
            case XCB_KEY_PRESS: {
                xcb_key_press_event_t* ev = (xcb_key_press_event_t*) event;
                if(ev->detail == 25) {
                    Vector3f tz = 0.05f * z;
                    camera.view -= tz;
                    camera.target -= tz;
                }else if(ev->detail == 39) {
                    Vector3f tz = 0.05f * z;
                    camera.view += tz;
                    camera.target += tz;
                }else if(ev->detail == 38) {
                    Vector3f tx = 0.05f * x;
                    camera.view -= tx;
                    camera.target -= tx;
                }else if(ev->detail == 40) {
                    Vector3f tx = 0.05f * x;
                    camera.view += tx;
                    camera.target += tx;
                }else if(ev->detail == 24) {
                    Vector3f ty = 0.05f * y;
                    camera.view += ty;
                    camera.target += ty;
                }else if(ev->detail == 26) {
                    Vector3f ty = 0.05f * y;
                    camera.view -= ty;
                    camera.target -= ty;
                }
                break;
            }
        }
        free(event);
    }
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