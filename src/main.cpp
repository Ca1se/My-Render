#include <cstring>
#include <cstdint>
#include <iostream>
#include <vector>
#include "camera.hpp"
#include "matrix.hpp"
#include "pipeline.hpp"
#include "util.hpp"

static const int kWindowWidth = 800;
static const int kWindowHeight = 600;

int main(int argc, char** argv) {
    ObjLoader loader;
    loader.loadModel("model/cottage/cottage.obj");

    Pipeline pipeline;
    pipeline.setRenderingSize(kWindowWidth, kWindowHeight);
    pipeline.clearBuffer();

    Camera camera{Vector3f{25, 25, 25}, Vector3f{-1, -1, -1}, Vector3f{0, 1, -1}};

    // Matrix4f model_matrix = Matrix4f::Identity();
    Matrix4f view_matrix = calViewMatrix(camera);
    Matrix4f perspective_matrix = calPerspectiveMatrix(60, float(kWindowWidth) / kWindowHeight, -0.1, -10000);

    Shader shader;
    shader.viewer_pos = camera.position;
    shader.mvp = perspective_matrix * view_matrix;
    shader.texture = loader.loaded_materials()[1].diffuse_texture;

    setPhongInfo(shader, camera);

    pipeline.draw(loader.loaded_models()[0], shader);

    PNGImage image;
    auto& header = image.header();
    header = shader.texture->image_->header();

    header.color_type = TRUE_COLOR_ALPHA;
    header.height = kWindowHeight;
    header.width = kWindowWidth;

    image.data_.reset(new std::uint8_t[kWindowWidth * kWindowHeight * 4]);
    memcpy(image.data(), pipeline.data(), kWindowHeight * kWindowWidth * 4);
    image.generatePNG("output.png");

    return 0;
}