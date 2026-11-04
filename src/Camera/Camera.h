
#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera();

    // Получение матриц камеры
    glm::mat4 getViewMatrix() const;

    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    void frame(const glm::vec3& center, float radius);
    // Управление камерой
    void orbit(float deltaX, float deltaY);

    void pan(float deltaX, float deltaY, float viewportHeight);

    void zoom(float scrollOffset);

    // Возвращение камеры в исходное положение
    void reset();

private:
    // Вычисление позиции камеры из сферических координат
    glm::vec3 getPosition() const;

private:
    glm::vec3 target;

    float yaw;
    float pitch;
    float distance;

    float fov;
    float nearPlane;
    float farPlane;

    float minDistance;
    float maxDistance;

    const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    const float orbitSensitivity = 0.25f;
    const float zoomSensitivity = 0.12f;
};
