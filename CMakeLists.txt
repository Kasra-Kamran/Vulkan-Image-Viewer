Project(VulkanImageViewer)

cmake_minimum_required(VERSION 3.10)

add_compile_options(-g)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(Vulkan_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/Vulkan-Headers/include)
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
qt_standard_project_setup()


add_subdirectory(third_party)

qt_add_executable(main
    src/main.cpp src/window_qt.cpp
)

target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/third_party/Vulkan-Headers/include)
target_link_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/third_party/Vulkan-Loader/loader)
target_link_libraries(main PUBLIC vulkan Qt6::Widgets)

