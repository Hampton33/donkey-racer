//
#pragma comment(lib, "Ws2_32.lib")
#include "dk_client.hpp"
#include "dk_car.hpp"
// Server will send list current of players on initial contact

// Client sends local player transform to server, as soon as recived by server, it will send that to all other clients. Server will also send notifcations on
// Player connect and player disconnect

int connectToServer()
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
    std::thread communicationThread(handleCommunication, sock);
    communicationThread.detach(); // Optionally, detach the thread if you don't need to join it later

    return 0; // Return control to the main program
}

void handleCommunication(SOCKET sock)
{
    int result;
    char recvbuf[512];
    char sendBuff[512] = {0}; // Initializes the entire buffer to zero

    // Loop to keep the connection alive and handle incoming data
    result = recv(sock, recvbuf, 511, 0);
    if (result > 0)
    {
        recvbuf[result] = '\0'; // null-terminate
        std::cout << "Received: " << recvbuf << std::endl;
        // Handle sending data similarly with send()
    }
    else if (result == 0)
    {
        std::cout << "Connection closed" << std::endl;
        return; // Exit loop if connection is closed
    }
    else
    {
        std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
        return; // Exit loop on error
    }

    while (true)
    {

        int messageSize = 0;
        // constructPlayerTransformMessage(sendBuff, messageSize);
        float xdd = 120.0f;
        float xdd2 = 150.0f;

        glm::vec3 vec2 = {xdd, xdd2, 130.0f};
        // Correctly serialize the float into the send buffer
        memcpy(sendBuff, &vec2, sizeof(vec2));

        // Correctly serialize the second float into the send buffer, immediately following the first
        // memcpy(sendBuff + sizeof(xdd), &xdd2, sizeof(xdd2));
        // Now send the buffer over the socket
        result = send(sock, sendBuff, sizeof(vec2), 0);
        std::cout << "Sent: " << sendBuff << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));
        result = recv(sock, recvbuf, 511, 0);
        if (result > 0)
        {
            unsigned char typeOfEnum = recvbuf[0];
            unsigned char numOfPlayers = recvbuf[1];

            // Assuming playerID starts at index 2 and is 8 bytes long
            int64_t playerID;
            memcpy(&playerID, recvbuf + 2, sizeof(playerID));

            // Assuming x, y, z floats follow immediately after playerID
            float x, y, z;
            memcpy(&x, recvbuf + 10, sizeof(x)); // x starts at index 10 (after typeOfEnum + numOfPlayers + playerID)
            memcpy(&y, recvbuf + 14, sizeof(y)); // y follows x
            memcpy(&z, recvbuf + 18, sizeof(z)); // z follows y

            std::cout << "Received:\n"
                      << "Type of Enum: " << static_cast<int>(typeOfEnum) << "\n"
                      << "Number of Players: " << static_cast<int>(numOfPlayers) << "\n"
                      << "Player ID: " << playerID << "\n"
                      << "Position - X: " << x << ", Y: " << y << ", Z: " << z << std::endl;
        }
        else if (result == 0)
        {
            std::cout << "Connection closed" << std::endl;
            return; // Exit loop if connection is closed
        }
        else
        {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            return; // Exit loop on error
        }
    }
    std::cout << "Server connection closed. Exiting communication thread..." << std::endl;
    // Cleanup
    closesocket(sock);
    WSACleanup();
}

void constructPlayerTransformMessage(char sendBuff[512], int &messageSize)
{
    char enumNumber = 0;
    memcpy(sendBuff, &enumNumber, sizeof(enumNumber));

    // Define the player's transform.
    const float playerTransform[3] = {1.2, 1.2, 0.0};
    std::cout << "SIZE" << sizeof(playerTransform) << std::endl;
    // Copy the playerTransform data into the sendBuff, after the enum number.
    memcpy(sendBuff + sizeof(enumNumber), &playerTransform, sizeof(playerTransform));

    // Calculate the total size of the message.
    messageSize = sizeof(enumNumber) + sizeof(playerTransform);
}
