
#pragma once

#include <glad/glad.h>

#include <vector>

struct Vertex
{
    float position[3];
    float normal[3];
};

class Mesh
{
public:
    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        const std::vector<unsigned int>& edgeIndices
    );

    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;
    void drawWireframe() const;

private:
    void drawElements(GLenum mode, GLuint buffer, GLsizei count) const;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    GLuint edgeEBO = 0;

    GLsizei indexCount = 0;
    GLsizei edgeIndexCount = 0;
};
