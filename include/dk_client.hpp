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

        Player(uint64_t id, float x, float y, float z) : id(id), x(x), y(y), z(z) {}
    };

    inline std::mutex sync;             // sync for thread shared access of players
    inline std::vector<Player> players; // local copy of players

    class DkClient
    {
    public:
        DkClient();
        ~DkClient(); // Will set shouldTerminate to true
        void run();
        void updatePos(glm::vec3 pos);
        std::thread thread; // to check if thread is running or not use .joinable()
        bool isThreadRunning;

    private:
        int tryConnect(); // zero for success,
    };

    // thread
    void communicationLoop(SOCKET sock);

}
