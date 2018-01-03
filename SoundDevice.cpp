#include "SoundDevice.h"
#include <QtGlobal>
#include <QtMath>
#include <QDebug>
#include <QAudioFormat>
#include <QBuffer>


SoundDevice::SoundDevice(QObject *parent) : QIODevice(parent)
{
    m_tempo = 1.0;

    m_audioOutput = nullptr;
}

bool SoundDevice::loadFile(const QString &filePath)
{
    if (m_audioOutput) {
        delete m_audioOutput;
    }

    if (!m_audioData.load(filePath))
    {
        emit dataChanged();
        return false;
    }

    QAudioFormat format;
    format.setChannelCount(m_audioData.numChannels());
    format.setSampleRate(m_audioData.sampleRate());
    format.setSampleSize(sizeof(float) * 8);
    format.setSampleType(QAudioFormat::Float);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    m_audioOutput = new QAudioOutput(format);
    connect(m_audioOutput, &QAudioOutput::stateChanged, this, &SoundDevice::stateChanged);

    m_processor.setChannels(m_audioData.numChannels());
    m_processor.setSampleRate(m_audioData.sampleRate());
    m_processor.setPitch(1);
    m_processor.clear();


//    connect(this, &SoundDevice::aboutToStop,
//            m_audioOutput, &QAudioOutput::stop, Qt::QueuedConnection);
    connect(m_audioOutput, &QAudioOutput::stateChanged,
            this, &SoundDevice::audioOutputStateChanged);
    open(ReadOnly);

    emit dataChanged();

    return true;
}

const AudioData* SoundDevice::audioData() const
{
    return &m_audioData;
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

bool SoundDevice::isValid()
{
    return m_audioData.isValid();
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
    if (m_audioOutput->state() == QAudio::SuspendedState)
    {
        m_audioOutput->resume();
    }
    else if (m_audioOutput->state() == QAudio::StoppedState)
    {
        m_audioOutput->start(this);
    }
}

void SoundDevice::seek(int sampleId)
{
    m_processor.clear();
    m_audioData.seek(sampleId * m_audioData.sampleSize());

    QAudio::State state = m_audioOutput->state();
    m_audioOutput->reset();
    if (state == QAudio::ActiveState)
    {
        m_audioOutput->start(this);
    }
}

int SoundDevice::position() const
{
    return m_audioData.pos() / m_audioData.sampleSize();
}


qint64 SoundDevice::readData(char *data, qint64 maxlen)
{
    if (!m_audioData.isOpen()) {
        return -1;
    }

    qint64 requestedSamples = maxlen / m_audioData.sampleSize();

    while (!m_audioData.atEnd() && (m_processor.numSamples() < requestedSamples))
    {
        char buf[4096];
        qint64 size = m_audioData.read(buf, sizeof(buf));
        m_processor.putSamples(reinterpret_cast<float*>(&buf), size / m_audioData.sampleSize());
    }
    if (m_audioData.atEnd())
    {
        m_processor.flush();
        emit positionChanged(m_audioData.numSamples());
    }
    else
    {
        emit positionChanged(m_audioData.pos() / m_audioData.sampleSize());
    }

    if (m_processor.numUnprocessedSamples() == 0) {
        return -1;
    }

    qint64 outputSamples = m_processor.receiveSamples(reinterpret_cast<float*>(data),
                                                      static_cast<uint>(maxlen / m_audioData.sampleSize()));
    qDebug() << "numSamples =" << m_processor.numSamples() << "numUnprocessedSamples =" << m_processor.numUnprocessedSamples();
    return outputSamples * m_audioData.sampleSize();
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
