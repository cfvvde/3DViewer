
#pragma once

#include "Mesh.h"

#include <vector>

class Model
{
public:
    Model(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        float boundingRadius
    );

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    void draw() const;

    float getBoundingRadius() const;

private:
    Mesh mesh;
    float boundingRadius;
};
