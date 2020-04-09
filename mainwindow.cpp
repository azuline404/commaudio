#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionStream_Audio->setEnabled(false);
    ui->actionPlay_File->setEnabled(false);
    ui->actionMicrophone->setEnabled(false);

    ui->function_stack->setHidden(true);

    ui->label_stream_vol_value->setText(QString::number(ui->slider_stream_volume->value()));
    ui->label_file_vol_value->setText(QString::number(ui->slider_file_volume->value()));
    ui->label_mic_vol_value->setText(QString::number(ui->slider_mic_volume->value()));

    ui->btn_stream_disconnect->setEnabled(false);
    ui->btn_mic_disconnect->setEnabled(false);

    QObject::connect(ui->actionServer_Start, SIGNAL(triggered()),
        this, SLOT(startServer()));

    server = Server::getInstance();
    audio = new AudioController();
    transfer = ClientTransfer::getInstance();
        multicastServer = new MulticastServer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionStream_Audio_triggered()
{
    if(isClient) {
        ui->function_stack->setCurrentIndex(0);
    } else {
        ui->function_stack->setCurrentIndex(3);
    }
}

void MainWindow::on_actionPlay_File_triggered()
{
    if(isClient) {
        ui->function_stack->setCurrentIndex(1);
    } else {
        ui->function_stack->setCurrentIndex(4);
    }
}

void MainWindow::on_actionMicrophone_triggered()
{
    ui->function_stack->setCurrentIndex(2);
}

void MainWindow::on_actionClient_triggered()
{
    isClient = true;
    ui->actionStream_Audio->setEnabled(true);
    ui->actionPlay_File->setEnabled(true);
    ui->actionMicrophone->setEnabled(true);

    ui->function_stack->setVisible(true);
    ui->function_stack->setCurrentIndex(0);
}

void MainWindow::on_actionServer_triggered()
{
    isClient = false;

    ui->actionStream_Audio->setEnabled(true);
    ui->actionPlay_File->setEnabled(true);
    ui->actionMicrophone->setEnabled(false);

    ui->function_stack->setCurrentIndex(3);
    ui->function_stack->setVisible(true);
}

void MainWindow::on_slider_stream_volume_sliderMoved(int position)
{
    ui->label_stream_vol_value->setText(QString::number(ui->slider_stream_volume->value()));
}

void MainWindow::on_slider_file_volume_sliderMoved(int position)
{
    ui->label_file_vol_value->setText(QString::number(ui->slider_file_volume->value()));
}

void MainWindow::on_slider_mic_volume_sliderMoved(int position)
{
    ui->label_mic_vol_value->setText(QString::number(ui->slider_mic_volume->value()));
}


void MainWindow::on_btn_stream_connect_clicked()
{
    ui->btn_stream_connect->setEnabled(false);
    ui->btn_stream_disconnect->setEnabled(true);

    ui->stream_ip->setEnabled(false);
    ui->stream_port->setEnabled(false);
    multicastClient = new MulticastClient();
    multicastClient->initMulticastClient();
    multicastClient->connected = true;
    multicastClient->start();
}

void MainWindow::on_btn_stream_disconnect_clicked()
{
    ui->btn_stream_connect->setEnabled(true);
    ui->btn_stream_disconnect->setEnabled(false);

    ui->stream_ip->setEnabled(true);
    ui->stream_port->setEnabled(true);
    multicastClient->connected = false;
}

void MainWindow::on_btn_mic_connect_clicked()
{
    ui->btn_mic_connect->setEnabled(false);
    ui->btn_mic_disconnect->setEnabled(true);

    ui->mic_ip->setEnabled(false);
    ui->mic_port->setEnabled(false);
}

void MainWindow::on_btn_mic_disconnect_clicked()
{
    ui->btn_mic_connect->setEnabled(true);
    ui->btn_mic_disconnect->setEnabled(false);

    ui->mic_ip->setEnabled(true);
    ui->mic_port->setEnabled(true);
}

void MainWindow::startServer() {
    ui->actionServer_Start->setChecked(true);
    server->startServer();
}

void MainWindow::on_server_stream_start_clicked()
{
    multicastServer->initMulticast();
//    multicast->sendMulticastData();
}

void MainWindow::on_server_file_start_clicked()
{

}


void MainWindow::on_btn_play_file_clicked()
{
    QFileDialog temp;
    QString fileName = temp.getOpenFileName(this,"Open a file", "C:\\");
    QMessageBox::information(this,"File Selection","Currently Playing: " + fileName.section('/',-1));
    audio->setFileName(fileName);
    audio->start();
}

void MainWindow::on_server_stream_file_clicked()
{
    multicastServer->fileToPlay.seek(0);
    multicastServer->start();
}

void MainWindow::on_btn_pause_clicked()
{
    audio->pause();
}

void MainWindow::on_btn_resume_clicked()
{
    audio->resume();
}

void MainWindow::on_btn_stop_clicked()
{
    audio->stop();
}

void MainWindow::on_btn_download_file_clicked()
{
    transfer->serverIPAddress = ui->file_ip->text();
    transfer->port = ui->file_port->text().toInt();
    transfer->fileName = ui->file_name->text().toStdString().c_str();
    transfer->start();
    connect(transfer, SIGNAL(DownloadFail()), this, SLOT(DownloadFailed()));
    connect(transfer, SIGNAL(DownloadSuccess()), this, SLOT(DownloadSuccess()));
}
void MainWindow::DownloadFailed() {
     QMessageBox::information(this,"File Download","Result: Server Could Not Find File");
}

void MainWindow::DownloadSuccess() {
     QMessageBox::information(this,"File Download","Result: Download Success!");
}



void MainWindow::on_select_file_clicked()
{
    QFileDialog temp;
    QString fileName = temp.getOpenFileName(this,"Select a wav file", "C:\\");
    multicastServer->fileToPlay.setFileName(fileName);
}
