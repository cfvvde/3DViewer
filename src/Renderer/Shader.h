
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader(
        const char* vertexSource,
        const char* fragmentSource
    );

    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const;

    void setMat4(
        const char* name,
        const glm::mat4& matrix
    ) const;

private:
    GLuint program = 0;
};
