
#pragma comment(lib, "Ws2_32.lib")
#include "dk_client.hpp"
#include "dk_car.hpp"

bool shouldTerminate{false};

dk::DkClient::DkClient()
{
    Player localPlayer(231, 2.0f, 4.0f, 7.0f, 0.0f, 0.0f, 0.0f);
    players.push_back(std::move(localPlayer));
    return;
}

void dk::DkClient::run()
{
    std::cout << "[CLIENT] CONSTRUCTER CALLED" << std::endl;
    auto xdd = sizeof(Player);
    if (tryConnect() == 1)
    {
        std::cout << "[CLIENT] Failed to connect" << std::endl;
        isThreadRunning = false;
        return;
    }
    else
    {
        std::cout << "[CLIENT] successful connect" << std::endl;
        isThreadRunning = true;
    }
}

dk::DkClient::~DkClient()
{
    std::cout << "[CLIENT] DE-CONSTRUCTER CALLED" << std::endl;

    if (isThreadRunning)
    {
        shouldTerminate = true;

        // thread.join();
        std::cout << "[CLIENT] Thread has been closed" << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return;
}

int dk::DkClient::tryConnect()
{
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in server;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(6969);

    result = inet_pton(AF_INET, "192.168.38.164", &server.sin_addr);
    if (result <= 0)
    {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    result = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Connection failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "[CLIENT] CALLING TO START THREEAD" << std::endl;
    thread = std::thread(communicationLoop, sock, std::ref(isThreadRunning));
    thread.detach();
    return 0;
}

#include <iomanip>
void dk::communicationLoop(SOCKET sock, bool &isThreadRunning) // THREAD
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    std::cout << "[CLIENT] STARTING THREAD" << std::endl;
    int result;
    char recvBuff[512];
    char sendBuff[512];

    uint64_t hashedID;

    bool getLocalHash = true;
    while (getLocalHash)
    {
        result = recv(sock, recvBuff, sizeof(hashedID), 0);
        if (result > 0)
        {
            memcpy(&hashedID, recvBuff, sizeof(hashedID));
            printf("[CLIENT] CLIENT HASH ID: %llu\n", hashedID);
            getLocalHash = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    players[0].id = hashedID;

    printf("[CLIENT] LOCAL PLAYER ID: %llu\n", players[0].id);
    while (true)
    {
        char sendBuff[512];
        char recvBuff[512];
        if (shouldTerminate)

        {
            printf("Should terminate");
            break;
        };

        C2SMessageType sendType = C2SMessageType::LocalPlayerPosition;

        size_t offsetxd = 0;

        {
            std::lock_guard<std::mutex> lock(sync);

            memcpy(sendBuff + offsetxd, &sendType, sizeof(sendType));
            offsetxd += sizeof(sendType);

            memcpy(sendBuff + offsetxd, &players[0].id, sizeof(players[0].id));
            offsetxd += sizeof(players[0].id);

            memcpy(sendBuff + offsetxd, &players[0].x, sizeof(players[0].x));
            offsetxd += sizeof(players[0].x);

            memcpy(sendBuff + offsetxd, &players[0].y, sizeof(players[0].y));
            offsetxd += sizeof(players[0].y);

            memcpy(sendBuff + offsetxd, &players[0].z, sizeof(players[0].z));
            offsetxd += sizeof(players[0].z);

            memcpy(sendBuff + offsetxd, &players[0].rotationX, sizeof(players[0].rotationX));
            offsetxd += sizeof(players[0].rotationX);

            memcpy(sendBuff + offsetxd, &players[0].rotationY, sizeof(players[0].rotationY));
            offsetxd += sizeof(players[0].rotationY);

            memcpy(sendBuff + offsetxd, &players[0].rotationZ, sizeof(players[0].rotationZ));
            offsetxd += sizeof(players[0].rotationZ);
        }

        size_t totalSize = offsetxd;
        result = send(sock, sendBuff, totalSize, 0);
        result = recv(sock, recvBuff, 511, 0);
        if (result > 0)
        {

            int offset = 0;
            int32_t recvType = 0;
            int32_t numOfPlayers = 0;

            memcpy(&recvType, recvBuff + 0, 4);

            memcpy(&numOfPlayers, recvBuff + 4, 4);

            printf("3 [CLIENT] NUMBER CONNECTED OF PLAYERS: %d\n", numOfPlayers);
            offset += 8;
            std::set<uint64_t> receivedPlayerIDs;
            for (int32_t i = 0; i < numOfPlayers && i < 10; i++)
            {
                uint64_t playerID;
                float x, y, z;
                float rotationX, rotationY, rotationZ;
                memcpy(&playerID, recvBuff + offset, 8);
                offset += 8;
                printf("[CLIENT] RECV PLAYER ID: %llu\n", playerID);
                receivedPlayerIDs.insert(playerID);
                memcpy(&x, recvBuff + offset, 4);
                offset += 4;
                printf("[CLIENT] RECV PLAYER X: %f\n", x);

                memcpy(&y, recvBuff + offset, 4);
                offset += 4;
                printf("[CLIENT] RECV PLAYER Y: %f\n", y);

                memcpy(&z, recvBuff + offset, 4);
                offset += 4;
                printf("[CLIENT] RECV PLAYER Z: %f\n", z);

                memcpy(&rotationX, recvBuff + offset, 4);
                offset += 4;
                printf("[CLIENT] RECV PLAYER ROTATION X: %f\n", z);

                memcpy(&rotationY, recvBuff + offset, 4);
                offset += 4;
                printf("[CLIENT] RECV PLAYER ROTATION Y: %f\n", z);

                memcpy(&rotationZ, recvBuff + offset, 4);
                offset += 4;

                printf("\n");

                std::lock_guard<std::mutex> lock(sync);

                auto it = std::find_if(dk::players.begin(), dk::players.end(), [&playerID](const Player &player)
                                       { return player.id == playerID; });

                if (it != dk::players.end())
                {
                    if (it->id != hashedID)
                    {
                        printf("ok");
                        it->x = x;
                        it->y = y;
                        it->z = z;
                        it->rotationX = rotationX;
                        it->rotationY = rotationY;
                        it->rotationZ = rotationZ;
                        // it->gameObject->transform.translation = glm::vec3(x, y, z);
                    }
                }
                else
                {
                    std::cout << "Player added to the list:" << playerID << std::endl;
                    dk::players.push_back(Player(playerID, x, y, z, rotationX, rotationY, rotationZ));
                }
            }
            dk::players.erase(std::remove_if(dk::players.begin(), dk::players.end(),
                                             [&receivedPlayerIDs](const Player &player)
                                             {
                                                 return receivedPlayerIDs.find(player.id) == receivedPlayerIDs.end();
                                             }),
                              dk::players.end());
            printf("4 [CLIENT] SIZE OF PLAYERS: %d\n", players.size());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        else if (result == 0)
        {
            std::cout << "Connection closed" << std::endl;
            break;
        }
        else
        {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // for now testing
    }
    std::cout << "[CLIENT THREAD] Loop has been broken. Closing connection and thread..." << std::endl;
    std::lock_guard<std::mutex> lock(sync);
    isThreadRunning = false;
    players.clear();
    closesocket(sock);
    WSACleanup();
}

void dk::DkClient::updatePos(glm::vec3 pos, glm::vec3 rotation)
{

    std::lock_guard<std::mutex> lock(sync);
    // LOCAL PLAYER WILL ALWAYS BE AT INDEX 0, SO THIS IS FINE (ALWAYS(I THINK))

    players[0].x = pos.x;
    players[0].y = pos.y;
    players[0].z = pos.z;

    players[0].rotationX = rotation.x;
    players[0].rotationY = rotation.y;
    players[0].rotationZ = rotation.z;
}

struct SimplePushConstantData
{
    glm::mat4 transform{1.f};
    alignas(16) glm::vec3 color{};
};

namespace dk
{

    void drawPlayers(VkCommandBuffer commandBuffer, const lve::LveCamera &camera, VkPipelineLayout pipeline, lve::LveDevice &device, lve::LveGameObject &ground)
    {

        std::lock_guard<std::mutex> lock(sync);
        auto viewProj = camera.getProjection() * camera.getView();
        if (players.size() == 1)
        {
            return;
        }
        // Skip first index as this is local player, and is drawn in dk_car, perhaps I ought ot merge the two but I rlly cba wanna finish this asap. Can do later
        for (int i = 1; i < players.size(); i++)
        {

            auto &player = players[i];
            if (player.gameObject->model == nullptr)
            {
                std::cout << "Setting player model for: " << player.id << std::endl;
                player.gameObject->model = ground.model;
            }
            SimplePushConstantData carPush{};
            player.gameObject->transform.translation = glm::vec3(player.x, -1.f, player.z);
            player.gameObject->transform.rotation = glm::vec3(player.rotationX, player.rotationY, player.rotationZ);
            player.gameObject->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
            carPush.transform = viewProj * player.gameObject->transform.mat4();
            carPush.color = glm::vec3(1.0f, 0.0f, 0.0f);

            vkCmdPushConstants(
                commandBuffer,
                pipeline,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &carPush);
            player.gameObject->model->draw(commandBuffer);
        }
    }
}