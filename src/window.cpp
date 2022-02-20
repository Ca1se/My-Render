#include <cstddef>
#include <cstdint>
#include <iostream>
#include <cstdlib>
#include <type_traits>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xproto.h>
#include "util.hpp"
#include "macro.hpp"
#include "window.hpp"


Window::Window(int width, int height):
        closed_(true), width_(width), height_(height), connection_(xcb_connect(nullptr, nullptr)), 
        window_(xcb_generate_id(connection_)), gcontext_(xcb_generate_id(connection_)), 
        canvas_(xcb_generate_id(connection_)), image_(nullptr), mouse_pressed_(false) {
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
        int pre_event = -1;
        if((event = xcb_wait_for_event(connection_)) != nullptr) free(event);
        if((event = xcb_wait_for_event(connection_)) != nullptr) free(event);
        while((event = xcb_wait_for_event(connection_)) != nullptr) {
            if(int e = (event->response_type & ~0x80); e != XCB_NO_EXPOSURE && e != XCB_KEY_RELEASE) {
                if(e == pre_event) {
                    free(event);
                    pre_event = -1;
                }else {
                    pre_event = e;
                    waited_events_.push(event);
                }
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
    Vector3f y = z.cross(x).normalized();

    float distance = Vector3f{camera.target - camera.view}.norm();

    xcb_generic_event_t* event;
    if((event = waitEventNonBlock()) != nullptr) {
        switch (event->response_type & ~0x80) {
            // w: 25, s: 39, a: 38, d: 40, q: 24, e: 26, shift: 50
            case XCB_KEY_PRESS: {
                xcb_key_press_event_t* ev = (xcb_key_press_event_t*) event;
                if(ev->detail == 25) {
                    Vector3f tz = 0.15f * z;
                    camera.view -= tz;
                    camera.target -= tz;
                }else if(ev->detail == 39) {
                    Vector3f tz = 0.15f * z;
                    camera.view += tz;
                    camera.target += tz;
                }else if(ev->detail == 38) {
                    Vector3f tx = 0.15f * x;
                    camera.view -= tx;
                    camera.target -= tx;
                }else if(ev->detail == 40) {
                    Vector3f tx = 0.15f * x;
                    camera.view += tx;
                    camera.target += tx;
                }else if(ev->detail == 24) {
                    Vector3f ty = 0.15f * y;
                    camera.view += ty;
                    camera.target += ty;
                }else if(ev->detail == 26) {
                    Vector3f ty = 0.15f * y;
                    camera.view -= ty;
                    camera.target -= ty;
                }
                break;
            }

            case XCB_BUTTON_PRESS: {
                xcb_button_press_event_t* ev = (xcb_button_press_event_t*) event;
                if(ev->detail == 1) {
                    mouse_pressed_ = true;
                    mouse_x_ = ev->event_x;
                    mouse_y_ = ev->event_y;
                }else if(ev->detail == 4) {
                    camera.view -= 0.01 * distance * z;
                }else if(ev->detail == 5) {
                    camera.view += 0.35f * z;
                }
                
                break;
            }

            case XCB_BUTTON_RELEASE: {
                xcb_button_release_event_t* ev = (xcb_button_release_event_t*) event;
                if(ev->detail == 1)
                    mouse_pressed_ = false;
                break;
            }

            case XCB_MOTION_NOTIFY: {
                if(mouse_pressed_) {
                    xcb_motion_notify_event_t* ev = (xcb_motion_notify_event_t*) event;
                    int x_offset = ev->event_x - mouse_x_;
                    int y_offset = ev->event_y - mouse_y_;
                    mouse_x_ = ev->event_x;
                    mouse_y_ = ev->event_y;
                    x_offset = -x_offset;
                    y_offset = -y_offset;

                    // reference from SunXLei: https://github.com/SunXLei/SRender/blob/master/core/camera.cpp
                    Vector3f from_target = camera.view - camera.target;
                    float radius = distance;

                    float phi     = (float) atan2(from_target.x(), from_target.z());
                    float theta   = (float) acos(from_target.y() / radius);
                    float x_delta = (float) x_offset / width_;
                    float y_delta = (float) y_offset / height_;

                    float factor = 1.5 * PI;

                    phi   += x_delta * factor;
                    theta += y_delta * factor;

                    if(theta > PI) theta = PI - MINIMAL_VAL * 100;
                    if(theta < 0)  theta = MINIMAL_VAL * 100;

                    camera.view.x() = camera.target.x() + radius * sin(phi) * sin(theta);
                    camera.view.y() = camera.target.y() + radius * cos(theta);
                    camera.view.z() = camera.target.z() + radius * sin(theta) * cos(phi);

                    /*
                    Matrix3f viewspace{
                        x.x(), y.x(), z.x(),
                        x.y(), y.y(), z.y(),
                        x.z(), y.z(), z.z()
                    };

                    Vector3f vs_view = Vector3f::Zero();
                    Vector3f vs_target = viewspace * (camera.target - camera.view);
                    
                    Vector2f zx = rotatePoint2D(Vector2f{ 0, 0 }, -0.15 * x_offset, Vector2f{ vs_target.z(), vs_target.x() });
                    vs_view.z() = zx.x();
                    vs_view.x() = zx.y();

                    camera.view = viewspace.transpose() * vs_view + camera.view;

                    Vector3f new_z = Vector3f{ camera.view - camera.target }.normalized();
                    Vector3f new_x = y.cross(new_z).normalized();
                    Vector3f new_y = new_z.cross(new_x).normalized();

                    Matrix3f new_viewspace{
                        new_x.x(), new_y.x(), new_z.x(),
                        new_x.y(), new_y.y(), new_z.y(),
                        new_x.z(), new_y.z(), new_z.z()
                    };

                    Vector3f new_vs_view = Vector3f::Zero();
                    Vector3f new_vs_target = new_viewspace * (camera.target - camera.view);

                    Vector2f zy = rotatePoint2D(Vector2f{ 0, 0 }, -0.15 * y_offset, Vector2f{ new_vs_target.z(), new_vs_target.y() });
                    new_vs_view.z() = zy.x();
                    new_vs_view.y() = zy.y();

                    camera.view = new_viewspace.transpose() * new_vs_view + camera.view;
                    */

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