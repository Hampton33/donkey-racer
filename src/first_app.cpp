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
    // camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
    viewerObject.transform.translation = {2.0f, -2.f, 0.0f};
    viewerObject.transform.rotation = {-0.5f, -1.57f, 0.f};
    // camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));

    while (!lveWindow.shouldClose())
    {
      glfwPollEvents();
      handle_input(lveWindow.getWindow(), viewerObject, car);
      camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
      float aspect = lveRenderer.getAspectRatio();
      // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
      camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
      car.carGameObject.transform.translation = viewerObject.transform.translation - glm::vec3{2.f, -2.f, 0.f};
      if (auto commandBuffer = lveRenderer.beginFrame())
      {
        lveRenderer.beginSwapChainRenderPass(commandBuffer);

        simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera, viewerObject, car);

        lveRenderer.endSwapChainRenderPass(commandBuffer);
        lveRenderer.endFrame();
      }
    }

    vkDeviceWaitIdle(lveDevice.device());
  }

  void FirstApp::loadGameObjects()
  {
    // std::shared_ptr<LveModel> lveModel = createCubeModel(lveDevice, {.0f, .0f, .0f});
    // auto cube = LveGameObject::createGameObject();
    // cube.model = lveModel;
    // cube.transform.translation = {.0f, .0f, 2.5f};
    // cube.transform.scale = {.5f, .5f, .5f};
    // gameObjects.push_back(std::move(cube));

    // std::shared_ptr<LveModel> lveModel2 =
    //     LveModel::createModelFromFile(lveDevice, "models/colored_cube.obj");
    // auto cube = LveGameObject::createGameObject();
    // cube.model = lveModel2;
    // cube.transform.translation = {.0f, .0f, 2.5f};
    // cube.transform.scale = glm::vec3(1.f);
    // gameObjects.push_back(std::move(cube));

    std::shared_ptr<LveModel> wallModel =
        LveModel::createModelFromFile(lveDevice, "models/colored_cube.obj");

    // Positions of the four walls in a room
    // std::vector<glm::vec3> wallPositions = {
    //     glm::vec3(0.0f, wallHeight / 2.0f, wallWidth / 2.0f),  // Front wall
    //     glm::vec3(wallWidth / 2.0f, wallHeight / 2.0f, 0.0f),  // Right wall
    //     glm::vec3(0.0f, wallHeight / 2.0f, -wallWidth / 2.0f), // Back wall
    //     glm::vec3(-wallWidth / 2.0f, wallHeight / 2.0f, 0.0f)  // Left wall
    // };std::shared_ptr<LveModel> wallModel =
    std::shared_ptr<LveModel> xdModel = LveModel::createModelFromFile(lveDevice, "models/cube.obj");

    // // Rotations to apply to each wall
    // std::vector<glm::vec3> wallRotations = {
    //     glm::vec3(0.0f, 0.0f, 0.0f),                 // Front wall requires no rotation
    //     glm::vec3(0.0f, glm::radians(90.0f), 0.0f),  // Right wall
    //     glm::vec3(0.0f, glm::radians(180.0f), 0.0f), // Back wall
    //     glm::vec3(0.0f, glm::radians(-90.0f), 0.0f)  // Left wall
    // };

    // // Create and position the four walls
    // for (int i = 0; i < 4; ++i)
    // {
    //   auto wall = LveGameObject::createGameObject();
    //   wall.model = wallModel;
    //   wall.transform.translation = wallPositions[i];
    //   wall.transform.rotation = wallRotations[i];
    //   wall.transform.scale = glm::vec3(wallWidth, wallHeight, wallThickness);
    //   gameObjects.push_back(std::move(wall));
    // }

    auto ground = LveGameObject::createGameObject();
    ground.model = xdModel;                                     // Make sure this model is a simple plane or a very flat cube
    ground.transform.scale = glm::vec3(10.0f, 0.0f, 10.0f);     // Slightly non-zero y scale
    ground.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f); // Adjust y-axis to position the ground
    ground.color = glm::vec3(0.5f, 0.5f, 0.5f);                 // Black ground

    gameObjects.push_back(std::move(ground));

    auto sky = LveGameObject::createGameObject();
    sky.model = wallModel;
    sky.transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);
    sky.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
    sky.color = glm::vec3(1.0f, 1.0f, 1.0f);
    // sky.transform.rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f)
    gameObjects.push_back(std::move(sky));

    // std::shared_ptr<LveModel> carM = LveModel::createModelFromFile(lveDevice, "models/ghetto_car.obj");
    // auto car = LveGameObject::createGameObject();
    // car.model = carM;
    // car.transform.scale = glm::vec3(0.2, 0.2, 0.2);
    // car.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
    //// test.color = glm::vec3(1.0f, 1.0f, 1.0f);
    ////  sky.transform.rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f)
    // gameObjects.push_back(std::move(car));
    //
    // std::shared_ptr<LveModel> wheelsM = LveModel::createModelFromFile(lveDevice, "models/cube.obj");
    // auto wheels = LveGameObject::createGameObject();
    // wheels.model = wheelsM;
    //;
    // wheels.transform.scale = glm::vec3(0.1, 0.1, 0.1);
    //
    // wheels.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
    //
    // test.color = glm::vec3(1.0f, 1.0f, 1.0f);
    //  sky.transform.rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f)
    // gameObjects.push_back(std::move(wheels));
  }

} // namespace lve
