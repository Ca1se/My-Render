#include <cstdio>
#include <cstdint>
#include <ctime>
#include <memory>
#include <vector>
#include <string>
#include "camera.hpp"
#include "material.hpp"
#include "matrix.hpp"
#include "model.hpp"
#include "phong_shader.hpp"
#include "pipeline.hpp"
#include "util.hpp"
#include "macro.hpp"
#include "window.hpp"

int main(int argc, char** argv) {
    static const char* const kHelpMessage = 
        "A simple software rasterizer.\n"
        "\n"
        "Usage:\n"
        "\trenderer --help\n"
        "\trenderer [--window-size <width>x<height>] --model <model-path>... --texture (<texture-path> | null)...\n"
        "\n"
        "Options:\n"
        "\t--help                                     Show this help message\n"
        "\t--model <model-path>...                    Set model paths, only support obj format model file\n"
        "\t--texture (<texture-path> | null)...       Set texture paths, model and texture should correspond to\n"
        "\t                                           each other, set the texture path to 'null' means no texture\n"
        "\t                                           assigned to the model, only support png format texture file\n"
        "\t--window-size <width>x<height>             Set the window width and height [default: 800x600]\n";

    if(argc < 2) {
        printf(kHelpMessage);
        return 0;
    }

    int window_width = 800;
    int window_height = 600;
    bool model = false;
    bool texture = false;
    ObjLoader loader;
    std::vector<std::shared_ptr<Model>> models;
    std::vector<std::shared_ptr<Texture>> textures;

    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "--help")) {
            printf(kHelpMessage);
            return 0;
        }else if(!strcmp(argv[i], "--model")){
            if(i + 1 >= argc) {
                printMissArg(argv[0]);
                return 0;
            }
            model = true;
            texture = false;
        }else if(!strcmp(argv[i], "--texture")) {
            if(i + 1 >= argc) {
                printMissArg(argv[0]);
                return 0;
            }
            model = false;
            texture = true;
        }else if(!strcmp(argv[i], "--window-size")) {
            if(i + 1 >= argc) {
                printMissArg(argv[0]);
                return 0;
            }
            model = texture = false;
            i++;
            if(char* p = strchr(argv[i], 'x'); p != nullptr) {
                std::string arg(argv[i]);
                try {
                    window_width = std::stoi(arg.substr(0, p - argv[i]));
                    window_height = std::stoi(arg.substr(p - argv[i] + 1));
                }catch(...) {
                    printWrongArg(argv[0], argv[i]);
                    return 0;
                }
            }else {
                printWrongArg(argv[0], argv[i]);
                return 0;
            }
        }else if(model) {
            auto m = loader.loadModel(argv[i]);
            models.push_back(m);
        }else if(texture) {
            if(!strcmp(argv[i], "null")) {
                textures.push_back(std::shared_ptr<Texture>{});
            }else {
                auto t = loader.loadTexture(argv[i]);
                textures.push_back(t);
            }
        }
    }

    if(models.size() != textures.size()) {
        printf("%s: the number of models and textures should be equal\n", argv[0]);
        return 0;
    }

    Camera camera{ Vector3f{0.145243, 1.88633, -0.901848}, Vector3f{-15.9753, -1.11993, -6.45258}, Vector3f{0, 1, 0} };

    Pipeline pipeline(window_width, window_height);

    Shader shader;
    setPhongInfo(shader);

    Matrix4f perspective_matrix = calProjectionMatrix(60, float(window_width) / window_height, -0.1, -10000);

    Window window(window_width, window_height);
    window.display();

    std::uint32_t frame_count = 0;
    time_t now, begin = time(nullptr);
    while(!window.isClosed()) {
        pipeline.clearBuffer();

        window.handleEvent(camera);
        updateShader(shader, camera, perspective_matrix);



        for(int i = 0; i < models.size(); i++) {
            if(models[i]) {
                shader.texture = textures[i];
                pipeline.renderingModel(*models[i], shader);
            }
        }
        window.draw(pipeline.data(), 0, 0, window_width, window_height);
        
        if(now = time(nullptr); now - begin >= 1) {
            printf("fps: %u\n", frame_count);
            frame_count = 0;
            begin = now;
        }
        frame_count++;
    }

    return 0;
}
