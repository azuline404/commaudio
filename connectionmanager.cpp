#include "connectionmanager.h"

ConnectionManager::ConnectionManager()
{
    WSAData wsaData;
    WSAEVENT acceptEvent = 0;
}

int ConnectionManager::start_wsa(WSAData* wsaData)
{
    int result = 0;

    if((result = WSAStartup(0x0202,wsaData)) != 0)
    {
        result = -1;
    }
    return result;
}

SOCKET* init_socket(int type)
{
    SOCKET* sd;

    if(type == TCP)
    {
        if ((*sd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
                                      WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
        {
            return NULL;
        }
    } else if (type == UDP)
    {
        if((*sd = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
        {
            return NULL;
        }
    }
    return sd;
}

void init_sockaddr_in(struct sockaddr_in* inetAddress, int port)
{
    inetAddress->sin_family = AF_INET;
    inetAddress->sin_addr.s_addr = htonl(INADDR_ANY);
    inetAddress->sin_port = htons(port);
}

int bind_socket(SOCKET* sd, struct sockaddr_in* inetAddress)
{
    int result = 0;

    if(bind(*sd, (PSOCKADDR)inetAddress, sizeof(*inetAddress)) == SOCKET_ERROR)
    {
        result = -1;
    }
    return result;
}

void close_socket(SOCKET* sd)
{
    closesocket(*sd);
}
