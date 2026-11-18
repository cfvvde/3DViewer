
#include "Mesh.h"

#include <cstddef>
#include <limits>
#include <stdexcept>

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    const std::vector<unsigned int>& edgeIndices
)
{
    if (vertices.empty() || indices.empty() || edgeIndices.empty())
    {
        throw std::invalid_argument(
            "Mesh cannot be created from empty geometry"
        );
    }

    const auto maxCount = static_cast<std::size_t>(std::numeric_limits<GLsizei>::max());
    if (indices.size() > maxCount || edgeIndices.size() > maxCount)
    {
        throw std::length_error("Too many mesh indices");
    }

    indexCount = static_cast<GLsizei>(indices.size());
    edgeIndexCount = static_cast<GLsizei>(edgeIndices.size());

    // Создаём OpenGL-объекты
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &edgeEBO);

    glBindVertexArray(VAO);

    // Загружаем вершины
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(
            vertices.size() * sizeof(Vertex)
        ),
        vertices.data(),
        GL_STATIC_DRAW
    );

    // Загружаем индексы
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(
            indices.size() * sizeof(unsigned int)
        ),
        indices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(edgeIndices.size() * sizeof(unsigned int)),
        edgeIndices.data(),
        GL_STATIC_DRAW
    );

    // Координаты вершин
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(
            offsetof(Vertex, position)
        )
    );

    glEnableVertexAttribArray(0);

    // Нормали вершин
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(
            offsetof(Vertex, normal)
        )
    );

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    if (VAO)
        glDeleteVertexArrays(1, &VAO);

    if (VBO)
        glDeleteBuffers(1, &VBO);

    if (EBO)
        glDeleteBuffers(1, &EBO);
    if (edgeEBO)
        glDeleteBuffers(1, &edgeEBO);
}

void Mesh::draw() const
{
    drawElements(GL_TRIANGLES, EBO, indexCount);
}

void Mesh::drawWireframe() const
{
    drawElements(GL_LINES, edgeEBO, edgeIndexCount);
}

void Mesh::drawElements(GLenum mode, GLuint buffer, GLsizei count) const
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
