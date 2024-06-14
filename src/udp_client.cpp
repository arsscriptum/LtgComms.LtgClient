
//==============================================================================
//
//  udp_server.cpp
//
//==============================================================================
//  Copyright (C) 2024 Luminator Tech. Group  <guillaume.plante@luminator.com>
//==============================================================================

#include "stdafx.h"
#include "udp_client.h"
#include "log.h"

#include <iostream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include <iostream>
#include <cstring>

// Constants
constexpr int PORT = 12345;
constexpr char SERVER_IP[] = "127.0.0.1";
constexpr int BUFFER_SIZE = 256;

// Constructor
LtgUdpClient::LtgUdpClient() : sockfd(INVALID_SOCKET) {
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    currentId = 0;
}

// Destructor
LtgUdpClient::~LtgUdpClient() {
    if (sockfd != INVALID_SOCKET) {
        closesocket(sockfd);
    }
    WSACleanup();
}

// Initialize client
bool LtgUdpClient::init() {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        perror("Socket creation failed");
        return false;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        closesocket(sockfd);
        return false;
    }

    return true;
}


unsigned long LtgUdpClient::Process(void* parameter)
{
    printf("starting client thread\n");
    while (true) {
        Sleep(1000);
        printf("running..\n");
        currentId++;
        std::vector<uint8_t> data = { 0x02, 0x022, currentId }; // Example data
        sendPacket(0xAA, 0x01, data);
    }
}


// Send packet to server
void LtgUdpClient::sendPacket(uint8_t startByte, uint8_t eventId, const std::vector<uint8_t>& data) {
    Packet packet = createPacket(startByte, eventId, data);
    std::vector<uint8_t> buffer = serializePacket(packet);

    if (sendto(sockfd, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0,
        (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        perror("Send failed");
        return;
    }

    std::cout << "Packet sent to server\n";
}

// Calculate checksum
uint8_t LtgUdpClient::calculateChecksum(const Packet& packet) {
    uint8_t checksum = 0;
    checksum ^= packet.startByte;
    checksum ^= packet.eventId;
    checksum ^= packet.length;
    for (int i = 0; i < packet.length; ++i) {
        checksum ^= packet.data[i];
    }
    return checksum;
}

// Create packet
LtgUdpClient::Packet LtgUdpClient::createPacket(uint8_t startByte, uint8_t eventId, const std::vector<uint8_t>& data) {
    Packet packet;
    packet.startByte = startByte;
    packet.eventId = eventId;
    packet.length = data.size();
    std::memcpy(packet.data, data.data(), data.size());
    packet.checksum = calculateChecksum(packet);
    return packet;
}

// Serialize packet
std::vector<uint8_t> LtgUdpClient::serializePacket(const Packet& packet) {
    std::vector<uint8_t> buffer;
    buffer.push_back(packet.startByte);
    buffer.push_back(packet.eventId);
    buffer.push_back(packet.length);
    for (int i = 0; i < packet.length; ++i) {
        buffer.push_back(packet.data[i]);
    }
    buffer.push_back(packet.checksum);
    return buffer;
}
