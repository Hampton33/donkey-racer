#include "dk_input.hpp"
#include <chrono>
#include <algorithm>

void handle_input(GLFWwindow *window, DkCar &car)
{

    static glm::vec3 accel = {0.0f, 0.0f, 0.0f};
    static float moveSpeed = 3.0f;      // Base acceleration speed
    static float lookSpeed = 1.5f;      // Turning speed
    static float maxAccelLength = 5.0f; // Maximum acceleration magnitude
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
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        rotate.y += 1.0f;
        car.turnAngle = glm::clamp(car.turnAngle + 1.0f, -45.0f, 45.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        rotate.y -= 1.0f;
        car.turnAngle = glm::clamp(car.turnAngle - 1.0f, -45.0f, 45.0f);
    }
    if (rotate.y == 0 && car.turnAngle != 0)
    {
        float dampingFactor = 5.0f;
        car.turnAngle -= dampingFactor * car.turnAngle * frameTime;
    }

    float yaw = car.carGameObject.transform.rotation.y - glm::half_pi<float>(); // t.transform.rotation.y;
    const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
    // printf("forwardDir: %f %f %f", forwardDir.x, forwardDir.y, forwardDir.z);
    const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
    const glm::vec3 upDir{0.f, -1.f, 0.f};
    float currentSpeed = glm::length(accel);
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
                car.isMovingForward = false;
                rotate = -rotate;
            }
            else
            {
                car.isMovingForward = true;
            }
            car.carGameObject.transform.rotation += lookSpeed * frameTime * glm::normalize(rotate);
        }
        float accelerationFactor = std::min(0.5f + currentSpeed / maxAccelLength, 1.5f);
        accel += moveSpeed * accelerationFactor * frameTime * moveDir;
        car.isMoving = true;
        // printf("accelFactor: %f \n", accelerationFactor);
        // printf("currentSpeed: %f \n", currentSpeed);
        //  printf("maxAccelLength: %f \n", maxAccelLength);
        //   printf("movedir: %f %f %f \n", accel.x, accel.y, accel.z);
        //   printf("glm::length(moveDir): %f \n", glm::length(accel));

        // Check if the acceleration exceeds the maximum length
        if (glm::length(accel) > maxAccelLength)
        {
            // Normalize accel and scale it to the max length
            accel = glm::normalize(accel) * maxAccelLength;
        }
    }
    else
    {
        car.isMoving = false;
        float frictionFactor = 0.96f; // Adjust friction based on speed
        // printf("frictionFactor: %f \n", frictionFactor);
        accel *= frictionFactor;
    }
    // accel = glm::vec3(0.0f);
    car.carGameObject.transform.translation += accel * 0.03f;
}