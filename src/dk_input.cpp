#include "dk_input.hpp"
#include <chrono>
void handle_input(GLFWwindow *window, DkCar &car)
{

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
    // if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS)
    //     rotate.y += 1.f;
    // if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS)
    //     rotate.y -= 1.f;
    // if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS)
    //     rotate.x += 1.f;
    // if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS)
    //     rotate.x -= 1.f;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)
    {
        rotate.y += 1.f;
        auto hehe = car.turnAngle + 1.0f;
        car.turnAngle = glm::clamp(hehe, -45.0f, 45.0f); // Correct
    }

    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)
    {
        rotate.y -= 1.f;
        auto hehe = car.turnAngle - 1.0f;
        car.turnAngle = glm::clamp(hehe, -45.0f, 45.0f); // Correct
    }

    if (rotate.y == 0 && car.turnAngle != 0)
    {
        float dampingFactor = 5.0f; // Adjust this value to control the rate of return
        car.turnAngle -= dampingFactor * car.turnAngle * frameTime;
    }

    float yaw = car.carGameObject.transform.rotation.y - glm::half_pi<float>(); // t.transform.rotation.y;
    const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
    // printf("forwardDir: %f %f %f", forwardDir.x, forwardDir.y, forwardDir.z);
    const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
    const glm::vec3 upDir{0.f, -1.f, 0.f};

    glm::vec3 moveDir{0.f};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)
        moveDir += forwardDir;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)
        moveDir -= forwardDir;
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS)
        moveDir += upDir;
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS)
        moveDir -= upDir;

    if (glm::length(moveDir) > 0)
    {
        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        {
            if (glm::dot(moveDir, forwardDir) < 0)
            {
                rotate = -rotate;
            }
            car.carGameObject.transform.rotation += lookSpeed * frameTime * glm::normalize(rotate);
            printf("rotating: %f %f %f \n", car.carGameObject.transform.rotation.x, car.carGameObject.transform.rotation.y, car.carGameObject.transform.rotation.z);
            printf("translatiion: %f %f %f\n", car.carGameObject.transform.translation.x, car.carGameObject.transform.translation.y, car.carGameObject.transform.translation.z);
        }
        if (moveDir.z < 0.f)
        {
            car.isMovingForward = false;
        }
        else
        {
            car.isMovingForward = true;
        }
        car.isMoving = true;
        car.carGameObject.transform.translation += moveSpeed * frameTime * glm::normalize(moveDir);
    }
    else
    {
        car.isMoving = false;
    }
    // car.carGameObject.transform.rotation.z = 0.0f;
    // car.carGameObject.transform.translation.y = 0.0f;
}