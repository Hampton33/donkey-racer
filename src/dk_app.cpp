#include "dk_app.hpp"
#include "lve_model.hpp"
#include "lve_camera.hpp"
#include "dk_render_system.hpp"
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

  DkApp::DkApp() { loadGameObjects(); }

  DkApp::~DkApp() {}

  void DkApp::run()
  {
    client.run();
    glm::vec3 cameraOffset = glm::vec3{3.0f, -3.0f, 3.0f};
    SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};
    LveCamera camera{};
    DkCar car(lveDevice);
    int frame = 0;
    while (!lveWindow.shouldClose())
    {
      frame++;
      if (frame % 10 == 0 && client.isThreadRunning)
      {
        frame = 0;
        client.updatePos(car.carGameObject.transform.translation, car.carGameObject.transform.rotation);
      }
      glm::vec3 cameraPosition = car.carGameObject.transform.translation + cameraOffset;

      glfwPollEvents();
      handle_input(lveWindow.getWindow(), car);
      camera.setViewTarget(cameraPosition, car.carGameObject.transform.translation, glm::vec3{0.f, -1.f, 0.f});

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

  void DkApp::loadGameObjects()
  {

    std::shared_ptr<LveModel> groundModel = LveModel::createModelFromFile(lveDevice, "models/cube.obj");
    auto ground = LveGameObject::createGameObject();
    ground.model = groundModel;
    ground.transform.scale = glm::vec3(10.0f, 0.0f, 10.0f);
    ground.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
    ground.color = glm::vec3(0.0f, 1.0f, 0.0f);
    gameObjects.push_back(std::move(ground));

    std::shared_ptr<LveModel> skyModel = LveModel::createModelFromFile(lveDevice, "models/skybox.obj");
    auto sky = LveGameObject::createGameObject();
    sky.model = skyModel;
    sky.transform.scale = glm::vec3(10.0f, 5.0f, 10.0f);
    sky.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
    sky.color = glm::vec3(1.0f, 0.0f, 0.0f);
    gameObjects.push_back(std::move(sky));

    std::shared_ptr<LveModel> carModel = LveModel::createModelFromFile(lveDevice, "models/ghetto_car.obj");

    carcarModel = carModel;
    auto car = LveGameObject::createGameObject();
    car.model = carModel;
    // car.transform.scale = glm::vec3(0.5, 0.5, 0.5);
    // car.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
    car.color = glm::vec3(0.0f, 1.0f, 0.0f);

    gameObjects.push_back(std::move(car));
  }

}
