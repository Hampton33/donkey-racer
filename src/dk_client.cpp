
#pragma comment(lib, "Ws2_32.lib")
#include "dk_client.hpp"
#include "dk_car.hpp"

// Server will send list current of players on initial contact

// Client sends local player transform to server, as soon as recived by server, it will send that to all other clients. Server will also send notifcations on
// Player connect and player disconnect

bool shouldTerminate{false};

dk::DkClient::DkClient()
{
    return;
}

void dk::DkClient::run()
{
    std::cout << "[CLIENT] CONSTRUCTER CALLED" << std::endl;
    auto xdd = sizeof(Player);
    printf("%d", xdd);
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
    // Start a new thread to handle communication
    thread = std::thread(communicationLoop, sock);
    thread.detach();
    return 0;
}

void dk::communicationLoop(SOCKET sock) // THREAD
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

    Player localPlayer(hashedID, 42.0f, 72.0f, -12.0f);
    players.push_back(localPlayer);
    while (true)
    {
        if (shouldTerminate)

        {
            printf("Should terminate");
            break;
        };

        C2SMessageType sendType = C2SMessageType::LocalPlayerPosition;

        // WRAPPED IN {} SO IT UNLOCKS INSTANTLY WHEN FINISHED
        {
            std::lock_guard<std::mutex> lock(sync);
            localPlayer.x = players[0].x;
            localPlayer.y = players[0].y;
            localPlayer.z = players[0].z;
        }

        memcpy(sendBuff, &sendType, sizeof(sendType));
        memcpy(sendBuff + sizeof(sendType), &localPlayer, sizeof(localPlayer));
        size_t totalSize = sizeof(sendType) + sizeof(localPlayer);
        result = send(sock, sendBuff, totalSize, 0);

        result = recv(sock, recvBuff, 511, 0);
        if (result > 0)
        {
            int offset = 0;

            // no enum matching yet
            int32_t recvType = 0;

            memcpy(&recvType, recvBuff + offset, sizeof(recvType));
            printf("[CLIENT] RECV TYPE: %d\n", recvType);
            offset += sizeof(recvType);

            int32_t numOfPlayers = 0;
            memcpy(&numOfPlayers, recvBuff + offset, sizeof(numOfPlayers));
            printf("[CLIENT] RECV NUM OF PLAYERS: %d\n", numOfPlayers);
            offset += sizeof(numOfPlayers);

            for (int32_t i = 0; i < numOfPlayers && i < 10; i++)
            {
                uint64_t playerID;
                float x, y, z;

                memcpy(&playerID, recvBuff + offset, sizeof(playerID));
                offset += sizeof(playerID);
                // printf("[CLIENT] RECV PLAYER ID: %llu\n", playerID);

                memcpy(&x, recvBuff + offset, sizeof(x));
                offset += sizeof(x);
                // printf("[CLIENT] RECV PLAYER X: %f\n", x);

                memcpy(&y, recvBuff + offset, sizeof(y));
                offset += sizeof(y);
                // printf("[CLIENT] RECV PLAYER Y: %f\n", y);

                memcpy(&z, recvBuff + offset, sizeof(z));
                offset += sizeof(z);
                // printf("[CLIENT] RECV PLAYER Z: %f\n", z);

                std::lock_guard<std::mutex> lock(sync);

                auto it = std::find_if(dk::players.begin(), dk::players.end(), [&playerID](const Player &player)
                                       { return player.id == playerID; });

                for (auto &player : dk::players)
                {
                    printf("PLAYER XD: %llu %f %f %f\n", player.id, player.x, player.y, player.z);
                }

                // printf("dk::players size: %d\n", dk::players.size());

                if (it != dk::players.end())
                {
                    it->x = x;
                    it->y = y;
                    it->z = z;
                }
                else
                {
                    dk::players.push_back(Player(playerID, x, y, z));
                }
            }
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
    closesocket(sock);
    WSACleanup();
}

void dk::DkClient::updatePos(glm::vec3 pos)
{
    std::lock_guard<std::mutex> lock(sync);
    if (players.size() == 0)
    {
        return;
    }
    // LOCAL PLAYER WILL ALWAYS BE AT INDEX 0, SO THIS IS FINE (ALWAYS(I THINK))

    players[0].x = pos.x;
    players[0].y = pos.y;
    players[0].z = pos.z;
}
