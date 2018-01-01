#include "SoundDevice.h"
#include <QtGlobal>
#include <QtMath>
#include <QDebug>
#include <QAudioFormat>
#include <QBuffer>


SoundDevice::SoundDevice(QObject *parent) : QIODevice(parent)
{
    m_numSamples = 0;
    m_pos = 0;
    m_tempo = 1.0;

    m_audioOutput = nullptr;
}

void SoundDevice::loadFile(const QString &filePath)
{
    if (m_audioOutput) {
        delete m_audioOutput;
    }

    AudioFile<float> audioFile;
    audioFile.load(filePath.toStdString());
    m_numChannels = audioFile.getNumChannels();
    m_numSamples = audioFile.getNumSamplesPerChannel();
    QVector<float> data(m_numChannels * m_numSamples);
    for (qint64 i = 0; i < m_numSamples; i++) {
        for (int ch = 0; ch < m_numChannels; ch++) {
            data[i * m_numChannels + ch] = audioFile.samples[ch][i];
        }
    }
    m_samples.setData(reinterpret_cast<char*>(data.data()), m_numChannels * m_numSamples * sizeof(float));
    m_samples.open(QBuffer::ReadOnly);

    m_pos = 0;

    QAudioFormat format;
    format.setChannelCount(m_numChannels);
    format.setSampleRate(audioFile.getSampleRate());
    format.setSampleSize(sizeof(float) * 8);
    format.setSampleType(QAudioFormat::Float);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    m_audioOutput = new QAudioOutput(format);

    m_processor.setChannels(m_numChannels);
    m_processor.setSampleRate(audioFile.getSampleRate());
    m_processor.setPitch(1);
    m_processor.clear();


//    connect(this, &SoundDevice::aboutToStop,
//            m_audioOutput, &QAudioOutput::stop, Qt::QueuedConnection);
    connect(m_audioOutput, &QAudioOutput::stateChanged,
            this, &SoundDevice::audioOutputStateChanged);
    open(ReadOnly);
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

void SoundDevice::start()
{
    m_audioOutput->start(this);
    qDebug() << m_audioOutput->state() << m_audioOutput->error();
}

void SoundDevice::stop()
{
    qDebug() << "stopped";
    m_processor.clear();
    m_audioOutput->stop();
}

void SoundDevice::pause()
{
    m_audioOutput->suspend();
}

void SoundDevice::resume()
{
    m_audioOutput->resume();
}


qint64 SoundDevice::readData(char *data, qint64 maxlen)
{
    if (!m_samples.isOpen()) {
        return -1;
    }

    qint64 sampleSize = sizeof(float) * m_numChannels;

    qint64 requestedSamples = maxlen / sampleSize;

    while (!m_samples.atEnd() && (m_processor.numSamples() < requestedSamples))
    {
        char buf[4096];
        qint64 size = m_samples.read(buf, sizeof(buf));
        m_processor.putSamples(reinterpret_cast<float*>(&buf), size / sampleSize);
    }
    if (m_samples.atEnd())
    {
        m_processor.flush();
    }

    if (m_processor.numUnprocessedSamples() == 0) {
        return -1;
    }

    qint64 outputSamples = m_processor.receiveSamples(reinterpret_cast<float*>(data),
                                                      static_cast<uint>(maxlen / sampleSize));
    qDebug() << "numSamples =" << m_processor.numSamples() << "numUnprocessedSamples =" << m_processor.numUnprocessedSamples();
    return outputSamples * sampleSize;
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
        stop();
    }
}



//bool SoundDevice::atEnd() const
//{
//    return !m_isPlaying;
//}
