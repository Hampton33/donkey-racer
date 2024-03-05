#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>

// g++ src/main.cpp -o client -lws2_32

//
int connectToServer();
void handleCommunication(SOCKET sock);
void constructPlayerTransformMessage(char sendBuff[512], int &messageSize);