#include "server.h"
#include <QDebug>
Server* Server::u_instance = NULL;

SOCKET AcceptSocket;


int Server::startServer(){
    qDebug() << "started TCP server";
    DWORD ThreadId;
    HANDLE ThreadHandle;
    if ((ThreadHandle = CreateThread(NULL, 0, TCPServer, 0, 0, &ThreadId)) == NULL)
    {
         return 1;
    }
    return 0;
}

DWORD WINAPI Server::TCPServer(LPVOID lpParameter){
       WSADATA wsaData;
       SOCKET ListenSocket;
       SOCKADDR_IN InternetAddr;
       INT Ret;
       HANDLE ThreadHandle;
       DWORD ThreadId;
       WSAEVENT AcceptEvent;

       if ((Ret = WSAStartup(0x0202,&wsaData)) != 0)
       {
          qDebug("WSAStartup failed with error %d\n", Ret);
          WSACleanup();
          return FALSE;
       }

       if ((ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
          WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
       {
          qDebug("Failed to get a socket %d\n", WSAGetLastError());
          return FALSE;
       }

       InternetAddr.sin_family = AF_INET;
       InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
       InternetAddr.sin_port = htons(7777);

       if (bind(ListenSocket, (PSOCKADDR) &InternetAddr,
          sizeof(InternetAddr)) == SOCKET_ERROR)
       {
          qDebug("bind() failed with error %d\n", WSAGetLastError());
          return FALSE;
       }

       if (listen(ListenSocket, 5))
       {
          qDebug("listen() failed with error %d\n", WSAGetLastError());
          return FALSE;
       }

       if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
       {
          qDebug("WSACreateEvent() failed with error %d\n", WSAGetLastError());
          return FALSE;
       }

       // Create a worker thread to service completed I/O requests.

       if ((ThreadHandle = CreateThread(NULL, 0, WorkerThread, (LPVOID) AcceptEvent, 0, &ThreadId)) == NULL)
       {
          qDebug("CreateThread failed with error %d\n", GetLastError());
          return FALSE;
       }
       while(TRUE)
       {
          AcceptSocket = accept(ListenSocket, NULL, NULL);

          if (WSASetEvent(AcceptEvent) == FALSE)
          {
             qDebug("WSASetEvent failed with error %d\n", WSAGetLastError());
             return FALSE;
          }
       }
       return TRUE;
}

DWORD WINAPI Server::WorkerThread(LPVOID lpParameter)
{
   DWORD Flags;
   LPSOCKET_INFORMATION SocketInfo;
   WSAEVENT EventArray[1];
   DWORD Index;
   DWORD RecvBytes;
   HANDLE ThreadHandle;

   // Save the accept event in the event array.

   EventArray[0] = (WSAEVENT) lpParameter;

   while(TRUE)
   {
      // Wait for accept() to signal an event and also process WorkerRoutine() returns.

      while(TRUE)
      {
         Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);

         if (Index == WSA_WAIT_FAILED)
         {
            qDebug("WSAWaitForMultipleEvents failed with error %d\n", WSAGetLastError());
            return FALSE;
         }

         if (Index != WAIT_IO_COMPLETION)
         {
            // An accept() call event is ready - break the wait loop
            break;
         }
      }

      WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);

      // Create a socket information structure to associate with the accepted socket.

      if ((SocketInfo = (LPSOCKET_INFORMATION) GlobalAlloc(GPTR,
         sizeof(SOCKET_INFORMATION))) == NULL)
      {
         qDebug("GlobalAlloc() failed with error %d\n", GetLastError());
         return FALSE;
      }

      // Fill in the details of our accepted socket.

      SocketInfo->Socket = AcceptSocket;
      ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
      SocketInfo->BytesSEND = 0;
      SocketInfo->BytesRECV = 0;
      SocketInfo->DataBuf.len = BUFSIZE;
      SocketInfo->DataBuf.buf = SocketInfo->Buffer;

      Flags = 0;
      if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
         &(SocketInfo->Overlapped), ReceiveRoutine) == SOCKET_ERROR)
      {
         if (WSAGetLastError() != WSA_IO_PENDING)
         {
            qDebug("WSARecv() failed with error %d\n", WSAGetLastError());
            return FALSE;
         }
      }
      qDebug("Socket %d connected\n", AcceptSocket);
   }
   return TRUE;
}

