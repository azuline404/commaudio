#ifndef SERVER_H
#define SERVER_H

#include "SocketInformation.h"
#include <QDialog>

class Server : public QObject
{
    Q_OBJECT
public:
    static Server * getInstance() {
        if (!u_instance) {
            u_instance = new Server();
        }
        return u_instance;
    }

    int startServer();
    static DWORD WINAPI TCPServer(LPVOID lpParameter);
    static DWORD WINAPI WorkerThread(LPVOID lpParameter);
    static DWORD WINAPI SendFileThread(LPVOID lpParameter);
    static void CALLBACK ReceiveRoutine(DWORD Error, DWORD BytesTransferred,
       LPWSAOVERLAPPED Overlapped, DWORD InFlags);

    static Server* u_instance;
    virtual ~Server() {};

private:
     Server(){};  // Private so that it can  not be called
     SOCKET listenSocket;
     WSADATA wsaData;
     SOCKADDR_IN InternetAddr;
};




#endif // SERVER_H
