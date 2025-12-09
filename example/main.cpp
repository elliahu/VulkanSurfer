/**
* Copyright(C) by Matěj Eliáš - matejelias.cz
 *
 * This code is licensed under the MIT license(MIT) (http://opensource.org/licenses/MIT)
 */


#include "VulkanSurfer.h" // Includes <vulkan/vulkan.h> inside
#include <iostream>

// Do not forget to define your platform
// Here we do it autmatically
#ifdef _WIN32
#define SURFER_PLATFORM_WIN32 // On windows define this
#endif


#ifdef __linux__
#define SURFER_PLATFORM_X11 // On linux define this
#endif


// Create a vulkan instance, your code probably already does that
VkResult createVulkanInstance(VkInstance *instance) {
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

    return vkCreateInstance(&createInfo, nullptr, instance);
}

int main() {

    // Create Vulkan instance (your probably already does that)
    VkInstance instance = VK_NULL_HANDLE;
    if (createVulkanInstance(&instance) != VK_SUCCESS) {
        std::cout << "Failed to create Vulkan instance" << std::endl;
        return EXIT_FAILURE;
    }

    // Create window
    auto window = Surfer::Window::createWindow("Example window", 800, 600 , 100, 100 );

    // Now we can create a Vulkan surface
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (window->createSurface(instance, &surface) != VK_SUCCESS) {
        std::cout << "Failed to create Vulkan surface" << std::endl;
        return EXIT_FAILURE;
    }

    // Register callbacks

    // Key is presses
    window->registerKeyPressCallback([](Surfer::KeyCode key) {
        std::cout << "Key pressed " << static_cast<unsigned int>(key) << std::endl;
    });

    // Key is releases
    window->registerKeyReleaseCallback([](Surfer::KeyCode key) {
        std::cout << "Key released "  << static_cast<unsigned int>(key) << std::endl;
    });

    // For your ui
    window->registerMouseMotionCallback([](unsigned int x, unsigned int y) {
        std::cout << "Mouse moved in window: " << x << " x " << y << std::endl;
    });

    // Hre you would typically recreate swapchain and dependent resources
    window->registerResizeCallback([](unsigned int width, unsigned int height) {
        std::cout << "Window resized: " << width << " x " << height << std::endl;
    });

    // This is called when window is moved
    window->registerMoveCallback([](int x, int y) {
        std::cout << "Window moved: " << x << " x " << y << std::endl;
    });

    // Here you would typically release your resources
    window->registerCloseCallback([]() {
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

    // You can use these methods to add support any key as long as your os detects it!

#ifdef SURFER_PLATFORM_X11
    window->registerNativeKeyPressCallback([](KeySym sym) {
        std::cout << "Native key: " << sym << std::endl;
    });
#endif

#ifdef SURFER_PLATFORM_WIN32
    window->registerNativeKeyPressCallback([](WPARAM sym) {
        std::cout << "Native key: " << sym << std::endl;
    });
#endif

    // same for native key release


    // Window loop
    while (!window->shouldClose()) {
        // Process OS events
        window->pollEvents();

        // Query state after events processed
        unsigned int x,y;
        window->getCursorPosition(x,y);

        // Draw
        // Your drawing here
    }

    // Don't forget to destroy the window
    Surfer::Window::destroyWindow(window);

    // cleanup of Vulkan resource
    vkDestroySurfaceKHR(instance,surface,nullptr);
    vkDestroyInstance(instance, nullptr);

    return EXIT_SUCCESS;
}