DWORD WINAPI Server::SendFileThread(LPVOID lpParameter) {
    FILE_INFORMATION * fileStruct = (FILE_INFORMATION*)lpParameter;
    SOCKET sd = fileStruct->sd;
    char soundBuffer[65000];
    std::ifstream fs("output.wav", std::ifstream::binary);
    memset(soundBuffer,0,65000);
    fs.read(soundBuffer,65000);
    int bytesRead = fs.gcount();
    send(sd, soundBuffer,bytesRead,0);
    fs.seekg (0, fs.end);
    int remainingBytes = fs.tellg();
    fs.seekg (0, fs.beg);
    while (remainingBytes > 0) {
        memset(soundBuffer,0,65000);
        fs.read(soundBuffer,65000);
        int bytesRead = fs.gcount();
        send(sd, soundBuffer,bytesRead,0);
        qDebug() << bytesRead;
        remainingBytes -= bytesRead;
    }
    send (sd, "K", 1, 0);
    free(fileStruct->fileName);
    free(fileStruct);
    fs.close();
}

void CALLBACK Server::ReceiveRoutine(DWORD Error, DWORD BytesTransferred,
   LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
   DWORD SendBytes, RecvBytes;
   DWORD Flags;

   // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
   LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;

   if (Error != 0)
   {
     qDebug("I/O operation failed with error %d\n", Error);
   }

   if (BytesTransferred == 0)
   {
      qDebug("Closing socket %d\n", SI->Socket);
   }

   if (Error != 0 || BytesTransferred == 0)
   {
      closesocket(SI->Socket);
      GlobalFree(SI);
      return;
   }

   // Check to see if the BytesRECV field equals zero. If this is so, then
   // this means a WSARecv call just completed so update the BytesRECV field
   // with the BytesTransferred value from the completed WSARecv() call.
   else
   {

      SI->BytesRECV = BytesTransferred;
      SI->BytesSEND = 0;

      std::ofstream outputFile;


//      std::string  strBuffer(SI->DataBuf.buf, BytesTransferred);
//      outputFile.open("output.wav", std::ios::app | std::ios::binary);
//      outputFile << strBuffer;
//      outputFile.close();
      char * test = (char*)malloc(sizeof(char) * BytesTransferred);
      if (SI->DataBuf.buf[0] == STX) {
          memcpy(test, &SI->DataBuf.buf[1], BytesTransferred - 1);
          qDebug("File name: %s", test);
          FILE *file;
              if((file = fopen(test,"rb"))!=NULL){
                  DWORD ThreadId;
                  HANDLE ThreadHandle;
                  FILE_INFORMATION * fileStruct = (FILE_INFORMATION*)malloc(sizeof(FILE_INFORMATION));
                  fileStruct->sd = SI->Socket;
                  fileStruct->fileName = test;
                  if ((ThreadHandle = CreateThread(NULL, 0, SendFileThread, (LPVOID)fileStruct, 0, &ThreadId)) == NULL)
                  {
                      qDebug() << "Failed to create send file thread";
                  }
              }
              else {
                  qDebug() << "file doesn't exist";
                  send(SI->Socket, "X", 1, 0);
              }
         }
      }


      SI->BytesRECV = 0;

      Flags = 0;
      ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

      SI->DataBuf.len = BUFSIZE;
      SI->DataBuf.buf = SI->Buffer;

      if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
         &(SI->Overlapped), ReceiveRoutine) == SOCKET_ERROR)
      {
         if (WSAGetLastError() != WSA_IO_PENDING )
         {
            qDebug("WSARecv() failed with error %d\n", WSAGetLastError());
            return;
         }
      }
}
