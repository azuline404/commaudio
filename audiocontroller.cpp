#include "audiocontroller.h"

AudioController::AudioController(){
     format = new QAudioFormat();
}

void AudioController::run() {
    playFile();
}

void AudioController::stop() {
    audio->stop();
}

void AudioController::pause() {
    audio->suspend();
}
void AudioController::resume() {
    audio->resume();
}
void AudioController::setFileName(QString fileName) {
    this->fileToPlay.setFileName(fileName);
}
void AudioController::setFormat(char *header) {
    short bps,channel;
    int frequency;
    memcpy(&bps, header + 34, 2);
    memcpy(&channel, header + 22, 2);
    memcpy(&frequency, header + 24,4);
    format->setSampleRate(frequency);
    format->setChannelCount(channel);
    format->setSampleSize(bps);
    format->setCodec("audio/pcm");
    format->setByteOrder(QAudioFormat::LittleEndian);
    format->setSampleType(QAudioFormat::SignedInt);
}
void AudioController::playFile() {
    char header[44];
    fileToPlay.open(QFile::ReadOnly);
    fileToPlay.read(header,44);
    setFormat(header);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(*format)) {
      qDebug() << "raw audio format not supported by backend";
      return;
    }
    audio= new QAudioOutput(*format);
    audio->setBufferSize(800000);

    wavBuffer = audio->start();
    fileToPlay.read(soundData,500000);
    wavBuffer->write(soundData, 500000);
    while (!fileToPlay.atEnd()) {
        if (audio->bytesFree() > 500000) {
            fileToPlay.read(soundData,500000);
            wavBuffer->write(soundData, 500000);
            memset(soundData, 0, 500000);
        }
    }
//    connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
}

void AudioController::playAudio(char * buffer, int bufferSize) {
    qDebug() << "Called playAudio";
    setFormat(buffer);
    int dataSize = bufferSize - 44;
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(*format)) {
        qDebug() << "raw audio format not supported by backend";
        return;
    }
    audio= new QAudioOutput(*format);
    audio->setBufferSize(5000000);
    if (wavBuffer == nullptr) {
        wavBuffer= audio->start();
    }
    char soundData [dataSize];
    memcpy(soundData, buffer + 44, dataSize);
    qDebug() << wavBuffer->write(soundData, dataSize);
}


void AudioController::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
        case QAudio::IdleState:
            if (!fileToPlay.atEnd()) {
                qDebug() << "read once" << endl;
                fileToPlay.read(soundData,500000);
                wavBuffer->write(soundData, 500000);
                memset(soundData, 0, 500000);
            }
            else {
                fileToPlay.close();
            }
            // Finished playing (no more data)
            break;

        case QAudio::StoppedState:
            // Stopped for other reasons
            break;

        default:
            // ... other cases as appropriate
            break;
    }
}
