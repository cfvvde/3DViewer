
#pragma once

#include <memory>
#include <string>

class Model;

class ModelLoader
{
public:
    static std::unique_ptr<Model> loadOBJ(
        const std::string& filepath
    );
};
