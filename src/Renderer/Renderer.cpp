#include "Renderer.h"

namespace
{
    const char* vertexShaderSource = R"(
        #version 330 core

        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec3 aNormal;

        out vec3 viewPosition;
        out vec3 viewNormal;

        uniform mat4 uModelView;
        uniform mat4 uProjection;
        uniform mat3 uNormalMatrix;

        void main()
        {
            vec4 position = uModelView * vec4(aPosition, 1.0);
            gl_Position = uProjection * position;
            viewPosition = position.xyz;
            viewNormal = uNormalMatrix * aNormal;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core

        in vec3 viewPosition;
        in vec3 viewNormal;
        out vec4 FragColor;

        uniform bool uWireframe;
        uniform vec3 uColor;
        uniform float uAmbient;
        uniform float uDiffuse;
        uniform float uSpecular;
        uniform float uShininess;

        void main()
        {
            if (uWireframe)
            {
                FragColor = vec4(uColor, 1.0);
                return;
            }

            vec3 faceNormal = cross(dFdx(viewPosition), dFdy(viewPosition));
            vec3 normal = gl_FrontFacing ? viewNormal : -viewNormal;
            if (!(dot(normal, normal) > 0.000001))
                normal = faceNormal;

            normal = normalize(normal);
            vec3 light = normalize(vec3(-0.28, 0.62, 1.0));
            vec3 view = normalize(-viewPosition);
            vec3 halfway = normalize(light + view);

            float diffuse = max(dot(normal, light), 0.0);
            float specular = 0.0;
            if (diffuse > 0.0)
                specular = pow(max(dot(normal, halfway), 0.0), uShininess);

            vec3 color = uColor * (uAmbient + uDiffuse * diffuse) + uSpecular * specular;
            FragColor = vec4(color, 1.0);
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
    const glm::mat4& projection,
    const RenderSettings& settings
) const
{
    const glm::mat4 modelView = view * model;
    const bool wireframe = settings.shading == ShadingMode::Wireframe;
    shader.use();
    shader.setMat4("uModelView", modelView);
    shader.setMat4("uProjection", projection);
    shader.setBool("uWireframe", wireframe);

    if (wireframe)
    {
        shader.setVec3("uColor", settings.wireColor);
        modelObject.drawWireframe();
        return;
    }

    shader.setMat3("uNormalMatrix", glm::transpose(glm::inverse(glm::mat3(modelView))));
    shader.setVec3("uColor", settings.surfaceColor);
    shader.setFloat("uAmbient", settings.ambient);
    shader.setFloat("uDiffuse", settings.diffuse);
    shader.setFloat("uSpecular", settings.specular);
    shader.setFloat("uShininess", settings.shininess);
    modelObject.draw();
}
