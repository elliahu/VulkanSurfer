cmake_minimum_required(VERSION 3.30)
project(VulkanSurfer)

set(CMAKE_CXX_STANDARD 11)

add_executable(VulkanSurfer example/main.cpp
        VulkanSurfer.h)

target_include_directories(VulkanSurfer PRIVATE ${CMAKE_SOURCE_DIR})

# Link dependencies
find_package(Vulkan REQUIRED )
target_link_libraries(VulkanSurfer PUBLIC Vulkan::Vulkan)
