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

    m_audioOutput = nullptr;
}

void SoundDevice::loadFile(const QString &filePath)
{
    if (m_audioOutput) {
        delete m_audioOutput;
    }
    m_audioFile = new AudioFile<float>();
    m_audioFile->load(filePath.toStdString());
    m_numChannels = m_audioFile->getNumChannels();
    m_numSamples = m_audioFile->getNumSamplesPerChannel();
    m_pos = 0;
    m_isPlaying = false;

    QAudioFormat format;
    format.setChannelCount(m_numChannels);
    format.setSampleRate(m_audioFile->getSampleRate());
    format.setSampleSize(sizeof(float) * 8);
    format.setSampleType(QAudioFormat::Float);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    m_audioOutput = new QAudioOutput(format);


    connect(this, &SoundDevice::aboutToStop,
            m_audioOutput, &QAudioOutput::stop, Qt::QueuedConnection);
    //connect(m_audioOutput, &QAudioOutput::stateChanged,
    //        this, &SoundDevice::audioOutputStateChanged);
    open(ReadOnly);
}

void SoundDevice::setPlaying(bool v)
{
    if (m_isPlaying == v) {
        return;
    }
    m_isPlaying = v;
    if (m_isPlaying) {
        m_audioOutput->start(this);
        qDebug() << m_audioOutput->state() << m_audioOutput->error();
    } else {
        emit aboutToStop();
    }
}


qint64 SoundDevice::readData(char *data, qint64 maxlen)
{
    if (!m_audioFile) {
        return -1;
    }
    if (!m_isPlaying) {
        return -1;
    }

    qint64 sampleSize = sizeof(float) * m_numChannels;
    qint64 currentNumSamples = qMin(maxlen / sampleSize, m_numSamples - m_pos);
    if (currentNumSamples <= 0) {
        setPlaying(false);
        return -1;
    }
    qint64 len = currentNumSamples * sampleSize;

    for (qint64 i = 0; i < currentNumSamples; i++) {
        for (int ch = 0; ch < m_numChannels; ch++) {
            reinterpret_cast<float*>(data)[i * m_numChannels + ch] = m_audioFile->samples[ch][i + m_pos];
        }
    }
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

//void SoundDevice::audioOutputStateChanged(QAudio::State newState)
//{
//    if (newState == QAudio::IdleState) {
//        m_audioOutput->stop();
//    }
//}



//bool SoundDevice::atEnd() const
//{
//    return !m_isPlaying;
//}
