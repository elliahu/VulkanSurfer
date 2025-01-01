cmake_minimum_required(VERSION 3.29)
project(VulkanSurfer)

set(CMAKE_CXX_STANDARD 11)

add_executable(VulkanSurfer example/main.cpp
        VulkanSurfer.h)

target_include_directories(VulkanSurfer PRIVATE ${CMAKE_SOURCE_DIR})

if(WIN32)
    target_compile_definitions(VulkanSurfer PRIVATE VK_USE_PLATFORM_WIN32_KHR)
    target_link_libraries(VulkanSurfer PRIVATE Dwmapi)

elseif(UNIX AND NOT APPLE)
    target_compile_definitions(VulkanSurfer PRIVATE VK_USE_PLATFORM_XLIB_KHR)

    # Find and link X11 libraries
    find_package(X11 REQUIRED)
    target_include_directories(VulkanSurfer PRIVATE ${X11_INCLUDE_DIR})
    target_link_libraries(VulkanSurfer PRIVATE ${X11_LIBRARIES})
    target_link_libraries(VulkanSurfer PRIVATE ${X11_LIBRARIES} X11::Xkb)

elseif(APPLE)
    target_compile_definitions(VulkanSurfer PRIVATE VK_USE_PLATFORM_METAL_EXT)
endif()

# Link dependencies
find_package(Vulkan REQUIRED )
target_link_libraries(VulkanSurfer PUBLIC Vulkan::Vulkan)
