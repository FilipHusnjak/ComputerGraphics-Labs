cmake_minimum_required(VERSION 3.16)
project(Neon)

set(CMAKE_CXX_STANDARD 17)

include_directories($ENV{VULKAN_SDK})
include_directories($ENV{VULKAN_SDK}/Include)
include_directories(src)
include_directories(src/Core)
include_directories(src/Event)
include_directories(src/Renderer)
include_directories(src/Tools)
include_directories(src/Window)
include_directories(vendor)
include_directories(vendor/allocator)
include_directories(vendor/glfw-3.3.2/include)
include_directories(vendor/glm-0.9.9.7)
include_directories(vendor/imgui)
include_directories(vendor/stb_image)
include_directories(vendor/entt)
include_directories(vendor/assimp)

add_executable(Neon
        src/Core/Allocator.cpp
        src/Core/ImGuiLayer.cpp
        src/Core/Input.cpp
        src/Core/LayerStack.cpp
        src/Renderer/Context.cpp
        src/Renderer/PhysicalDevice.cpp
        src/Renderer/LogicalDevice.cpp
        src/Renderer/DescriptorPool.cpp
        src/Renderer/DescriptorSet.cpp
        src/Renderer/GraphicsPipeline.cpp
        src/Renderer/PerspectiveCamera.cpp
        src/Renderer/PerspectiveCameraController.cpp
        src/Renderer/RenderPass.cpp
        src/Renderer/VulkanRenderer.cpp
        src/Renderer/VulkanShader.cpp
        src/Renderer/SwapChain.cpp
        src/Tools/FileTools.cpp
        src/Window/Window.cpp
        src/Application.cpp
        src/main.cpp
        src/Sandbox3D.cpp
        src/Scene/Scene.cpp
        src/Scene/Animation.cpp
        src/Scene/Components.cpp

        vendor/imgui/examples/imgui_impl_glfw.cpp
        vendor/imgui/examples/imgui_impl_vulkan.cpp
        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_widgets.cpp)

target_link_libraries(Neon
        $ENV{VULKAN_SDK}/lib/vulkan-1.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw-3.3.2/lib-vc2019/glfw3.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp/assimp-vc142-mt.lib)

target_precompile_headers(Neon PUBLIC src/neopch.h)
