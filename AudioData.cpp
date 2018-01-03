#include "AudioData.h"
#include "AudioFile.h"

AudioData::AudioData(QObject *parent) : QBuffer(parent)
{
    m_numChannels = 0;
    m_sampleRate = 0;
    m_numSamples = 0;
}

bool AudioData::isValid() const
{
    return isOpen();
}

bool AudioData::load(const QString &filePath)
{
    if (isOpen())
    {
        close();
    }

    AudioFile<float> audioFile;
    if (!audioFile.load(filePath.toStdString())) return false;
    m_numChannels = audioFile.getNumChannels();
    if (m_numChannels > 2) return false;

    m_sampleRate = audioFile.getSampleRate();
    m_numSamples = audioFile.getNumSamplesPerChannel();
    m_samples.resize(m_numChannels * m_numSamples);
    for (qint64 i = 0; i < m_numSamples; i++) {
        for (int ch = 0; ch < m_numChannels; ch++) {
            m_samples[i * m_numChannels + ch] = audioFile.samples[ch][i];
        }
    }
    setData(reinterpret_cast<char*>(m_samples.data()), m_numChannels * m_numSamples * sizeof(float));
    open(QBuffer::ReadOnly);

    m_filePath = filePath;
    return true;
}

QString AudioData::filePath() const
{
    return m_filePath;
}

int AudioData::numChannels() const
{
    return m_numChannels;
}

int AudioData::sampleRate() const
{
    return m_sampleRate;
}

int AudioData::numSamples() const
{
    return m_numSamples;
}

int AudioData::sampleSize() const
{
    return sizeof(float) * m_numChannels;
}

float AudioData::sample(int channel, int sampleIndex) const
{
    int index = sampleIndex * m_numChannels + channel;
    if ((index < 0) || (index >= m_samples.size())) return 0.0f;
    return m_samples[index];
}
