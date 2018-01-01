#ifndef SOUNDDEVICE_H
#define SOUNDDEVICE_H

#include <QAudioOutput>
#include <QIODevice>
#include "AudioFile.h"

class SoundDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit SoundDevice(QObject *parent = nullptr);

    void loadFile(const QString& filePath);


    void setPlaying(bool v);



private:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

    AudioFile<float>* m_audioFile;
    qint64 m_pos;
    qint64 m_numSamples;

    //const qint64 READ_SIZE = 512;

    bool m_wasStarted;
    bool m_isPlaying;
    QAudioOutput *m_audioOutput;


};

#endif // SOUNDDEVICE_H
