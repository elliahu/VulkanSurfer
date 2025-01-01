# VulkanSurfer

***Important note:*** This project is in early stage of development. Only Linux (X11) is supported at the moment. 
Windows (Win32) is up next. Please se support section bellow.

A minimal cross-platform object-oriented c++11 header only window library for Vulkan.

This project aims to make window creation with a Vulkan surface and event handling as simple and portable as possible.
If you just need to open a simple window with basing event/input handling for your Vulkan projects, this is the tool you need. 
Library is header-only single file and there is no need for implementation files. Just drop it into you project.

## Simple API
The window API was made so it is as simple as possible. Open a window and create a `VkSurface` in just two lines:
```c++
#define SURFER_PLATFORM_X11
#include "VulkanSurfer.h"

// Create a window
Surfer::Window * window = Surfer::Window::createWindow("Example window", instance, 800, 600 , 100, 100 );

// Get the surface 
VkSurfaceKHR surface = window->getSurface();

// Draw
while (!window->shouldClose()) {
    // poll for events
    window->pollEvents();
    
    // do your rendering
}

// Don't forget to destroy the window
Surfer::Window::destroyWindow(window);
```
See the full example in the `example` directory.

## Callback-based event handling
Event handling is done using simple callback system.
```c++
window->registerKeyPressCallback([](Surfer::KeyCode key) {
    std::cout << "Key pressed " << static_cast<unsigned int>(key) << std::endl;
});
```
Event callbacks:
- `KeyPressCallback` - (keyboard or mouse) key was pressed
- `KeyReleaseCallback` - (keyboard or mouse) key was released
- `MouseMotionCallback` - mouse moved in window (cursor position changed)
- `ResizeCallback` - window was resized
- `MoveCallback` - window was moved on screen
- `CloseCallback` - window was closed
- `MouseEnterExitCallback` - mouse entered / exited the window
- `FocusCallback` - window is in / out of focus

## Platforms
Currently, VulkanSurfer targets windows using Win32 API and Linux using X11. Other native APIs might be added in the future.
MacOS support is **not** currently planned.
- Linux
  - [x] **X11 (Supported)**
  - [ ] *Wayland (Not supported, planned)*
- Windows
  - [x] **Win32 (Supported)**
  - [ ] *WinnApp (Not supported, planned)*
- MacOs
  - [ ] *Metal (Not supported)*

## Unsupported features
Some of the features you may know from larger libraries such as GLFW or SDL2 are not supported in VulkanSurfer.
Some of the unsupported features are already planned and will be implemented as soon as possible. 
Others are not supported and not planned, but may be supported in the future. 
Some features will not be supported at all as it would be out of scope of this project and miss the point of this library - simplicity and portability.

- **Fullscreen / Borderless - Planned**
- **Gamepads - Basic gamepad support planned**
- Rendering surfaces for other graphics APIs such as OpenGL and DirectX - This library is Vulkan only, for other APIs, there's many alternatives.
- Swapchain - Should be in the hands of your application. This can be done by your application. You can retrieve native window pointer.
- Decoration - Out of scope.
- UI rendering - Out of scope. This can be done by your application. You can retrieve native window pointer from the surfer window and use native UI widgets.

## Documentation
As this is a one-man project at the moment, I am focusing my efforts on feature implementation and bug fixes. 
Exposed windows methods are well commented. You can always check the source for reference.

## About this project
In graphics engines and renderers, window management and event handling is painful necessity 
that most of the developers choose to abstract using some sort of library. The biggest players here are GLFW and SDL (and similar),
but these libraries are heavy and require some sort of installation (whether it is a nuget, vcpkg or manual download). 
This harms the portability of a project using the library.
As a researcher and graphics programming enthusiast, I need the window only for the rendering surface and basic input 
handling (so that the window closes when I press Esc, basic camera movement etc.). 
For such a simple task it is completely unnecessary to use such a large libraries. Don't get me wrong, 
SDL2 is great and GLFW is just awesome, but for small projects it is total overkill. 
I was looking if there are some option out there that would be single-header, but I could not find anything suiting my needs. 
In an OpenGL ecosystem, there is plenty of option, but for Vulkan, not so much. 
That is why I decided to create my own. Initially extracted from my renderer, I've built a portable single-header window library specially for Vulkan.
