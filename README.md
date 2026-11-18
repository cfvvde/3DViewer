
# Simple 3D Viewer

A lightweight interactive 3D model viewer developed in C++ using OpenGL.

The application allows users to load Wavefront OBJ models and inspect them using mouse-based camera controls.

## Features

- OBJ model loading using Assimp
- Interactive orbit camera
- Camera panning
- Zoom in / zoom out
- Automatic model centering
- Automatic camera framing
- Native file selection dialog
- Solid shading with Blinn–Phong lighting and imported or generated normals
- Wireframe view of the original OBJ polygon edges
- Blender-inspired Dear ImGui toolbar and viewport settings

## Technologies

- C++17
- OpenGL 3.3 Core
- GLFW
- GLAD
- GLM
- Assimp
- Native File Dialog Extended
- Dear ImGui
- CMake
- vcpkg

## Project Structure

```text
3DViewer/
├── assets/
│   └── models/
├── src/
│   ├── Camera/
│   ├── Model/
│   ├── Renderer/
│   ├── UI/
│   └── main.cpp
├── CMakeLists.txt
├── CMakePresets.json
└── vcpkg.json
```

## Requirements

- Windows 10/11
- Visual Studio 2026 Build Tools with C++ support
- CMake 3.21 or newer
- Git
- vcpkg

## Dependencies

All external libraries are managed using vcpkg.

Dependencies are declared in `vcpkg.json`.

Make sure the `VCPKG_ROOT` environment variable points to your vcpkg installation directory.

## Build Instructions

Clone the repository:

```bash
git clone https://github.com/YOUR_USERNAME/3DViewer.git
cd 3DViewer
```

Configure the project:

```bash
cmake --preset default
```

Build:

```bash
cmake --build --preset debug
```

Run:

```powershell
.\build\Debug\3DViewer.exe
```

## Controls

| Action | Input |
|---|---|
| Orbit | Middle Mouse Button |
| Pan | Shift + Middle Mouse Button |
| Zoom | Mouse Wheel |
| Open Model | Open OBJ… or Ctrl + O |
| Frame Model | Frame Model or F |
| Switch Solid / Wireframe | Toolbar or Z |

Use the right-hand panel to change the background, surface or wire color.
Solid mode also exposes ambient, diffuse and specular intensity and shininess.

## Supported Formats

- OBJ

## Project Status

Early development / MVP.

## License

Not specified yet.
