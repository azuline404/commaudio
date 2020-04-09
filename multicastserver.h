#ifndef MULTICASTSERVER_H
#define MULTICASTSERVER_H
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "SocketInformation.h"

#define MAXADDRSTR  16

#define TIMECAST_ADDR   "234.5.6.7"
#define TIMECAST_PORT   8910
#define TIMECAST_TTL    2
#define TIMECAST_INTRVL 30


class MulticastServer: public QThread
{
    Q_OBJECT
public:
    void run() override;
    MulticastServer();
    void initMulticast();
    void sendMulticastData();
    char header[44];
    char soundBuffer[65000];
    char achMCAddr[MAXADDRSTR] = TIMECAST_ADDR;
    u_long lMCAddr;
    u_short nPort;
    u_long  lTTL;
    u_short nInterval;
    SOCKADDR_IN stLclAddr, stDstAddr;
    struct ip_mreq stMreq;        /* Multicast interface structure */
    SOCKET hSocket;
    WSADATA stWSAData;
    QFile fileToPlay;
    QTimer * t;

public slots:
    void playData();
};


#endif // MULTICASTSERVER_H
