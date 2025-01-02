# VulkanSurfer

***Important note:*** This project is in early stage of development. Only Linux (X11) and Windows (Win32) is supported at the moment.
Key translation from native keycodes is currently in development. Only basic keys (those on a common keyboard) are supported as of now.
Please se support section to see which platforms are supported and Callback-based event handling section to see how to handle unsupported keys.

A minimal cross-platform object-oriented c++11 header only window library for Vulkan.

This project aims to make creation of a window with a Vulkan surface and basic event handling as simple and portable as possible.
If you just need a window with Vulkan surface and basic event handling, this is the tool you need. 
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

### How to handle unsupported keys
If a key you wish to use is unsupported by Surfer (this can be due to the un-complete mapping or the key is just not defined), you can use `NativeKeyPressCallback` and `NativeKeyReleaseCallback` that when called passes the native keycode when any key is pressed. This way you can add support for missing key by yourself. See docs for your platform.

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
In graphics engines and renderers, window management and event handling is often a necessary but tedious task. Most developers choose to abstract this functionality using libraries like GLFW, SDL, or similar tools. While these libraries are robust and feature-rich, they tend to be heavyweight and require installation via NuGet, vcpkg, or manual downloads. This can impact the portability of projects that rely on them.

As a researcher and graphics programming enthusiast, my needs are simpler. I primarily need a window for the rendering surface and basic input handling—just enough to close the window with the Esc key or enable basic camera movement. For such straightforward requirements, using a large library often feels like overkill. Don’t get me wrong—SDL2 is excellent, and GLFW is truly amazing. But for smaller projects, their size and complexity can be unnecessary.

When I looked for lightweight, single-header alternatives, I was surprised to find a lack of options tailored to Vulkan. The OpenGL ecosystem has plenty of solutions, but Vulkan seems to have been overlooked in this area. That’s why I decided to create my own solution.

What started as a utility extracted from my renderer has evolved into a portable, single-header window library designed specifically for Vulkan. It’s simple, lightweight, and focuses on exactly what’s needed—nothing more, nothing less.
