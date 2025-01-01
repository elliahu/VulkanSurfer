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
- Linux
  - **X11 (Supported)**
  - *Wayland (Not supported, planned)*
- Windows
  - Win32 (Not supported, planned, up next)
- MacOs
  - Metal (Not supported)
