#include "dk_input.hpp"
#include <chrono>
void handle_input(GLFWwindow *window, lve::LveGameObject &gameObject, DkCar &car)
{

    auto isXD = false;
    static float moveSpeed{3.f};
    static float lookSpeed{1.5f};
    struct KeyMappings keys
    {
    };
    static auto currentTime = std::chrono::high_resolution_clock::now();
    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;
    glm::vec3 rotate{0};
    if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS)
        rotate.y += 1.f;
    if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS)
        rotate.y -= 1.f;
    if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS)
        rotate.x += 1.f;
    if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS)
        rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
    {
        gameObject.transform.rotation += lookSpeed * frameTime * glm::normalize(rotate);
    }

    // limit pitch values between about +/- 85ish degrees
    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = gameObject.transform.rotation.y;
    const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
    const glm::vec3 upDir{0.f, -1.f, 0.f};

    glm::vec3 moveDir{0.f};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)
        moveDir += forwardDir;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)
        moveDir -= forwardDir;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)
    {

        auto hehe = car.turnAngle + 1.0f;
        car.turnAngle = glm::clamp(hehe, -45.0f, 45.0f); // Correct
    }

    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)
    {

        auto hehe = car.turnAngle - 1.0f;
        car.turnAngle = glm::clamp(hehe, -45.0f, 45.0f); // Correct
    }

    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS)
        moveDir += upDir;
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS)
        moveDir -= upDir;

    // Apply movement if there is a direction to move in
    if (glm::length(moveDir) > 0)
    {
        if (moveDir.z < 0.f)
        {
            car.isMovingForward = false;
        }
        else
        {
            car.isMovingForward = true;
        }
        printf("Move Dir: %f %f %f", moveDir.x, moveDir.y, moveDir.z);
        car.isMoving = true;
        gameObject.transform.translation += moveSpeed * frameTime * glm::normalize(moveDir);
    }
    else
    {
        car.isMoving = false;
    }
}