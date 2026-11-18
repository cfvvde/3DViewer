#include "ViewerUI.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <algorithm>
#include <array>
#include <stdexcept>

namespace
{
    const ImVec4 accent{0.81f, 0.48f, 0.50f, 1.0f};
    const ImVec4 accentActive{0.87f, 0.57f, 0.58f, 1.0f};
    const ImVec4 accentFill{0.23f, 0.14f, 0.16f, 1.0f};
    const ImVec4 accentFillHovered{0.30f, 0.18f, 0.20f, 1.0f};
    const ImVec4 mutedText{0.67f, 0.70f, 0.75f, 1.0f};

    constexpr ImGuiWindowFlags panelFlags = ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

    void setPanelBounds(float x, float y, float width, float height)
    {
        ImGui::SetNextWindowPos(ImVec2(x, y));
        ImGui::SetNextWindowSize(ImVec2(width, height));
    }

    void setStyle()
    {
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.FontSizeBase = 15.0f;
        style.WindowPadding = ImVec2(14, 10);
        style.WindowMinSize = ImVec2(1, 1);
        style.FramePadding = ImVec2(10, 5);
        style.ItemSpacing = ImVec2(8, 8);
        style.WindowRounding = 0.0f;
        style.FrameRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.PopupRounding = 5.0f;

        auto* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.105f, 0.112f, 0.127f, 1.0f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
        colors[ImGuiCol_Border] = ImVec4(0.22f, 0.24f, 0.27f, 1.0f);
        colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
        colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.19f, 0.22f, 1.0f);
        colors[ImGuiCol_Button] = colors[ImGuiCol_FrameBg];
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.26f, 0.30f, 1.0f);
        colors[ImGuiCol_ButtonHovered] = colors[ImGuiCol_FrameBgHovered];
        colors[ImGuiCol_ButtonActive] = accentFillHovered;
        colors[ImGuiCol_FrameBgActive] = colors[ImGuiCol_ButtonActive];
        colors[ImGuiCol_SliderGrab] = accent;
        colors[ImGuiCol_SliderGrabActive] = accentActive;
        colors[ImGuiCol_CheckMark] = accent;
        colors[ImGuiCol_NavCursor] = accent;
        colors[ImGuiCol_TextSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.32f);
    }

    void shadingButton(const char* label, ShadingMode option, RenderSettings& settings)
    {
        const bool active = option == settings.shading;
        if (active)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, accentFill);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, accentFillHovered);
            ImGui::PushStyleColor(ImGuiCol_Text, accent);
        }

        const bool clicked = ImGui::Button(label);
        if (active)
            ImGui::PopStyleColor(3);

        if (clicked)
            settings.shading = option;
    }

    void drawColor(const char* label, glm::vec3& color)
    {
        ImGui::TextUnformatted(label);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::PushID(label);
        ImGui::ColorEdit3("##color", &color.x, ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_NoOptions);
        ImGui::PopID();
    }

    void drawSlider(const char* label, float& value, float minimum, float maximum, const char* format)
    {
        ImGui::TextUnformatted(label);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::PushID(label);
        ImGui::SliderFloat("##value", &value, minimum, maximum, format, ImGuiSliderFlags_AlwaysClamp);
        ImGui::PopID();
    }
}

ViewerUI::ViewerUI(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.FontDefault = io.Fonts->AddFontDefaultVector();
    setStyle();

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
    {
        ImGui::DestroyContext();
        throw std::runtime_error("Could not initialize the ImGui GLFW backend");
    }

    if (!ImGui_ImplOpenGL3_Init("#version 330 core"))
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        throw std::runtime_error("Could not initialize the ImGui OpenGL backend");
    }
}

ViewerUI::~ViewerUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ViewerUI::beginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

