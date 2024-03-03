#include "dk_car.hpp"
#include "lve_pipeline.hpp"
struct SimplePushConstantData
{
    glm::mat4 transform{1.f};
    alignas(16) glm::vec3 color{};
};
DkCar::DkCar(lve::LveDevice &device) : device{device}
{
    carModel =
        lve::LveModel::createModelFromFile(device, carModelPath);

    wheelModel =
        lve::LveModel::createModelFromFile(device, wheelModelPath);

    carGameObject.model = carModel;
    carGameObject.transform.scale = glm::vec3(0.2, -0.2, 0.2);
    carGameObject.transform.translation = glm::vec3(0.0f, -0.2f, 0.0f);
    carGameObject.transform.rotation = glm::vec3(0.0);
};
// DkCar::~DkCar();

void DkCar::draw(VkCommandBuffer commandBuffer, const lve::LveCamera &camera, VkPipelineLayout pipeline)
{

    carModel->bind(commandBuffer);
    SimplePushConstantData carPush{};
    auto carTransform = (camera.getProjection() * camera.getView()) * carGameObject.transform.mat4();
    carPush.transform = carTransform;
    vkCmdPushConstants(
        commandBuffer,
        pipeline,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(SimplePushConstantData),
        &carPush);
    carModel->draw(commandBuffer);
    wheelModel->bind(commandBuffer);
    std::vector<glm::vec3> relOffset = {
        glm::vec3(1.5f, 0.1, 1.5f),   // Front Right
        glm::vec3(-1.5f, 0.1, 1.5f),  // Front Left
        glm::vec3(1.5f, 0.1, -1.5f),  // Back Right
        glm::vec3(-1.5f, 0.1, -1.5f), // Back Left
    };
    // glm::mat4 smallerScale = carGameObject.transform.m
    static float wheelRotationAngle = 0.0f;
    static auto currentTime = std::chrono::high_resolution_clock::now();
    float turnAngleRadians = glm::radians(turnAngle);
    if (isMoving)
    {
        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime =
            std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;
        if (isMovingForward)
        {
        }
        auto wheelSpinSpeed = 4.0f;                       // Define how fast the wheels spin
        wheelRotationAngle += wheelSpinSpeed * frameTime; // Increase the wheel's rotation angle based on time and speed
    }
    for (int i = 0; i < 4; i++)
    {
        glm::mat4 wheelRelativeTransform = glm::translate(glm::mat4(1.0f), relOffset[i]);

        // Apply rotation to the wheel for spinning
        glm::mat4 wheelSpinRotation = glm::rotate(glm::mat4(1.0f), wheelRotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));

        // Apply steering rotation to the front wheels
        glm::mat4 wheelSteerRotation = glm::mat4(1.0f); // Identity matrix for rear wheels
        if (i == 1 || i == 3)
        { // Assuming indices 0 and 1 are the front wheels
            wheelSteerRotation = glm::rotate(glm::mat4(1.0f), turnAngleRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        auto wheelTransform = carGameObject.transform.mat4() * wheelRelativeTransform * wheelSpinRotation * wheelSteerRotation;

        SimplePushConstantData wheelPush{};
        wheelPush.transform = (camera.getProjection() * camera.getView()) * wheelTransform;
        vkCmdPushConstants(
            commandBuffer,
            pipeline,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &wheelPush);
        wheelModel->draw(commandBuffer);
    }
}