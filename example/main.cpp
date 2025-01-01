#include <iostream>
#include <memory>
#define SURFER_PLATFORM_X11
#include "VulkanSurfer.h"

int main() {

    // Create vulkan instance
    // Your code probably already does this
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;  // Choose Vulkan API version

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;


    const char* extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(SURFER_PLATFORM_WIN32)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined(SURFER_PLATFORM_X11)
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#endif
    };
    createInfo.enabledExtensionCount = sizeof(extensions) / sizeof(extensions[0]);
    createInfo.ppEnabledExtensionNames = extensions;

    const char* validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    createInfo.enabledLayerCount = sizeof(validationLayers) / sizeof(validationLayers[0]);
    createInfo.ppEnabledLayerNames = validationLayers;

    VkInstance instance;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance!" << std::endl;
        return -1;
    }

    // We need a vulkan instance to create a Surfer::Window instance
    Surfer::Window * window = Surfer::Window::createWindow("Example window", instance, 800, 600 , 100, 100 );

    // Now we can ga a surface to draw onto
    VkSurfaceKHR surface = window->getSurface();

    // Register callbacks
    window->registerKeyPressCallback([](Surfer::KeyCode key) {
        std::cout << "Key pressed " << static_cast<unsigned int>(key) << std::endl;
    });

    window->registerKeyReleaseCallback([](Surfer::KeyCode key) {
        std::cout << "Key released "  << static_cast<unsigned int>(key) << std::endl;
    });

    window->registerMouseMotionCallback([](unsigned int x, unsigned int y) {
        std::cout << "Mouse moved in window: " << x << " x " << y << std::endl;
    });

    window->registerResizeCallback([](unsigned int width, unsigned int height) {
        std::cout << "Window resized: " << width << " x " << height << std::endl;
    });

    window->registerMoveCallback([](int x, int y) {
        std::cout << "Window moved: " << x << " x " << y << std::endl;
    });

    window->registerCloseCallback([]() {
        // you can call destroyWindow here
        std::cout << "Closing..." << std::endl;
    });

    window->registerMouseEnterExitCallback([](bool enter) {
        if (enter) {std::cout << "Mouse entered." << std::endl;}
        else {std::cout << "Mouse exited." << std::endl;}
    });

    window->registerFocusCallback([](bool focused) {
        if (focused) {std::cout << "Window is in focus." << std::endl;}
        else {std::cout << "Window is out of focus." << std::endl;}
    });


    // Window loop
    while (!window->shouldClose()) {
        // poll for events
        window->pollEvents();


    }

    // Don't forget to destroy the window
    Surfer::Window::destroyWindow(window);

    // cleanup of Vulkan resource
    vkDestroyInstance(instance, nullptr);
}