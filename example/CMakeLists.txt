cmake_minimum_required(VERSION 3.22.1)
project(VulkanSurferExample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

# Add the library
add_subdirectory(${CMAKE_SOURCE_DIR}/.. ${CMAKE_BINARY_DIR}/VulkanSurfer)

# Add executable
add_executable(VulkanSurferExample main.cpp)

# Link your library (which automatically brings in Vulkan)
target_link_libraries(VulkanSurferExample PRIVATE VulkanSurfer)