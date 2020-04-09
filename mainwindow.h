#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "constants.h"
#include "Server.h"
#include "multicastserver.h"
#include "multicastclient.h"
#include "audiocontroller.h"
#include "clienttransfer.h"
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread workerThread;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionStream_Audio_triggered();

    void on_actionPlay_File_triggered();

    void on_actionMicrophone_triggered();

    void on_actionClient_triggered();

    void on_actionServer_triggered();

    void on_slider_stream_volume_sliderMoved(int position);

    void on_slider_file_volume_sliderMoved(int position);

    void on_slider_mic_volume_sliderMoved(int position);

    void on_btn_stream_connect_clicked();

    void on_btn_stream_disconnect_clicked();

    void on_btn_mic_connect_clicked();

    void on_btn_mic_disconnect_clicked();

    void startServer();


    void on_server_stream_start_clicked();

    void on_server_file_start_clicked();

    void on_btn_play_file_clicked();

    void on_server_stream_file_clicked();


    void on_btn_pause_clicked();

    void on_btn_resume_clicked();

    void on_btn_stop_clicked();

    void on_btn_download_file_clicked();

    void DownloadFailed();

    void DownloadSuccess();

    void on_select_file_clicked();

signals:

private:
    Ui::MainWindow *ui;
    bool isClient;
    void set_ui_client();
    void set_ui_server();
    Server * server;
    MulticastServer * multicastServer;
    MulticastClient * multicastClient;
    AudioController * audio;
    ClientTransfer * transfer;

};

#endif // MAINWINDOW_H
