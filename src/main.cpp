#include <algorithm>
#include <cmath>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <nfd.h>
#include <nfd_glfw3.h>

#include "Camera/Camera.h"
#include "Model/ModelLoader.h"
#include "Renderer/Renderer.h"
#include "UI/ViewerUI.h"

namespace
{
    void loadModel(
        const std::string& path,
        std::unique_ptr<Model>& model,
        std::string& modelName,
        std::string& error
    )
    {
        try
        {
            auto nextModel = ModelLoader::loadOBJ(path);
            modelName = std::filesystem::u8path(path).filename().u8string();
            model = std::move(nextModel);
            error.clear();
        }
        catch (const std::exception& exception)
        {
            error = exception.what();
            std::cerr << error << '\n';
        }
    }

    bool openModelDialog(
        GLFWwindow* window,
        std::unique_ptr<Model>& model,
        std::string& modelName,
        std::string& error
    )
    {
        const nfdu8filteritem_t filters[] = {{"Wavefront OBJ", "obj"}};
        nfdopendialogu8args_t args{};
        args.filterList = filters;
        args.filterCount = 1;
        args.title = "Open 3D Model";
        args.acceptLabel = "Open";
        NFD_GetNativeWindowFromGLFWWindow(window, &args.parentWindow);

        nfdu8char_t* selectedPath = nullptr;
        const nfdresult_t result = NFD_OpenDialogU8_With(&selectedPath, &args);
        if (result == NFD_CANCEL)
            return false;
        if (result == NFD_ERROR)
        {
            error = NFD_GetError();
            std::cerr << error << '\n';
            return false;
        }

        const std::unique_ptr<nfdu8char_t, decltype(&NFD_FreePathU8)> path(selectedPath, NFD_FreePathU8);
        loadModel(path.get(), model, modelName, error);
        return error.empty();
    }

    void updateCamera(GLFWwindow* window, Camera& camera, const ViewerUI& ui, bool& dragging)
    {
        const ImGuiIO& io = ImGui::GetIO();
        if (!glfwGetWindowAttrib(window, GLFW_FOCUSED) || !ImGui::IsMouseDown(ImGuiMouseButton_Middle))
            dragging = false;

        const ImVec2 min = ui.viewportMin();
        const ImVec2 max = ui.viewportMax();
        const bool overViewport = !io.WantCaptureMouse &&
            ImGui::IsMouseHoveringRect(min, max, false);

        if (overViewport)
        {
            if (io.MouseWheel != 0.0f)
                camera.zoom(io.MouseWheel);
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
            {
                dragging = true;
                return;
            }
        }

        if (!dragging)
            return;

        if (io.KeyShift)
            camera.pan(io.MouseDelta.x, io.MouseDelta.y, max.y - min.y);
        else
            camera.orbit(io.MouseDelta.x, io.MouseDelta.y);
    }

    void runViewer(GLFWwindow* window, const char* programPath)
    {
        Camera camera;
        Renderer renderer;
        ViewerUI ui(window);
        RenderSettings settings;
        std::unique_ptr<Model> model;
        std::string modelName;
        std::string error;
        const std::filesystem::path samplePath("assets/models/test.obj");
        std::error_code fileError;
        const auto startPath = std::filesystem::exists(samplePath, fileError)
            ? samplePath
            : std::filesystem::u8path(programPath).parent_path() / samplePath;
        loadModel(startPath.u8string(), model, modelName, error);

        bool frameRequested = static_cast<bool>(model);
        bool dragging = false;
        glEnable(GL_DEPTH_TEST);

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            int framebufferWidth, framebufferHeight;
            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
            if (framebufferWidth == 0 || framebufferHeight == 0)
            {
                dragging = false;
                glfwWaitEventsTimeout(0.1);
                continue;
            }

            ui.beginFrame();
            const ImGuiIO& io = ImGui::GetIO();
            const bool keyboardAvailable = !io.WantCaptureKeyboard;
            if (keyboardAvailable && !io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z, false))
            {
                settings.shading = settings.shading == ShadingMode::Solid
                    ? ShadingMode::Wireframe
                    : ShadingMode::Solid;
            }

            const ViewerActions actions = ui.draw(settings, modelName, error);
            if (actions.openModel || (keyboardAvailable && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_O, false)))
            {
                dragging = false;
                frameRequested |= openModelDialog(window, model, modelName, error);
            }
            else
            {
                updateCamera(window, camera, ui, dragging);
            }
            frameRequested |= actions.frameModel ||
                (keyboardAvailable && !io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_F, false));

            const ImVec2 min = ui.viewportMin();
            const ImVec2 max = ui.viewportMax();
            const int left = static_cast<int>(std::lround(min.x * io.DisplayFramebufferScale.x));
            const int top = static_cast<int>(std::lround(min.y * io.DisplayFramebufferScale.y));
            const int right = static_cast<int>(std::lround(max.x * io.DisplayFramebufferScale.x));
            const int bottom = static_cast<int>(std::lround(max.y * io.DisplayFramebufferScale.y));

            glClearColor(settings.backgroundColor.r, settings.backgroundColor.g, settings.backgroundColor.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const int width = right - left;
            const int height = bottom - top;
            if (model && width > 0 && height > 0)
            {
                const float aspect = static_cast<float>(width) / static_cast<float>(height);
                if (frameRequested)
                {
                    camera.frame(glm::vec3(0.0f), model->getBoundingRadius(), aspect);
                    frameRequested = false;
                }

                glViewport(left, framebufferHeight - bottom, width, height);
                renderer.draw(*model, glm::mat4(1.0f), camera.getViewMatrix(),
                    camera.getProjectionMatrix(aspect), settings);
            }

            ui.drawViewportOverlay(camera.getViewMatrix());
            glViewport(0, 0, framebufferWidth, framebufferHeight);
            ui.render();
            glfwSwapBuffers(window);
        }
    }
}

int main(int argc, char* argv[])
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "3D Viewer", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return 1;
    }

    glfwSetWindowSizeLimits(window, 700, 460, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    int exitCode = 1;
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "GLAD initialization failed\n";
    }
    else if (NFD_Init() != NFD_OKAY)
    {
        std::cerr << "NFD initialization failed: " << NFD_GetError() << '\n';
    }
    else
    {
        try
        {
            runViewer(window, argc > 0 ? argv[0] : "");
            exitCode = 0;
        }
        catch (const std::exception& exception)
        {
            std::cerr << "Application error: " << exception.what() << '\n';
        }
        NFD_Quit();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return exitCode;
}
