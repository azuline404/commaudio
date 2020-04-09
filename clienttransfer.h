#ifndef CLIENTTRANSFER_H
#define CLIENTTRANSFER_H
#include "SocketInformation.h"
#include <QThread>
#include <QMessageBox>


class ClientTransfer : public QThread
{
    Q_OBJECT
public:
    void run() override;
    ClientTransfer();
    void startClientTransfer();
    QString serverIPAddress;
    int port;
    QString fileName;
    static void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred,
       LPWSAOVERLAPPED Overlapped, DWORD InFlags);
signals:
    void DownloadFail();
    void DownloadSuccess();
};




#endif // CLIENTTRANSFER_H
