#include <fstream>
#include <optional>
#include "model.hpp"

class StringPraser {
private:
    std::string_view str;
    size_t now;
    size_t pre;

public:
    StringPraser(std::string_view str): str(str), now(0), pre(0) {}

public:
    std::optional<std::string_view> nextLine() { 
        if(now == str.size()) return std::nullopt;

        auto p = str.find('\n', now);
        
        if(p == std::string::npos) {
            pre = now;
            now = str.size();
            return std::string_view(str.substr(pre));
        }
        
        pre = now;
        now = p + 1;
        return std::string_view(str.substr(pre, p - pre));
    }
};

void Model::loadModel(const std::string& obj_file_path) {
    using namespace std;

    string full_dir_path = model_dir_path_ + obj_file_path;
    string res;
    
    ifstream file(full_dir_path, ios::in | ios::binary | ios::ate);
    
    if(!file.is_open()) {
        throw std::runtime_error("Model::Model(): Cannot open file " + obj_file_path);
    }

    res.resize(file.tellg());
    file.seekg(0, ios::beg);
    file.read((char*) res.data(), res.size());
    file.close();

    StringPraser praser(res);
    praseObj(praser, false);
}



void Model::praseObj(StringPraser& praser, bool recurse) {
    auto& model = models_.emplace_back();

    std::optional<std::string_view> line = std::nullopt;
    while((line = praser.nextLine()) != std::nullopt) {

    }
}