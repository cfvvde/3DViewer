
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

Camera::Camera()
{
    reset();

    fov = 45.0f;
    nearPlane = 0.1f;
    farPlane = 100.0f;
}

void Camera::reset()
{
    target = glm::vec3(0.0f, 0.0f, 0.0f);

    yaw = 58.0f;
    pitch = 22.0f;

    distance = 3.0f;

    minDistance = 0.03f;
    maxDistance = 1000.0f;
}

// Вычисляем положение камеры
// относительно точки target
glm::vec3 Camera::getPosition() const
{
    float yawRadians = glm::radians(yaw);
    float pitchRadians = glm::radians(pitch);

    glm::vec3 position;

    position.x = distance *
        std::cos(pitchRadians) *
        std::cos(yawRadians);

    position.y = distance *
        std::sin(pitchRadians);

    position.z = distance *
        std::cos(pitchRadians) *
        std::sin(yawRadians);

    return target + position;
}

// Матрица вида
glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(
        getPosition(),
        target,
        worldUp
    );
}

// Матрица перспективной проекции
glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const
{
    if (aspectRatio <= 0.0f)
        return glm::mat4(1.0f);

    return glm::perspective(
        glm::radians(fov),
        aspectRatio,
        nearPlane,
        farPlane
    );
}

// Вращение камеры вокруг модели
void Camera::orbit(float deltaX, float deltaY)
{
    yaw += deltaX * orbitSensitivity;

    pitch += deltaY * orbitSensitivity;

    // Не даём камере перевернуться
    pitch = std::clamp(pitch, -89.0f, 89.0f);
}

// Перемещение камеры относительно модели
void Camera::pan(
    float deltaX,
    float deltaY,
    float viewportHeight
)
{
    if (viewportHeight <= 0.0f)
        return;

    glm::vec3 position = getPosition();

    glm::vec3 front = glm::normalize(target - position);

    glm::vec3 right = glm::normalize(
        glm::cross(front, worldUp)
    );

    glm::vec3 cameraUp = glm::normalize(
        glm::cross(right, front)
    );

    // Рассчитываем скорость перемещения
    // в мировых координатах на один пиксель
    float worldUnitsPerPixel =
        (2.0f * distance *
        std::tan(glm::radians(fov) * 0.5f))
        / viewportHeight;

    // Перемещаем точку вращения вместе с камерой
    target += (
        -deltaX * right +
        deltaY * cameraUp
    ) * worldUnitsPerPixel;
}

// Приближение и отдаление
void Camera::zoom(float scrollOffset)
{
    float zoomFactor = std::exp(
        -scrollOffset * zoomSensitivity
    );

    distance *= zoomFactor;

    // Ограничения приближения
    distance = std::clamp(
        distance,
        minDistance,
        maxDistance
    );
}

void Camera::frame(
    const glm::vec3& center,
    float radius,
    float aspectRatio
)
{
    target = center;

    radius = std::max(radius, 0.001f);

    const float verticalHalfFov = glm::radians(fov) * 0.5f;
    const float halfFov = std::atan(std::tan(verticalHalfFov) * std::min(aspectRatio, 1.0f));

    // Подбираем расстояние камеры,
    // чтобы объект целиком помещался в кадр
    distance = radius / std::sin(halfFov) * 1.25f;

    // Настраиваем диапазон приближения
    minDistance = std::max(radius * 0.01f, 0.0001f);
    maxDistance = std::max(radius * 100.0f, 100.0f);

    distance = std::clamp(
        distance,
        minDistance,
        maxDistance
    );

    // Настраиваем плоскости отсечения
    nearPlane = std::max(radius * 0.0001f, 0.001f);
    farPlane = std::max(radius * 100.0f, 100.0f);
}
