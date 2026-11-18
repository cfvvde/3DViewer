
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
    GLuint compileShader(
        GLenum type,
        const char* source
    )
    {
        GLuint shader = glCreateShader(type);

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;

        glGetShaderiv(
            shader,
            GL_COMPILE_STATUS,
            &success
        );

        if (!success)
        {
            GLint length = 0;

            glGetShaderiv(
                shader,
                GL_INFO_LOG_LENGTH,
                &length
            );

            std::string log(
                static_cast<std::size_t>(length),
                '\0'
            );

            glGetShaderInfoLog(
                shader,
                length,
                nullptr,
                log.data()
            );

            glDeleteShader(shader);

            throw std::runtime_error(
                "Shader compilation failed:\n" + log
            );
        }

        return shader;
    }
}

Shader::Shader(
    const char* vertexSource,
    const char* fragmentSource
)
{
    GLuint vertexShader =
        compileShader(GL_VERTEX_SHADER, vertexSource);

    GLuint fragmentShader = 0;

    try
    {
        fragmentShader =
            compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    }
    catch (...)
    {
        glDeleteShader(vertexShader);
        throw;
    }

    program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint success = GL_FALSE;

    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &success
    );

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!success)
    {
        GLint length = 0;

        glGetProgramiv(
            program,
            GL_INFO_LOG_LENGTH,
            &length
        );

        std::string log(
            static_cast<std::size_t>(length),
            '\0'
        );

        glGetProgramInfoLog(
            program,
            length,
            nullptr,
            log.data()
        );

        glDeleteProgram(program);
        program = 0;

        throw std::runtime_error(
            "Shader linking failed:\n" + log
        );
    }
}

Shader::~Shader()
{
    if (program)
        glDeleteProgram(program);
}

void Shader::use() const
{
    glUseProgram(program);
}

void Shader::setMat4(
    const char* name,
    const glm::mat4& matrix
) const
{
    GLint location = glGetUniformLocation(program, name);

    if (location == -1)
        return;

    glUniformMatrix4fv(
        location,
        1,
        GL_FALSE,
        glm::value_ptr(matrix)
    );
}

void Shader::setMat3(const char* name, const glm::mat3& matrix) const
{
    glUniformMatrix3fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setVec3(const char* name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(program, name), 1, glm::value_ptr(value));
}

void Shader::setFloat(const char* name, float value) const
{
    glUniform1f(glGetUniformLocation(program, name), value);
}

void Shader::setBool(const char* name, bool value) const
{
    glUniform1i(glGetUniformLocation(program, name), value ? GL_TRUE : GL_FALSE);
}
