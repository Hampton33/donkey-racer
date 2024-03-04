#include "first_app.hpp"
#include "lve_model.hpp"
#include "lve_camera.hpp"
#include "simple_render_system.hpp"
#include "dk_car.hpp"
// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "dk_input.hpp"
// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace lve
{

  FirstApp::FirstApp() { loadGameObjects(); }

  FirstApp::~FirstApp() {}

  void FirstApp::run()
  {

    SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};
    LveCamera camera{};
    DkCar car(lveDevice);
    while (!lveWindow.shouldClose())
    {
      glfwPollEvents(); // - glm::vec3{0.0f, -1.0f, 0.0f},
      handle_input(lveWindow.getWindow(), car);
      // camera.setViewYXZ(glm::vec3{0.0f, -2.0f, 0.0f}, glm::vec3{0.0f, 2.0f, 0.0f});
      camera.setViewDirection(glm::vec3{0.0f, -1.0f, 0.0f}, car.carGameObject.transform.translation); // car.carGameObject.transform.translation - glm::vec3{-2.0f, -5.0f, 0.0f}); // + glm::vec3{-0.5f, -1.0f, 0.f});
      float aspect = lveRenderer.getAspectRatio();
      camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);
      if (auto commandBuffer = lveRenderer.beginFrame())
      {
        lveRenderer.beginSwapChainRenderPass(commandBuffer);

        simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera, car);

        lveRenderer.endSwapChainRenderPass(commandBuffer);
        lveRenderer.endFrame();
      }
    }

    vkDeviceWaitIdle(lveDevice.device());
  }

  void FirstApp::loadGameObjects()
  {

    std::shared_ptr<LveModel> groundModel = LveModel::createModelFromFile(lveDevice, "models/cube.obj");
    auto ground = LveGameObject::createGameObject();
    ground.model = groundModel;                                 // Make sure this model is a simple plane or a very flat cube
    ground.transform.scale = glm::vec3(10.0f, 0.0f, 10.0f);     // Slightly non-zero y scale
    ground.transform.translation = glm::vec3(0.0f, 0.5f, 0.0f); // Adjust y-axis to position the ground
    ground.color = glm::vec3(0.0f, 1.0f, 0.0f);                 // Black ground
    gameObjects.push_back(std::move(ground));

    std::shared_ptr<LveModel> skyModel = LveModel::createModelFromFile(lveDevice, "models/skybox.obj");
    auto sky = LveGameObject::createGameObject();
    sky.model = skyModel;
    sky.transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);
    sky.transform.translation = glm::vec3(0.0f, 0.1f, 0.0f);
    sky.color = glm::vec3(1.0f, 0.0f, 0.0f);
    gameObjects.push_back(std::move(sky));

    std::shared_ptr<LveModel> carModel = LveModel::createModelFromFile(lveDevice, "models/ghetto_car.obj");
    auto car = LveGameObject::createGameObject();
    car.model = carModel;
    // car.transform.scale = glm::vec3(0.5, 0.5, 0.5);
    // car.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
    car.color = glm::vec3(0.0f, 1.0f, 0.0f);

    gameObjects.push_back(std::move(car));
  }

}
