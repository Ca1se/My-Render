#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>

class Window {
private:
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


};

#endif