#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace lve
{

  struct SimplePushConstantData
  {
    glm::mat4 transform{1.f};
    alignas(16) glm::vec3 color{};
  };

  SimpleRenderSystem::SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass)
      : lveDevice{device}
  {
    createPipelineLayout();
    createPipeline(renderPass);
  }

  SimpleRenderSystem::~SimpleRenderSystem()
  {
    vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
  }

  void SimpleRenderSystem::createPipelineLayout()
  {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS)
    {
      throw std::runtime_error("failed to create pipeline layout!");
    }
  }

  void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
  {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    lvePipeline = std::make_unique<LvePipeline>(
        lveDevice,
        "shaders/vert.spv",
        "shaders/frag.spv",
        pipelineConfig);
  }

  void SimpleRenderSystem::renderGameObjects(
      VkCommandBuffer commandBuffer,
      std::vector<LveGameObject> &gameObjects,
      const LveCamera &camera,
      LveGameObject &viewerObject,
      DkCar &car)
  {
    lvePipeline->bind(commandBuffer);
    LveGameObject &ground = gameObjects[0];
    LveGameObject &skybox = gameObjects[1];
    auto projectionView = camera.getProjection() * camera.getView();

    SimplePushConstantData skyboxPush{};
    skyboxPush.color = skybox.color;
    skybox.transform.translation = glm::vec3(viewerObject.transform.translation.x, 0.0f, viewerObject.transform.translation.z); // Adjust y-axis to position the ground
    skyboxPush.transform = projectionView * skybox.transform.mat4();
    vkCmdPushConstants(
        commandBuffer,
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(SimplePushConstantData),
        &skyboxPush);
    ;
    skybox.model->bind(commandBuffer);
    skybox.model->draw(commandBuffer);

    SimplePushConstantData groundPush{};
    ground.transform.translation = glm::vec3(viewerObject.transform.translation.x, 0.5f, viewerObject.transform.translation.z); // Adjust y-axis to position the ground
    groundPush.color = ground.color;
    groundPush.transform = projectionView * ground.transform.mat4();

    vkCmdPushConstants(
        commandBuffer,
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(SimplePushConstantData),
        &groundPush);

    ground.model->bind(commandBuffer);
    ground.model->draw(commandBuffer);

    LveGameObject &carXD = gameObjects[2];
    SimplePushConstantData carXDPush{};
    // carXD.transform.translation = glm::vec3(viewerObject.transform.translation.x, 0.0f, viewerObject.transform.translation.z); // Adjust y-axis to position the carXD
    carXDPush.color = carXD.color;
    carXD.transform.translation = glm::vec3(1.0f, -1.0f, 1.0f);
    carXDPush.transform = projectionView * carXD.transform.mat4();

    vkCmdPushConstants(
        commandBuffer,
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(SimplePushConstantData),
        &carXDPush);

    carXD.model->bind(commandBuffer);
    carXD.model->draw(commandBuffer);
    car.draw(commandBuffer, camera, pipelineLayout);
    /* SimplePushConstantData carPush{};
    carPush.color = car.color;
    // ground.transform.translation = glm::vec3(viewerObject.transform.translation.x, 0.5f, viewerObject.transform.translation.z); // Adjust y-axis to position the ground
    carPush.transform = projectionView * car.transform.mat4();

    vkCmdPushConstants(
        commandBuffer,
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(SimplePushConstantData),
        &carPush);

    car.model->bind(commandBuffer);
    car.model->draw(commandBuffer);

    SimplePushConstantData wheelsPush{};
    static auto currentTime = std::chrono::high_resolution_clock::now();
    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;
    auto rotate = glm::vec3(0.f);
    rotate.z += 1.5f;
    auto lookSpeed{1.5f};
    wheels.transform.rotation += lookSpeed * frameTime * glm::normalize(rotate);

    wheelsPush.color = wheels.color;
    wheelsPush.transform = projectionView * wheels.transform.mat4();
    vkCmdPushConstants(
        commandBuffer,
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(SimplePushConstantData),
        &wheelsPush);

    wheels.model->bind(commandBuffer);
    wheels.model->draw(commandBuffer); */
  }

} // namespace lve
