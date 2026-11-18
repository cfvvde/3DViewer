
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
    void setMat3(const char* name, const glm::mat3& matrix) const;
    void setVec3(const char* name, const glm::vec3& value) const;
    void setFloat(const char* name, float value) const;
    void setBool(const char* name, bool value) const;


private:
    GLuint program = 0;
};
