#include "SoundDevice.h"
#include <QtGlobal>
#include <QDebug>
#include <QAudioFormat>


SoundDevice::SoundDevice(QObject *parent) : QIODevice(parent)
{
    m_audioFile = nullptr;
    m_numSamples = 0;
    m_pos = 0;
    m_isPlaying = false;
    m_wasStarted = false;

    m_audioOutput = nullptr;
}

void SoundDevice::loadFile(const QString &filePath)
{
    Q_ASSERT(!m_audioOutput);
    m_audioFile = new AudioFile<float>();
    m_audioFile->load(filePath.toStdString());
    m_numSamples = m_audioFile->getNumSamplesPerChannel();

    QAudioFormat format;
    format.setChannelCount(1);
    format.setSampleRate(m_audioFile->getSampleRate());
    format.setSampleSize(sizeof(float) * 8);
    format.setSampleType(QAudioFormat::Float);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    m_audioOutput = new QAudioOutput(format);

    open(ReadOnly);
}

void SoundDevice::setPlaying(bool v)
{
    if (m_isPlaying == v) {
        return;
    }
    m_isPlaying = v;
    if (m_isPlaying) {
        //if (m_wasStarted) {
        //    m_audioOutput->resume();
        //} else {
            m_audioOutput->start(this);
            qDebug() << m_audioOutput->state() << m_audioOutput->error();
            m_wasStarted = true;
        //}
    } else {
        //m_audioOutput->suspend();
        m_audioOutput->reset();
    }
}


qint64 SoundDevice::readData(char *data, qint64 maxlen)
{
    qDebug() << "SoundDevice::readData" << maxlen;
    if (!m_audioFile) {
        return -1;
    }
    if (!m_isPlaying) {
        setPlaying(false);
        return 0;
    }

    qint64 currentNumSamples = qMin(maxlen / qint64(sizeof(float)), m_numSamples - m_pos);
    if (currentNumSamples <= 0) {
        setPlaying(false);
        return 0;
    }
    qint64 len = currentNumSamples * sizeof(float);

    memcpy(data,
           reinterpret_cast<const char*>(&m_audioFile->samples[0][m_pos]),
           len);
    m_pos += currentNumSamples;
    return len;
}

qint64 SoundDevice::writeData(const char *data, qint64 len)
{
    setErrorString(tr("SoundDevice: write is not supported"));
    return -1;
    Q_UNUSED(data)
    Q_UNUSED(len)
}

