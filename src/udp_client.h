
//==============================================================================
//
//  udp_server.h
//
//==============================================================================
//  Copyright (C) 2024 Luminator Tech. Group  <guillaume.plante@luminator.com>
//==============================================================================


#ifndef __LTG_UDP_CLIENT_H__
#define __LTG_UDP_CLIENT_H__

#include "stdafx.h"
#include "udp_client.h"
#include "cthread.h"
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

class LtgUdpClient : public CThread {
public:
    LtgUdpClient();
    ~LtgUdpClient();
    bool init();
    void sendPacket(uint8_t startByte, uint8_t eventId, const std::vector<uint8_t>& data);

protected:
    unsigned long Process(void* parameter);

private:
    struct Packet {
        uint8_t startByte;
        uint8_t eventId;
        uint8_t length;
        uint8_t data[31];
        uint8_t checksum;
    };
    uint8_t currentId;

    SOCKET sockfd;
    WSADATA wsaData;
    sockaddr_in servaddr;

    uint8_t calculateChecksum(const Packet& packet);
    Packet createPacket(uint8_t startByte, uint8_t eventId, const std::vector<uint8_t>& data);
    std::vector<uint8_t> serializePacket(const Packet& packet);
};

#endif // __LTG_UDP_CLIENT_H__
