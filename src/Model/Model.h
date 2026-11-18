
#pragma once

#include "Mesh.h"

#include <vector>

class Model
{
public:
    Model(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        const std::vector<unsigned int>& edgeIndices,
        float boundingRadius
    );

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    void draw() const;
    void drawWireframe() const;

    float getBoundingRadius() const;

private:
    Mesh mesh;
    float boundingRadius;
};
