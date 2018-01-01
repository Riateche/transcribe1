#ifndef SOUNDDEVICE_H
#define SOUNDDEVICE_H

#include <QAudioOutput>
#include <QBuffer>
#include <QIODevice>
#include <QVector>
#include "AudioFile.h"
#include "soundtouch/SoundTouch.h"

class SoundDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit SoundDevice(QObject *parent = nullptr);

    void loadFile(const QString& filePath);


    void setTempo(double v);

    void start();
    void stop();
    void pause();
    void resume();



private:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);



    QBuffer m_samples;
    qint64 m_pos;
    int m_numChannels;
    qint64 m_numSamples;

    //bool m_isPlaying;
    QAudioOutput *m_audioOutput;
    soundtouch::SoundTouch m_processor;
    QVector<float> m_sampleCache;
    double m_tempo;


signals:
//    void aboutToStop();

//    // QIODevice interface
//public:
//    bool atEnd() const;

private slots:
    void audioOutputStateChanged(QAudio::State newState);
};

#endif // SOUNDDEVICE_H
