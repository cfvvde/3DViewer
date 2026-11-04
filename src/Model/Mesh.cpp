
#include "Mesh.h"

#include <cstddef>
#include <limits>
#include <stdexcept>

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices
)
{
    if (vertices.empty() || indices.empty())
    {
        throw std::invalid_argument(
            "Mesh cannot be created from empty geometry"
        );
    }

    if (indices.size() >
        static_cast<std::size_t>(
            std::numeric_limits<GLsizei>::max()))
    {
        throw std::length_error("Too many mesh indices");
    }

    indexCount = static_cast<GLsizei>(indices.size());

    // Создаём OpenGL-объекты
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

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

    // Цвет вершин
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(
            offsetof(Vertex, color)
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
}

void Mesh::draw() const
{
    glBindVertexArray(VAO);

    glDrawElements(
        GL_TRIANGLES,
        indexCount,
        GL_UNSIGNED_INT,
        nullptr
    );

    glBindVertexArray(0);
}
