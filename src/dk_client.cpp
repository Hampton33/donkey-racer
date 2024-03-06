
#pragma comment(lib, "Ws2_32.lib")
#include "dk_client.hpp"
#include "dk_car.hpp"
// Server will send list current of players on initial contact

// Client sends local player transform to server, as soon as recived by server, it will send that to all other clients. Server will also send notifcations on
// Player connect and player disconnect

bool shouldTerminate{false};

dk::DkClient::DkClient()
{
    if (tryConnect() == 1)
    {
        return;
    }
}

dk::DkClient::~DkClient()
{

    if (thread.joinable())
    {
        shouldTerminate = true;
        thread.join();
        std::cout << "[CLIENT] Thread has been closed" << std::endl;
    }

    return;
}

int dk::DkClient::tryConnect()
{
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in server;

    // Initialize Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(6969);

    // Convert IPv4 address from text to binary form
    result = inet_pton(AF_INET, "192.168.38.164", &server.sin_addr);
    if (result <= 0)
    {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Connect to server
    result = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Connection failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Send message
    const char *sendMessage = "Hello, world!";
    result = send(sock, sendMessage, strlen(sendMessage), 0);
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    // Start a new thread to handle communication
    thread = std::thread(communicationLoop, sock);
    thread.detach();
    return 0;
}

void dk::communicationLoop(SOCKET sock) // THREAD
{
    int result;
    char recvBuff[512];
    char sendBuff[512] = {0};

    while (true)
    {
        if (shouldTerminate)

        {
            break;
        }
        Player localPlayer(69, 0.0f, 0.0f, 0.0f);
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
            int32_t recvType;
            memcpy(&recvType, recvBuff + offset, sizeof(recvType));
            offset += sizeof(recvType);

            int32_t numOfPlayers;
            memcpy(&numOfPlayers, recvBuff + offset, sizeof(numOfPlayers));
            offset += sizeof(numOfPlayers);

            numOfPlayers = std::min(numOfPlayers, 10);
            for (int i = 0; i < numOfPlayers; i++)
            {
                int64_t playerID;
                float x, y, z;

                memcpy(&playerID, recvBuff + offset, sizeof(playerID));
                offset += sizeof(playerID);

                memcpy(&x, recvBuff + offset, sizeof(x));
                offset += sizeof(x);

                memcpy(&y, recvBuff + offset, sizeof(y));
                offset += sizeof(y);

                memcpy(&z, recvBuff + offset, sizeof(z));
                offset += sizeof(z);

                std::lock_guard<std::mutex> lock(sync);

                auto it = std::find_if(dk::players.begin(), dk::players.end(), [&playerID](const Player &player)
                                       { return player.id == playerID; });

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

void dk::DkClient::updatePos(std::array<float, 3> pos)
{
    std::lock_guard<std::mutex> lock(sync);

    // LOCAL PLAYER WILL ALWAYS BE AT INDEX 0, SO THIS IS FINE (ALWAYS(I THINK))
    players[0].x = pos[0];
    players[0].y = pos[1];
    players[0].z = pos[2];
}
