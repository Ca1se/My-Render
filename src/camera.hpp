#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include "matrix.hpp"

struct Camera {
    Vector3f target;
    Vector3f view;
    Vector3f up;
};

#endif