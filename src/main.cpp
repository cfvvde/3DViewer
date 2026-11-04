
#include <iostream>
#include <vector>
#include <exception>
#include <memory>

#include "Model/Model.h"
#include "Model/ModelLoader.h"
#include <glad/glad.h>
#include <nfd.h>

#include <memory>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera/Camera.h"
#include "Model/Mesh.h"
#include "Renderer/Renderer.h"

// Состояние управления камерой
struct CameraInputState
{
    Camera* camera = nullptr;

    bool middleMousePressed = false;

    double lastX = 0.0;
    double lastY = 0.0;
};

// Обработка нажатий кнопок мыши
void mouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int
)
{
    if (button != GLFW_MOUSE_BUTTON_MIDDLE)
        return;

    auto* input = static_cast<CameraInputState*>(
        glfwGetWindowUserPointer(window)
    );

    if (!input)
        return;

    if (action == GLFW_PRESS)
    {
        input->middleMousePressed = true;

        glfwGetCursorPos(
            window,
            &input->lastX,
            &input->lastY
        );
    }
    else if (action == GLFW_RELEASE)
    {
        input->middleMousePressed = false;
    }
}

// Обработка движения мыши
void cursorPositionCallback(
    GLFWwindow* window,
    double xpos,
    double ypos
)
{
    auto* input = static_cast<CameraInputState*>(
        glfwGetWindowUserPointer(window)
    );

    if (!input || !input->camera)
        return;

    if (!input->middleMousePressed)
        return;

    // Разница между текущей и предыдущей позицией мыши
    float deltaX = static_cast<float>(
        xpos - input->lastX
    );

    float deltaY = static_cast<float>(
        ypos - input->lastY
    );

    input->lastX = xpos;
    input->lastY = ypos;

    // Проверяем, удерживается ли Shift
    bool shiftPressed =
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

    if (shiftPressed)
    {
        // Shift + MMB = Pan

        int width, height;

        glfwGetFramebufferSize(
            window,
            &width,
            &height
        );

        input->camera->pan(
            deltaX,
            deltaY,
            static_cast<float>(height)
        );
    }
    else
    {
        // MMB = Orbit

        input->camera->orbit(
            deltaX,
            deltaY
        );
    }
}

// Обработка колеса мыши
void scrollCallback(
    GLFWwindow* window,
    double xoffset,
    double yoffset
)
{
    auto* input = static_cast<CameraInputState*>(
        glfwGetWindowUserPointer(window)
    );

    if (!input || !input->camera)
        return;

    // Scroll = Zoom
    input->camera->zoom(
        static_cast<float>(yoffset)
    );
}

void openModelDialog(
    Camera& camera,
    std::unique_ptr<Model>& currentModel
)
{
    // Фильтр для OBJ-файлов
    nfdu8filteritem_t filters[] =
    {
        { "Wavefront OBJ", "obj" }
    };

    // Настройки диалогового окна
    nfdopendialogu8args_t args{};

    args.filterList = filters;
    args.filterCount = 1;
    args.title = "Open 3D Model";
    args.acceptLabel = "Open";

    nfdu8char_t* outPath = nullptr;

    // Открываем системный диалог
    nfdresult_t result = NFD_OpenDialogU8_With(
        &outPath,
        &args
    );

    // Пользователь закрыл окно
    if (result == NFD_CANCEL)
    {
        std::cout << "Model selection cancelled\n";
        return;
    }

    // Ошибка диалога
    if (result == NFD_ERROR)
    {
        std::cerr << "File dialog error: "
                  << NFD_GetError() << std::endl;
        return;
    }

    // Сохраняем путь в std::string
    std::string filepath(
        reinterpret_cast<const char*>(outPath)
    );

    // Освобождаем память, выделенную NFD
    NFD_FreePathU8(outPath);

    try
    {
        std::cout << "Loading model: "
                  << filepath << std::endl;

        // Сначала загружаем новую модель
        // во временный объект
        std::unique_ptr<Model> newModel =
            ModelLoader::loadOBJ(filepath);

        // Настраиваем камеру под размеры модели
        camera.frame(
            glm::vec3(0.0f),
            newModel->getBoundingRadius()
        );

        // Только после успешной загрузки
        // заменяем предыдущую модель
        currentModel = std::move(newModel);

        std::cout << "Model loaded successfully!\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Model loading error:\n"
                  << e.what() << std::endl;

        // Предыдущая модель остаётся на экране
    }
}


int main()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed\n";
        return -1;
    }
    // Инициализация Native File Dialog
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280,
        720,
        "Simple 3D Viewer",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(
        reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "GLAD initialization failed\n";

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }
    if (NFD_Init() != NFD_OKAY)
    {
        std::cerr << "NFD initialization failed: "
                << NFD_GetError() << std::endl;

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    int exitCode = 0;

    try
    {
        {
            // Создаём камеру и рендерер
            Camera camera;

            Renderer renderer;

            // Загружаем OBJ-модель
            std::unique_ptr<Model> model =
                ModelLoader::loadOBJ(
                    "assets/models/test.obj"
                );
            // Состояние сочетания Ctrl + O
            bool ctrlOWasPressed = false;
            // Автоматически настраиваем камеру
            camera.frame(
                glm::vec3(0.0f),
                model->getBoundingRadius()
            );

            // Настраиваем обработку мыши
            CameraInputState cameraInput;

            cameraInput.camera = &camera;

            glfwSetWindowUserPointer(
                window,
                &cameraInput
            );

            glfwSetMouseButtonCallback(
                window,
                mouseButtonCallback
            );

            glfwSetCursorPosCallback(
                window,
                cursorPositionCallback
            );

            glfwSetScrollCallback(
                window,
                scrollCallback
            );

            std::cout << "Model loaded successfully!\n";

            // Главный цикл
            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();

                // Проверяем Ctrl
                bool ctrlPressed =
                    glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                    glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;

                // Проверяем O
                bool oPressed =
                    glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS;

                // Проверяем сочетание Ctrl + O
                bool ctrlOPressed = ctrlPressed && oPressed;

                // Обрабатываем только новое нажатие,
                // чтобы диалог не открывался несколько раз
                if (ctrlOPressed && !ctrlOWasPressed)
                {
                    // Сбрасываем состояние вращения мыши
                    cameraInput.middleMousePressed = false;

                    // Открываем диалог выбора OBJ
                    openModelDialog(camera, model);
                }

                ctrlOWasPressed = ctrlOPressed;

                int width, height;

                glfwGetFramebufferSize(
                    window,
                    &width,
                    &height
                );

                glViewport(0, 0, width, height);

                glClearColor(
                    0.08f,
                    0.10f,
                    0.16f,
                    1.0f
                );

                glClear(
                    GL_COLOR_BUFFER_BIT |
                    GL_DEPTH_BUFFER_BIT
                );

                if (width > 0 && height > 0)
                {
                    // Модель пока неподвижна
                    glm::mat4 modelMatrix(1.0f);

                    // Получаем матрицы камеры
                    glm::mat4 view =
                        camera.getViewMatrix();

                    float aspectRatio =
                        static_cast<float>(width) /
                        static_cast<float>(height);

                    glm::mat4 projection =
                        camera.getProjectionMatrix(aspectRatio);

                    // Отрисовываем загруженную модель
                    renderer.draw(
                        *model,
                        modelMatrix,
                        view,
                        projection
                    );
                }

                glfwSwapBuffers(window);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Application error:\n"
                  << e.what() << std::endl;

        exitCode = 1;
    }

    NFD_Quit();

    glfwDestroyWindow(window);
    glfwTerminate();

    return exitCode;
}
