#pragma once
#include "lve_model.hpp"
#include "lve_game_object.hpp"
#include "lve_camera.hpp"
// Player Car Object
const std::string carModelPath = "models/ghetto_car.obj";
const std::string wheelModelPath = "models/ghetto_car_wheels.obj";

class DkCar
{
public:
    DkCar(lve::LveDevice &device);
    //~DkCar();

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer, const lve::LveCamera &camera, VkPipelineLayout pipeline);
    lve::LveGameObject carGameObject = {lve::LveGameObject::createGameObject()};
    bool isMoving = true;
    bool isMovingForward = false;
    float turnAngle = 0.0f;
    glm::vec3 velocity = {0.0f, 0.0f, 0.0f};

private:
    std::shared_ptr<lve::LveModel> carModel;
    std::shared_ptr<lve::LveModel> wheelModel;
    lve::LveDevice &device;
};