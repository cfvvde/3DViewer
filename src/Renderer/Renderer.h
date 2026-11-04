
#pragma once

#include "Shader.h"
#include "../Model/Model.h"

#include <glm/glm.hpp>

class Renderer
{
public:
    Renderer();

    void draw(
        const Model& modelObject,
        const glm::mat4& model,
        const glm::mat4& view,
        const glm::mat4& projection
    ) const;

private:
    Shader shader;
};
