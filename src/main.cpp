#include <iostream>
#include <cstdint>
#include <vector>
#include "camera.hpp"
#include "matrix.hpp"
#include "model.hpp"
#include "util.hpp"

static const int kWindowWidth = 700;
static const int kWindowHeight = 700;

int main(int argc, char** argv) {
    if(argc < 2)
        return 0;

    ObjLoader loader;
    for(int i = 1; i < argc; i++) {
        loader.loadModel(argv[i]);
    }

    std::vector<std::uint8_t> framebuffer(kWindowWidth * kWindowHeight * 4);
    std::vector<float> zbuffer(kWindowWidth * kWindowHeight);

    Camera camera{Vector3f{10, 0, 0}, Vector3f{-1, 0, 0}, Vector3f{0, 0, 1}};

    // Matrix4f model_matrix = Matrix4f::Identity();
    Matrix4f view_matrix = calViewMatrix(camera);
    Matrix4f perspective_matrix = calPerspectiveMatrix(60, float(kWindowWidth) / kWindowHeight, -0.1, -10000);

    

    return 0;
}