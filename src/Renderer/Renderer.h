
#pragma once

#include "Shader.h"
#include "../Model/Model.h"

#include <glm/glm.hpp>

enum class ShadingMode
{
    Solid,
    Wireframe
};

struct RenderSettings
{
    ShadingMode shading = ShadingMode::Solid;
    glm::vec3 surfaceColor{0.66f, 0.70f, 0.76f};
    glm::vec3 wireColor{0.81f, 0.48f, 0.50f};
    glm::vec3 backgroundColor{0.17f, 0.18f, 0.20f};
    float ambient = 0.32f;
    float diffuse = 0.68f;
    float specular = 0.32f;
    float shininess = 36.0f;
};

class Renderer
{
public:
    Renderer();

    void draw(
        const Model& modelObject,
        const glm::mat4& model,
        const glm::mat4& view,
        const glm::mat4& projection,
        const RenderSettings& settings
    ) const;

private:
    Shader shader;
};
