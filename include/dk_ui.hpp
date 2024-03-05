#pragma once
#include <ImGui/imconfig.h>
#include <ImGui/imgui_tables.cpp>
#include <ImGui/imgui_internal.h>
#include <ImGui/imgui.cpp>
#include <ImGui/imgui_draw.cpp>
#include <ImGui/imgui_widgets.cpp>
#include <ImGui/imgui_demo.cpp>
#include <ImGui/imgui_impl_glfw.cpp>
#include <ImGui/imgui_impl_vulkan_but_better.h>

namespace dk
{

    class DkUi
    {
    public:
        DkUi();
        ~DkUi();

    private:
        initImGui();
    };

}

Dk::DkUi::DkUi()
{
}

Dk::DkUi::~DkUi()
{
}

Dk::DkUi::initImGui()
{
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForVulkan(window, true);

    ImGui_ImplVulkan_InitInfo info;
    info.DescriptorPool = descriptorPool;
    info.RenderPass = renderPass;
    info.Device = device;
    info.PhysicalDevice = physicalDevice;
    info.ImageCount = MAX_FRAMES_IN_FLIGHT;
    info.MsaaSamples = msaaSamples;
    ImGui_ImplVulkan_Init(&info);

    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    endSingleTimeCommands(commandBuffer);

    vkDeviceWaitIdle(device);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}