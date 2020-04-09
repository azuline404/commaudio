#include "clienttransfer.h"
#include "mainwindow.h"
#define BUFLEN 65000
ClientTransfer::ClientTransfer()
{

}

void ClientTransfer::run() {
    startClientTransfer();
}
void ClientTransfer::startClientTransfer(){
    WSADATA WSAData;
    WORD wVersionRequested;
    const char * ipAddress = serverIPAddress.toStdString().c_str();
    const char * fileName = this->fileName.toStdString().c_str();
    int n, bytes_to_read;
    SOCKET sd;
    struct hostent	*hp;
    struct sockaddr_in server;
    char  *host, *bp, rbuf[BUFLEN], sbuf[BUFLEN], **pptr;

    wVersionRequested = MAKEWORD( 2, 2 );
    int x = WSAStartup( wVersionRequested, &WSAData );
    if ( x != 0 ) //No usable DLL
    {
        printf ("DLL not found!\n");
        exit(1);
    }
    // Create the socket
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Cannot create socket");
        exit(1);
    }
    memset((char *)&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if ((hp = gethostbyname(ipAddress)) == NULL)
    {
        fprintf(stderr, "Unknown server address\n");
        exit(1);
    }
     memcpy((char *)&server.sin_addr, hp->h_addr, hp->h_length);

    // Connecting to the server
    if (::connect (sd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        fprintf(stderr, "Can't connect to server\n");
        perror("connect");
        exit(1);
    }
    printf("Connected:    Server Name: %s\n", hp->h_name);
    pptr = hp->h_addr_list;
    sbuf[0] = 0x02;
    strcat(sbuf, fileName);

    // Transmit data through the socket
    send (sd, sbuf, BUFLEN, 0);
    bp = rbuf;
    bytes_to_read = BUFLEN;
    // opening file in writing mode

    // client makes repeated calls to recv until no more data is expected to arrive.
    n = recv (sd, rbuf, 65000, 0);
    qDebug("%s", rbuf);
    if (strcmp(rbuf, "X") == 0) {
        emit DownloadFail();
    }
    else {
        std::ofstream outputFile;
        std::string fileName2 = "Copy - ";
        fileName2 += fileName;
        outputFile.open(fileName2, std::ios::app | std::ios::binary);
        while (n > 0) {
            n = recv(sd, rbuf, 65000, 0);
            std::string strBuffer(rbuf, n);
            if (strcmp(rbuf, "K") == 0) {
                break;
            }
            outputFile << strBuffer;
            memset(rbuf, 0, 65000);
        }
        emit DownloadSuccess();
        outputFile.close();
    }
    close (sd);
}
