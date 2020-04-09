#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <winsock2.h>

#include "constants.h"
#include <QSound>

class ConnectionManager
{
public:
    ConnectionManager();
    ~ConnectionManager();

    static int start_wsa(WSAData*);
    static SOCKET* init_socket(int);
    static void init_sockaddr_in(struct sockaddr_in*, int);

    static int bind_socket(SOCKET*, struct sockaddr_in*);

    static void close_socket(SOCKET*);
};

#endif // CONNECTIONMANAGER_H
