#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <array>
#include <glm/glm.hpp>

#include "lve_game_object.hpp"
#include "lve_model.hpp"
#include "lve_camera.hpp"
// g++ src/main.cpp -o client -lws2_32

//

namespace dk
{

    // const float spawnPosition[3] = {0.0f, 0.0f, 0.0f};

    enum C2SMessageType : int32_t // CLIENT TO SERVER (4 BYTES)
    {
        LocalPlayerPosition,
    };

    enum S2CMessageType : int32_t // SERVER TO SERVER (4 BYTES)
    {
        Players,
    };

    // PLAYER STRUCT, SERVER WILL HAVE SAME PLAYER STRUCT
    struct Player
    {
        uint64_t id; // hashed client ip for other users, local can be whatever
        float x;
        float y;
        float z;
        float rotationX;
        float rotationY;
        float rotationZ;
        std::unique_ptr<lve::LveGameObject> gameObject = nullptr;
        Player(uint64_t id, float x, float y, float z, float rotX, float rotY, float rotZ) : id(id), x(x), y(y), z(z), rotationX(rotX), rotationY(rotY), rotationZ(rotZ)
        {
            // Create a temporary LveGameObject and then move it to the heap for unique_ptr to manage.
            auto tempGameObject = lve::LveGameObject::createGameObject();
            gameObject = std::unique_ptr<lve::LveGameObject>(new lve::LveGameObject(std::move(tempGameObject)));

            gameObject->model = nullptr;
            gameObject->transform.scale = glm::vec3(1.2, 1.2, 1.2);
            gameObject->transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
            gameObject->transform.rotation = glm::vec3(0.0, 0.0, 0.0);
            gameObject->color = {.1f, .1f, .1f};
        }
    };

    inline std::mutex sync;             // sync for thread shared access of players
    inline std::vector<Player> players; // local copy of players

    void drawPlayers(VkCommandBuffer commandBuffer, const lve::LveCamera &camera, VkPipelineLayout pipeline, lve::LveDevice &device, lve::LveGameObject &ground);

    class DkClient
    {
    public:
        DkClient();
        ~DkClient(); // Will set shouldTerminate to true
        void run();
        void updatePos(glm::vec3 pos, glm::vec3 rotation);
        std::thread thread; // to check if thread is running or not use .joinable()
        bool isThreadRunning;

    private:
        int tryConnect(); // zero for success,
    };

    // thread
    void communicationLoop(SOCKET sock, bool &isThreadRunning);

}
