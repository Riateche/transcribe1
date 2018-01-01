#include "SoundDevice.h"
#include <QtGlobal>
#include <QtMath>
#include <QDebug>
#include <QAudioFormat>


SoundDevice::SoundDevice(QObject *parent) : QIODevice(parent)
{
    m_audioFile = nullptr;
    m_numSamples = 0;
    m_pos = 0;
    m_isPlaying = false;
    m_tempo = 1.0;

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

    m_processor.setChannels(m_numChannels);
    m_processor.setSampleRate(m_audioFile->getSampleRate());
    //m_processor.setPitch(0.25);
    m_processor.clear();


//    connect(this, &SoundDevice::aboutToStop,
//            m_audioOutput, &QAudioOutput::stop, Qt::QueuedConnection);
    connect(m_audioOutput, &QAudioOutput::stateChanged,
            this, &SoundDevice::audioOutputStateChanged);
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
        //emit aboutToStop();
    }
}

void SoundDevice::setTempo(double v)
{
    if (m_tempo == v) {
        return;
    }
    m_tempo = v;
    m_processor.setTempo(m_tempo);
    m_processor.clear();
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
    qint64 realSamples = qMin(static_cast<qint64>(qFloor(m_tempo * maxlen / sampleSize)), m_numSamples - m_pos);
//    if (realSamples <= 0) {



//        setPlaying(false);
//        return -1;
//    }
    if (realSamples > 0) {
        m_sampleCache.resize(realSamples);
        for (qint64 i = 0; i < realSamples; i++) {
            for (int ch = 0; ch < m_numChannels; ch++) {
                m_sampleCache[i * m_numChannels + ch] = m_audioFile->samples[ch][i + m_pos];
            }
        }
        //memcpy(data, m_sampleCache.data(), realSamples * sampleSize);

        m_processor.putSamples(m_sampleCache.data(), realSamples);
        m_pos += realSamples;
        //return realSamples * sampleSize;
    } else {
        m_processor.flush();
        //setPlaying(false);
    }
    qint64 outputSamples = m_processor.receiveSamples(reinterpret_cast<float*>(data),
                                                      static_cast<uint>(maxlen / sampleSize));
    if (outputSamples == 0 && realSamples <= 0) {
        //setPlaying(false);
        return -1;
    }
    qDebug() << "real:" << realSamples << " output:" << outputSamples;

    // reinterpret_cast<float*>(data)[i * m_numChannels + ch] = m_audioFile->samples[ch][i + m_pos];
    qint64 len = outputSamples * sampleSize;
    return len;
}

qint64 SoundDevice::writeData(const char *data, qint64 len)
{
    setErrorString(tr("SoundDevice: write is not supported"));
    return -1;
    Q_UNUSED(data)
    Q_UNUSED(len)
}

void SoundDevice::audioOutputStateChanged(QAudio::State newState)
{
    if (newState == QAudio::IdleState) {
        m_audioOutput->stop();
        m_isPlaying = false;
    }
}



//bool SoundDevice::atEnd() const
//{
//    return !m_isPlaying;
//}
