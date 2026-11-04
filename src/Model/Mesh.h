
#pragma once

#include <glad/glad.h>

#include <vector>

struct Vertex
{
    float position[3];
    float color[3];
};

class Mesh
{
public:
    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices
    );

    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    GLsizei indexCount = 0;
};
