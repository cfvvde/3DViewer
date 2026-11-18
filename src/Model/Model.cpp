
#include "Model.h"

Model::Model(
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    const std::vector<unsigned int>& edgeIndices,
    float radius
)
    : mesh(vertices, indices, edgeIndices),
      boundingRadius(radius)
{
}

void Model::draw() const
{
    mesh.draw();
}

void Model::drawWireframe() const
{
    mesh.drawWireframe();
}

float Model::getBoundingRadius() const
{
    return boundingRadius;
}
