#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <memory>
#include <vector>
#include <string>
#include "camera.hpp"
#include "matrix.hpp"
#include "model.hpp"
#include "phong_shader.hpp"
#include "pipeline.hpp"
#include "util.hpp"
#include "macro.hpp"
#include "window.hpp"

static const char* const kHelpMessage = "--help                                 display this help message\n"
                                        "--model MODEL1 [MODEL2] ...            load models, model names should be separated by commas, only            \n"
                                        "                                       support obj format model file                                           \n"
                                        "--texture TEXTURE1 [TEXTURE2] ...      load textures, model and texture should correspond to each other,       \n"
                                        "                                       when you don't want to specify a texture for the model, texture         \n"
                                        "                                       name should be \"null\", only support obj format model and png format   \n"
                                        "--window-size WIDTHxHEIGHT             set the window width and height, default window size is 800x600         \n";

int main(int argc, char** argv) {
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

    Camera camera{ Vector3f{-40, 20, 20}, Vector3f{1, -1, -1}, Vector3f{0, 1, -1} };

    Pipeline pipeline;
    pipeline.setRenderingSize(window_width, window_height);

    Shader shader;
    setPhongInfo(shader);

    Matrix4f perspective_matrix = calPerspectiveMatrix(60, float(window_width) / window_height, -0.1, -10000);

    Window window(window_width, window_height);
    window.display();

    std::uint32_t frame_count = 0;
    clock_t now, begin = clock();
    while(!window.isClosed()) {
        pipeline.clearBuffer();

        updateShader(shader, camera, perspective_matrix);
        for(int i = 0; i < models.size(); i++) {
            shader.texture = textures[i];
            pipeline.renderingModel(*models[i], shader);
        }
        window.draw(pipeline.data(), 0, 0, window_width, window_height);
        
        if(now = clock(); (now - begin) / CLOCKS_PER_SEC) {
            printf("fps: %u\n", frame_count);
            frame_count = 0;
            begin = now;
        }
        frame_count++;
    }
    

    return 0;
}