
#include "ModelLoader.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

std::unique_ptr<Model> ModelLoader::loadOBJ(
    const std::string& filepath
)
{
    // 1. Создаём импортёр Assimp
    Assimp::Importer importer;

    // 2. Загружаем модель
    const aiScene* scene = importer.ReadFile(
        filepath,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_PreTransformVertices |
        aiProcess_ValidateDataStructure
    );

    if (!scene || !scene->mRootNode || scene->mNumMeshes == 0)
    {
        throw std::runtime_error(
            "Failed to load model: " +
            filepath + "\n" +
            importer.GetErrorString()
        );
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Начальные границы модели
    glm::vec3 minBounds(
        std::numeric_limits<float>::max()
    );

    glm::vec3 maxBounds(
        std::numeric_limits<float>::lowest()
    );

    // 3. Обрабатываем все Mesh внутри OBJ
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[i];

        if (!mesh || !mesh->HasPositions())
            continue;

        const std::size_t baseVertex = vertices.size();

        if (baseVertex + mesh->mNumVertices >
            std::numeric_limits<unsigned int>::max())
        {
            throw std::runtime_error(
                "Model contains too many vertices"
            );
        }

        // Извлекаем вершины
        for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
        {
            const aiVector3D& position = mesh->mVertices[j];

            Vertex vertex{};

            vertex.position[0] = position.x;
            vertex.position[1] = position.y;
            vertex.position[2] = position.z;

            // Временный единый цвет модели
            vertex.color[0] = 0.75f;
            vertex.color[1] = 0.80f;
            vertex.color[2] = 0.92f;

            vertices.push_back(vertex);

            glm::vec3 p(
                position.x,
                position.y,
                position.z
            );

            minBounds = glm::min(minBounds, p);
            maxBounds = glm::max(maxBounds, p);
        }

        // Извлекаем индексы треугольников
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
        {
            const aiFace& face = mesh->mFaces[j];

            // После aiProcess_Triangulate
            // каждая полигональная грань должна быть треугольником
            if (face.mNumIndices != 3)
                continue;

            for (unsigned int k = 0; k < 3; ++k)
            {
                if (face.mIndices[k] >= mesh->mNumVertices)
                {
                    throw std::runtime_error(
                        "Invalid face index in model"
                    );
                }

                indices.push_back(
                    static_cast<unsigned int>(
                        baseVertex + face.mIndices[k]
                    )
                );
            }
        }
    }

    if (vertices.empty() || indices.empty())
    {
        throw std::runtime_error(
            "The OBJ file contains no triangle geometry"
        );
    }

    // 4. Вычисляем центр модели
    glm::vec3 center = (minBounds + maxBounds) * 0.5f;

    // 5. Центрируем геометрию относительно начала координат
    float maxRadiusSquared = 0.0f;

    for (Vertex& vertex : vertices)
    {
        glm::vec3 position(
            vertex.position[0],
            vertex.position[1],
            vertex.position[2]
        );

        position -= center;

        vertex.position[0] = position.x;
        vertex.position[1] = position.y;
        vertex.position[2] = position.z;

        maxRadiusSquared = std::max(
            maxRadiusSquared,
            glm::dot(position, position)
        );
    }

    // 6. Вычисляем радиус модели
    float radius = std::sqrt(maxRadiusSquared);

    if (!std::isfinite(radius))
    {
        throw std::runtime_error(
            "Invalid model dimensions"
        );
    }

    if (radius < 0.000001f)
        radius = 1.0f;

    // 7. Создаём Model
    return std::make_unique<Model>(
        vertices,
        indices,
        radius
    );
}
