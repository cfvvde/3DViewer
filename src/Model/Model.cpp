
#include "Model.h"

Model::Model(
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    float radius
)
    : mesh(vertices, indices),
      boundingRadius(radius)
{
}

void Model::draw() const
{
    mesh.draw();
}

float Model::getBoundingRadius() const
{
    return boundingRadius;
}
