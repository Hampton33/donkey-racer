#pragma once

#include "lve_camera.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"
#include "dk_car.hpp"

// std
#include <memory>
#include <vector>

namespace lve
{
  class SimpleRenderSystem
  {
  public:
    SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

    void renderGameObjects(
        VkCommandBuffer commandBuffer,
        std::vector<LveGameObject> &gameObjects,
        const LveCamera &camera,
        DkCar &car);

  private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

    void testRenderObj(VkCommandBuffer commandBuffer, const lve::LveCamera &camera, VkPipelineLayout pipeline, LveGameObject &ground);
    LveDevice &lveDevice;

    std::unique_ptr<LvePipeline> lvePipeline;
    VkPipelineLayout pipelineLayout;
  };
} // namespace lve
