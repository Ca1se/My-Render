#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include <algorithm>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <cstdint>
#include <xcb/xproto.h>

class Window {
private:
    bool closed_;
    int width_;
    int height_;

    xcb_connection_t* connection_;
    xcb_window_t window_;
    xcb_gcontext_t gcontext_;
    xcb_pixmap_t canvas_;
    xcb_image_t* image_;


public:
    Window(int width = 800, int height = 600);
    ~Window();

public:
    void display() noexcept {
        xcb_map_window(connection_, window_);
        xcb_flush(connection_);
        xcb_generic_event_t *ev;
        while((ev = xcb_wait_for_event(connection_))) {
            if((ev->response_type & ~0x80) == XCB_EXPOSE)
                break;
        }
        closed_ = false;
    }

    void draw(const std::uint8_t* data, int x, int y, int width, int height) const noexcept;

    bool isClosed() const noexcept {
        return closed_;
    }
};

#endif