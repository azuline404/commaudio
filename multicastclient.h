#ifndef MULTICASTCLIENT_H
#define MULTICASTCLIENT_H
#include "SocketInformation.h"
#include "audiocontroller.h"
#include <QThread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <QDebug>
#define BUFSIZE     65000
#define MAXADDRSTR  16

#define TIMECAST_ADDR "234.5.6.7"
#define TIMECAST_PORT 8910

class MulticastClient : public QThread
{
    Q_OBJECT
public:
    void run() override;
    MulticastClient();
    void startMulticastClient();
    AudioController * audioController;
    SOCKET hSocket;
    void initMulticastClient();
    char achMCAddr[MAXADDRSTR] = TIMECAST_ADDR;
    u_long lMCAddr;
    u_short nPort = TIMECAST_PORT;
    SOCKADDR_IN stLclAddr, stSrcAddr;
    struct ip_mreq stMreq;         /* Multicast interface structure */
    char achInBuf[BUFSIZE];
    BOOL connected = false;
//    LPSOCKET_INFORMATION SocketInfo;
//    static void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred,
//       LPWSAOVERLAPPED Overlapped, DWORD InFlags);
};

#endif // MULTICASTCLIENT_H
