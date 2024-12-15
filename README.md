# Simple 2D Physics Engine
This application represents simple 2D physics engine that simulate world with solid dynamic and static objects that can move and rotate.

<img src="https://github.com/user-attachments/assets/275252e0-1782-4bd6-bb02-52ad7e951696" height="400" />

## Build
Requirements
- VcPkg
- VisualStudio 2022

To build, open visual studio solution and build by UI.
## Graphics
Engine uses Vulkan as graphics api. As window manager used glfw. Most of the graphics code written by following [this](https://vulkan-tutorial.com/Introduction) tutorial.

Libraries used:
- [Vulkan Api](https://github.com/KhronosGroup/Vulkan-Hpp)
- [Vulkan Memory Allocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [Vulkan Utility Libraries](https://github.com/KhronosGroup/Vulkan-Utility-Libraries)
- [ImGUI](https://github.com/ocornut/imgui)
- [Glfw](https://github.com/glfw/glfw)
## Physics
Engine implements simple 2D collision detection and resolusion. Used GJK and EPA algorithms to detect collisions.

Most of the physics part developed by [this youtube series](https://www.youtube.com/playlist?list=PLSlpr6o9vURwq3oxVZSimY8iC-cdd3kIs) about 2D physics Engine.

GJK and EPA algorithm mostly driven from [this](https://winter.dev/articles/gjk-algorithm) article about Collision detection algorithm in 2D/3D and [this](http://allenchou.net/game-physics-series/) series of articles about developing Game Physics engine.

Libraries used:
- [glm](https://github.com/g-truc/glm)

## Other useful utility libs used
- [boost-bind](https://www.boost.io/library/latest/bind/)
- [boost-signals2](https://www.boost.io/library/latest/signals2/)
- [magic-enum](https://github.com/Neargye/magic_enum)
- [lodepng](https://github.com/lvandeve/lodepng)

## Video example

https://github.com/user-attachments/assets/dfde0a68-da70-4fe6-9bbb-2ff0bf7bc598


