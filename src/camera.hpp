#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include "matrix.hpp"

struct Camera {
    Vector3f position;
    Vector3f gaze;
    Vector3f view_up;
};

#endif