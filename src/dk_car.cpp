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
    carGameObject.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
    carGameObject.transform.rotation = glm::vec3(0.0, 0.0, 0.0);
    carGameObject.color = {.1f, .1f, .1f};
};

void DkCar::draw(VkCommandBuffer commandBuffer, const lve::LveCamera &camera, VkPipelineLayout pipeline)
{
    auto xd = camera.getProjection() * camera.getView();
    carModel->bind(commandBuffer);
    SimplePushConstantData carPush{};
    carPush.transform = xd * carGameObject.transform.mat4();
    carPush.color = glm::vec3(1.0f, 0.0f, 0.0f);
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
        glm::vec3(-1.5f, 0.1, 1.5f),
        glm::vec3(-1.5f, 0.1, -1.5f),
        glm::vec3(1.5f, 0.1, -1.5f),
        glm::vec3(1.5f, 0.1, 1.5f),

    };

    static auto currentTime = std::chrono::high_resolution_clock::now();

    static float wheelRotationAngle = 0.0f;
    auto wheelSpinTransform = glm::mat4(1.0f);

    if (isMoving)
    {

        // printf("%d", isMovingForward);
        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime =
            std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;
        auto wheelSpinSpeed = 4.0f;

        if (isMovingForward)
        {

            wheelRotationAngle += wheelSpinSpeed * frameTime;
            wheelSpinTransform = glm::rotate(glm::mat4(1.0f), wheelRotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        }
        else
        {
            wheelRotationAngle += -(wheelSpinSpeed * frameTime);
            // printf("%f", wheelRotationAngle);
            wheelSpinTransform = glm::rotate(glm::mat4(1.0f), wheelRotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        }
        wheelRotationAngle = fmod(wheelRotationAngle, 2.0f * 3.14f);
    }

    for (int i = 0; i < 4; i++)
    {
        SimplePushConstantData wheelPush{};

        glm::mat4 wheelRelativeTransform = glm::translate(glm::mat4(1.0f), relOffset[i]);
        auto wheelSteerTransform = glm::mat4(1.0f);
        if (i == 0 || i == 1)
        {
            wheelSteerTransform = glm::rotate(glm::mat4(1.0f), glm::radians(turnAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            wheelPush.color = carGameObject.color;
        }

        auto wheelTransform = carGameObject.transform.mat4() * wheelRelativeTransform * wheelSteerTransform * wheelSpinTransform;

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