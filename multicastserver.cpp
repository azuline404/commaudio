#include "multicastserver.h"
#include "SocketInformation.h"
#include <QDebug>
#include <QDir>
#define DATASIZE 64956

MulticastServer::MulticastServer()
{
    u_short nPort              = TIMECAST_PORT;
    u_long  lTTL               = TIMECAST_TTL;
    u_short nInterval          = TIMECAST_INTRVL;
}

void MulticastServer::run() {
    fileToPlay.open(QFile::ReadOnly);
    fileToPlay.read(header,44);
    memcpy(soundBuffer, header, 44);
    t = new QTimer();
    /* Timer dot */
    connect(t,SIGNAL(timeout()),this,SLOT(playData()));
    t->start(1000);
    exec();
}

void MulticastServer::playData() {
    if (!fileToPlay.atEnd()) {
        fileToPlay.read(soundBuffer + 44, DATASIZE);
        int sent = sendto(hSocket, soundBuffer, 65000, 0,(struct sockaddr*)&stDstAddr, sizeof(stDstAddr));
    } else
        exit();
}

void MulticastServer::sendMulticastData(){

}

void MulticastServer::initMulticast(){
      int nRet;
      BOOL  fFlag;
      qDebug() << "Initializing Multicast!";

      /* WSAStartup */
      nRet = WSAStartup(0x0202, &stWSAData);
      if (nRet) {
        qDebug ("WSAStartup failed: %d\r\n", nRet);
      }
      nPort = TIMECAST_PORT;
      nInterval = TIMECAST_INTRVL;


      /* Display current settings */
      qDebug ("Multicast Address:%s, Port:%d, IP TTL:%lu, Interval:%d\n", achMCAddr, nPort, lTTL, nInterval);

      /* Get a datagram socket */
      hSocket = socket(AF_INET,SOCK_DGRAM,0);
      if (hSocket == INVALID_SOCKET) {
        qDebug ("socket() failed, Err: %d\n", WSAGetLastError());
        exit(1);
      }

      // Bind the socket
      stLclAddr.sin_family      = AF_INET;
      stLclAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* any interface */
      stLclAddr.sin_port        = 0;                 /* any port */
      nRet = bind(hSocket,
          (struct sockaddr*) &stLclAddr,
          sizeof(stLclAddr));
      if (nRet == SOCKET_ERROR) {
          qDebug ("bind() port: %d failed, Err: %d\n", nPort,
          WSAGetLastError());
      }

      /* Join the multicast group
       *
       * NOTE: According to RFC 1112, a sender does not need to join the
       *  group, however Microsoft requires a socket to join a group in
       *  order to use setsockopt() IP_MULTICAST_TTL (or fails with error
       *  WSAEINVAL).
       */
      stMreq.imr_multiaddr.s_addr = inet_addr(achMCAddr);
      stMreq.imr_interface.s_addr = INADDR_ANY;
      nRet = setsockopt(hSocket,IPPROTO_IP, IP_ADD_MEMBERSHIP,(char *)&stMreq,sizeof(stMreq));
      if (nRet == SOCKET_ERROR) {
        qDebug (
          "setsockopt() IP_ADD_MEMBERSHIP address %s failed, Err: %d\n",achMCAddr, WSAGetLastError());
      }

      /* Set IP TTL to traverse up to multiple routers */
      nRet = setsockopt(hSocket, IPPROTO_IP,IP_MULTICAST_TTL,(char *)&lTTL,sizeof(lTTL));
      if (nRet == SOCKET_ERROR) {
        qDebug ("setsockopt() IP_MULTICAST_TTL failed, Err: %d\n",WSAGetLastError());
      }

      /* Disable loopback */
      fFlag = FALSE;
      nRet = setsockopt(hSocket,IPPROTO_IP,IP_MULTICAST_LOOP,(char *)&fFlag,sizeof(fFlag));
      if (nRet == SOCKET_ERROR) {
          qDebug ("setsockopt() IP_MULTICAST_LOOP failed, Err: %d\n",
          WSAGetLastError());
      }

      /* Assign our destination address */
      stDstAddr.sin_family =      AF_INET;
      stDstAddr.sin_addr.s_addr = inet_addr(achMCAddr);
      stDstAddr.sin_port =        htons(nPort);
      qDebug() << "Hit the end of init!";
}
