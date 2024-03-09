#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "dk_client.hpp"
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
      DkCar &car)
  {
    lvePipeline->bind(commandBuffer);
    LveGameObject &ground = gameObjects[0];
    LveGameObject &skybox = gameObjects[1];
    auto projectionView = camera.getProjection() * camera.getView();

    SimplePushConstantData skyboxPush{};

    skybox.transform.translation = car.carGameObject.transform.translation;
    skyboxPush.transform = projectionView * skybox.transform.mat4();
    vkCmdPushConstants(
        commandBuffer,
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(SimplePushConstantData),
        &skyboxPush);
    ;
    // skybox.transform.rotation
    skybox.model->bind(commandBuffer);
    skybox.model->draw(commandBuffer);

    SimplePushConstantData groundPush{};
    ground.transform.translation = car.carGameObject.transform.translation;
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

    // LveGameObject &carXD = gameObjects[2];
    // SimplePushConstantData carXDPush{};

    // carXDPush.color = carXD.color;
    // carXD.transform.translation = glm::vec3(1.0f, -1.0f, 1.0f);
    // carXDPush.transform = projectionView * carXD.transform.mat4();

    // vkCmdPushConstants(
    //     commandBuffer,
    //     pipelineLayout,
    //     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
    //     0,
    //     sizeof(SimplePushConstantData),
    //     &carXDPush);

    // carXD.model->bind(commandBuffer);
    // carXD.model->draw(commandBuffer);
    car.draw(commandBuffer, camera, pipelineLayout);
    dk::drawPlayers(commandBuffer, camera, pipelineLayout, lveDevice);
  }

  void SimpleRenderSystem::testRenderObj()
  {
  }

}