ViewerActions ViewerUI::draw(RenderSettings& settings, const std::string& modelName, const std::string& error)
{
    const ImVec2 size = ImGui::GetIO().DisplaySize;
    const float toolbarHeight = ImGui::GetFrameHeight() + 2.0f * ImGui::GetStyle().WindowPadding.y;
    const float statusHeight = ImGui::GetTextLineHeight() + 12.0f;
    const float propertiesWidth = std::clamp(size.x * 0.25f, 234.0f, 288.0f);
    viewportTopLeft = ImVec2(0.0f, toolbarHeight);
    viewportBottomRight = ImVec2(size.x - propertiesWidth, size.y - statusHeight);

    ViewerActions actions;
    setPanelBounds(0.0f, 0.0f, size.x, toolbarHeight);
    ImGui::Begin("##toolbar", nullptr, panelFlags | ImGuiWindowFlags_NoScrollbar);
    ImGui::AlignTextToFramePadding();
    ImGui::TextColored(accent, "3D VIEWER");
    ImGui::SameLine(0.0f, 20.0f);
    actions.openModel = ImGui::Button("Open OBJ...");
    ImGui::SetItemTooltip("Open an OBJ file (Ctrl+O)");
    ImGui::SameLine();
    ImGui::BeginDisabled(modelName.empty());
    actions.frameModel = ImGui::Button("Frame Model");
    ImGui::EndDisabled();
    ImGui::SetItemTooltip("Center the view on the model (F)");
    ImGui::SameLine(0.0f, 24.0f);
    ImGui::AlignTextToFramePadding();
    ImGui::TextColored(mutedText, "Shading");
    ImGui::SameLine();
    shadingButton("Solid", ShadingMode::Solid, settings);
    ImGui::SameLine();
    shadingButton("Wireframe", ShadingMode::Wireframe, settings);
    ImGui::SetItemTooltip("Toggle Solid / Wireframe (Z)");
    ImGui::End();

    setPanelBounds(viewportBottomRight.x, toolbarHeight, propertiesWidth, viewportBottomRight.y - toolbarHeight);
    ImGui::Begin("##properties", nullptr, panelFlags);
    ImGui::TextColored(mutedText, "SCENE");
    ImGui::Separator();
    ImGui::TextWrapped("%s", modelName.empty() ? "No model loaded" : modelName.c_str());
    if (!error.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.49f, 0.42f, 1.0f));
        ImGui::TextWrapped("%s", error.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::Spacing();
    ImGui::TextColored(mutedText, "VIEWPORT");
    ImGui::Separator();
    drawColor("Background", settings.backgroundColor);

    if (settings.shading == ShadingMode::Wireframe)
    {
        drawColor("Wire color", settings.wireColor);
    }
    else
    {
        drawColor("Surface color", settings.surfaceColor);
        ImGui::Spacing();
        ImGui::TextColored(mutedText, "STUDIO LIGHT");
        ImGui::Separator();
        drawSlider("Ambient", settings.ambient, 0.0f, 1.0f, "%.2f");
        drawSlider("Diffuse", settings.diffuse, 0.0f, 1.5f, "%.2f");
        drawSlider("Specular", settings.specular, 0.0f, 1.0f, "%.2f");
        drawSlider("Shininess", settings.shininess, 1.0f, 128.0f, "%.0f");
    }

    ImGui::Spacing();
    if (ImGui::Button("Reset Settings"))
    {
        const ShadingMode mode = settings.shading;
        settings = RenderSettings{};
        settings.shading = mode;
    }
    ImGui::End();

    setPanelBounds(0.0f, viewportBottomRight.y, size.x, statusHeight);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14, 6));
    ImGui::Begin("##status", nullptr, panelFlags | ImGuiWindowFlags_NoScrollbar);
    ImGui::TextColored(mutedText, "MMB  Orbit     Shift+MMB  Pan     Wheel  Zoom     F  Frame     Z  Shading");
    ImGui::End();
    ImGui::PopStyleVar();

    return actions;
}

void ViewerUI::drawViewportOverlay(const glm::mat4& view) const
{
    auto* drawList = ImGui::GetBackgroundDrawList();
    drawList->AddText(ImVec2(viewportTopLeft.x + 15, viewportTopLeft.y + 15),
        IM_COL32(196, 202, 211, 255), "Perspective");

    struct Axis
    {
        glm::vec3 direction;
        ImU32 color;
        const char* label;
    };

    std::array<Axis, 3> axes{{
        {glm::vec3(view[0]), IM_COL32(239, 102, 115, 255), "X"},
        {glm::vec3(view[1]), IM_COL32(158, 205, 91, 255), "Y"},
        {glm::vec3(view[2]), IM_COL32(99, 169, 242, 255), "Z"}
    }};
    std::sort(axes.begin(), axes.end(), [](const Axis& left, const Axis& right) {
        return left.direction.z < right.direction.z;
    });

    const ImVec2 center(viewportBottomRight.x - 46, viewportTopLeft.y + 45);
    drawList->AddCircleFilled(center, 33.0f, IM_COL32(27, 30, 35, 235));
    for (const Axis& axis : axes)
    {
        const ImVec2 end(center.x + axis.direction.x * 21.0f, center.y - axis.direction.y * 21.0f);
        drawList->AddLine(center, end, axis.color, 1.5f);
        drawList->AddCircleFilled(end, 7.0f, axis.color);
        const ImVec2 labelSize = ImGui::CalcTextSize(axis.label);
        drawList->AddText(ImVec2(end.x - labelSize.x * 0.5f, end.y - labelSize.y * 0.5f),
            IM_COL32(24, 27, 32, 255), axis.label);
    }
}

void ViewerUI::render() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
