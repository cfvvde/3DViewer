
#include "Renderer.h"

namespace
{
    const char* vertexShaderSource = R"(
        #version 330 core

        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec3 aColor;

        out vec3 vertexColor;

        uniform mat4 uMVP;

        void main()
        {
            gl_Position = uMVP * vec4(aPosition, 1.0);
            vertexColor = aColor;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core

        in vec3 vertexColor;

        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(vertexColor, 1.0);
        }
    )";
}

Renderer::Renderer()
    : shader(vertexShaderSource, fragmentShaderSource)
{
}

void Renderer::draw(
    const Model& modelObject,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection
) const
{
    glm::mat4 mvp = projection * view * model;

    shader.use();

    shader.setMat4("uMVP", mvp);

    modelObject.draw();
}
