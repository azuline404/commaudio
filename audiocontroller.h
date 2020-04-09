#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H
#include <QAudioFormat>
#include <QAudioOutput>
#include <QSound>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QThread>


class AudioController : public QThread
{
    Q_OBJECT
public:
    void run() override;
    AudioController();
    QAudioOutput * audio;
    QIODevice * wavBuffer;
    QAudioFormat * format;
    void playAudio(char * buffer, int bufferSize);
    void playFile();
    char soundData[500000];
    QFile fileToPlay;
    void setFormat (char * header);
    void setFileName (QString fileName);
    void stop();
    void pause();
    void resume();

private slots:
    void handleStateChanged(QAudio::State newState);
};

#endif // AUDIOCONTROLLER_H
