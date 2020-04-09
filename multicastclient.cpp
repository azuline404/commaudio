#include "multicastclient.h"
#include <winsock2.h>
#include <stdio.h>
#include <QDebug>


MulticastClient::MulticastClient()
{
    audioController = new AudioController();
}

void MulticastClient::initMulticastClient() {
    int nRet;
    WSADATA stWSAData;
    BOOL fFlag;
    /* Init WinSock */
    nRet = WSAStartup(0x0202, &stWSAData);
    if (nRet) {
        qDebug("WSAStartup failed: %d\r\n", nRet);
    }
    nPort = TIMECAST_PORT;
    /* Get a datagram socket */
    hSocket = socket(AF_INET, SOCK_DGRAM,0);
    if (hSocket == INVALID_SOCKET) {
        qDebug("socket() failed, Err: %d\n", WSAGetLastError());
        WSACleanup();
    }
    /* Avoid (WSA)EADDRINUSE error on bind() (last one to bind should
     *  receive dgrams sent to our port, but there are no guarantees of
     *  this, unfortunately). */
    fFlag = TRUE;
    nRet = setsockopt(hSocket, SOL_SOCKET,SO_REUSEADDR,(char*)&fFlag,sizeof(fFlag));
    if (nRet == SOCKET_ERROR) {
        qDebug("setsockopt() SO_REUSEADDR failed, Err: %d\n",
            WSAGetLastError());
    }
    int n = 6000000;
    setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&n, sizeof(n) );
    /* Name the socket (assign the local port number to receive on) */
    stLclAddr.sin_family = AF_INET;
    stLclAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    stLclAddr.sin_port = htons(nPort);
    nRet = bind(hSocket, (struct sockaddr*) & stLclAddr, sizeof(stLclAddr));
    if (nRet == SOCKET_ERROR) {
        qDebug("bind() port: %d failed, Err: %d\n", nPort, WSAGetLastError());
    }

    /* Join the multicast group so we can receive from it */
    stMreq.imr_multiaddr.s_addr = inet_addr(achMCAddr);
    stMreq.imr_interface.s_addr = INADDR_ANY;
    nRet = setsockopt(hSocket,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&stMreq,sizeof(stMreq));
    if (nRet == SOCKET_ERROR) {
        qDebug("setsockopt() IP_ADD_MEMBERSHIP address %s failed, Err: %d\n", achMCAddr, WSAGetLastError());
    }
    qDebug("Now waiting for time updates from the TimeCast server\n");
    qDebug("multicast group address: %s, port number: %d\n", achMCAddr, nPort);
}

void MulticastClient::run(){
    startMulticastClient();
}
void MulticastClient::startMulticastClient(){
        int nRet;
        qDebug() << "Looping to receive from multicast server!";
        while(connected) {
            int addr_size = sizeof(struct sockaddr_in);
            nRet = recvfrom(hSocket, achInBuf,65000,0,(struct sockaddr*) & stSrcAddr,&addr_size);
            if (nRet < 0) {
                qDebug("recvfrom() failed, Error: %d\n", WSAGetLastError());
                WSACleanup();
            }
            audioController->playAudio(achInBuf, nRet - 44);
        }

        /* Leave the multicast group: With IGMP v1 this is a noop, but
         *  with IGMP v2, it may send notification to multicast router.
         *  Even if it's a noop, it's sanitary to cleanup after one's self.
         */
        stMreq.imr_multiaddr.s_addr = inet_addr(achMCAddr);
        stMreq.imr_interface.s_addr = INADDR_ANY;
        nRet = setsockopt(hSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&stMreq, sizeof(stMreq));
        if (nRet == SOCKET_ERROR) {
            qDebug("setsockopt() IP_DROP_MEMBERSHIP address %s failed, Err: %d\n", achMCAddr, WSAGetLastError());
        }
        /* Close the socket */
        closesocket(hSocket);
        audioController->audio->reset();
        /* Tell WinSock we're leaving */
        WSACleanup();
}
