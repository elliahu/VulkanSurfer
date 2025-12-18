cmake_minimum_required(VERSION 3.22.1)
project(VulkanSurferRoot VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Make the header-only library available
add_library(VulkanSurfer INTERFACE)

target_include_directories(VulkanSurfer INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link Vulkan (provides include directories and libraries)
target_link_libraries(VulkanSurfer INTERFACE Vulkan::Vulkan)

# Platform-specific libraries (but don't define platform macros - header does that)
if(WIN32)
    target_link_libraries(VulkanSurfer INTERFACE Dwmapi)
elseif(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(VulkanSurfer INTERFACE ${X11_LIBRARIES})
endif()