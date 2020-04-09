#ifndef SOCKETINFORMATION_H
#define SOCKETINFORMATION_H

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <QFileDialog>
#include <QMessageBox>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <QThread>
#include <QTimer>
#define BUFSIZE 200000
#define STX 0x02

typedef struct _SOCKET_INFORMATION {
   OVERLAPPED Overlapped;
   SOCKET Socket;
   CHAR Buffer[BUFSIZE];
   WSABUF DataBuf;
   DWORD BytesSEND;
   DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

typedef struct {
    SOCKET sd;
    char * fileName;
} FILE_INFORMATION;

#endif // SOCKETINFORMATION_H
