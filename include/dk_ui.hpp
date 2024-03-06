// #pragma once
// #include <ImGui/imconfig.h>
// #include <ImGui/imgui_tables.cpp>
// #include <ImGui/imgui_internal.h>
// #include <ImGui/imgui.cpp>
// #include <ImGui/imgui_draw.cpp>
// #include <ImGui/imgui_widgets.cpp>
// #include <ImGui/imgui_demo.cpp>
// #include <ImGui/imgui_impl_glfw.cpp>
// #include <ImGui/imgui_impl_vulkan.h>
// #include "lve_swap_chain.hpp"
// #include "lve_device.hpp"

// namespace dk
// {

//     class DkUi
//     {
//     public:
//         DkUi();
//         ~DkUi();

//     private:
//         void initImGui();
//     };

// }

// dk::DkUi::DkUi()

// {
//     initImGui();
// }

// dk::DkUi::~DkUi()
// {
// }

// void dk::DkUi::initImGui(lve::LveDevice &device)
// {

//     VkDescriptorPoolSize pool_sizes[] =
//         {
//             {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
//             {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
//             {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
//             {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
//             {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
//             {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
//             {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
//             {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
//             {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
//             {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
//             {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

//     VkDescriptorPoolCreateInfo pool_info = {};
//     pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//     pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
//     pool_info.maxSets = 1000;
//     pool_info.poolSizeCount = std::size(pool_sizes);
//     pool_info.pPoolSizes = pool_sizes;

//     VkDescriptorPool imguiPool;
//     vkCreateDescriptorPool(device.device(), &pool_info, nullptr, &imguiPool);
//     ImGui::CreateContext();
//     ImGui_ImplGlfw_InitForVulkan(device.getWindow(), true);
//     // this initializes imgui for Vulkan
//     ImGui_ImplVulkan_InitInfo init_info = {};
//     init_info.Instance = _instance;
//     init_info.PhysicalDevice = _chosenGPU;
//     init_info.Device = _device;
//     init_info.Queue = _graphicsQueue;
//     init_info.DescriptorPool = imguiPool;
//     init_info.MinImageCount = 3;
//     init_info.ImageCount = 3;
//     init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

//     ImGui_ImplVulkan_Init(&init_info, _renderPass);

//     // execute a gpu command to upload imgui font textures
//     immediate_submit([&](VkCommandBuffer cmd)
//                      { ImGui_ImplVulkan_CreateFontsTexture(cmd); });

//     // clear font textures from cpu data
//     ImGui_ImplVulkan_DestroyFontUploadObjects();

//     // add the destroy the imgui created structures
//     _mainDeletionQueue.push_function([=]()
//                                      {

// 		vkDestroyDescriptorPool(_device, imguiPool, nullptr);
// 		ImGui_ImplVulkan_Shutdown(); });
//     ImGui::CreateContext();
//     // ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

//     ImGui_ImplGlfw_InitForVulkan(window, true);

//     ImGui_ImplVulkan_InitInfo info;
//     info.DescriptorPool = descriptorPool;
//     info.RenderPass = device.renderPass();
//     info.Device = device.device();
//     info.PhysicalDevice = device.getPhysicalDevice();
//     info.ImageCount = lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT;
//     info.MsaaSamples = msaaSamples;
//     ImGui_ImplVulkan_Init(&info);

//     VkCommandBuffer commandBuffer = beginSingleTimeCommands();
//     ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
//     endSingleTimeCommands(commandBuffer);

//     vkDeviceWaitIdle(device);
//     ImGui_ImplVulkan_DestroyFontUploadObjects();
// }