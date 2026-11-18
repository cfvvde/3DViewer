#pragma once

#include <string>

#include <imgui.h>

#include "Renderer/Renderer.h"

struct GLFWwindow;

struct ViewerActions
{
    bool openModel = false;
    bool frameModel = false;
};

class ViewerUI
{
public:
    explicit ViewerUI(GLFWwindow* window);
    ~ViewerUI();

    ViewerUI(const ViewerUI&) = delete;
    ViewerUI& operator=(const ViewerUI&) = delete;

    void beginFrame();
    ViewerActions draw(RenderSettings& settings, const std::string& modelName, const std::string& error);
    void drawViewportOverlay(const glm::mat4& view) const;
    void render() const;

    ImVec2 viewportMin() const { return viewportTopLeft; }
    ImVec2 viewportMax() const { return viewportBottomRight; }

private:
    ImVec2 viewportTopLeft{};
    ImVec2 viewportBottomRight{};
};
