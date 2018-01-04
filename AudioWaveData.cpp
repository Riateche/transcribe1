#include "AudioWaveData.h"
#include <QtMath>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <QtDebug>

#define LEVELS 31
static const int powers[LEVELS] = {
    1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024,
    2048, 4096, 8192, 16384, 32768, 65536, 131072,
    262144, 524288, 1048576, 2097152, 4194304,
    8388608, 16777216, 33554432, 67108864, 134217728,
    268435456, 536870912, 1073741824};

AudioWaveData::AudioWaveData()
{
    m_audioData = nullptr;
    m_amplitudeScale = 1.0;
    m_lastSize = 0;
}

void AudioWaveData::setAudioData(const AudioData *audioData)
{
    QMutexLocker locker(&m_mutex);

    m_audioData = audioData;
    m_lastSize = 0;

    QElapsedTimer timer;
    timer.start();
    calculateAmplitudeScale();
    qDebug() << "amplitude scale computation took " << timer.elapsed() << "milliseconds";

    // Precalculate
    timer.start();
    m_sumCache[0].clear();
    m_sumCache[1].clear();
    for (int channel = 0; channel < m_audioData->numChannels(); channel++)
    {
        calculateSum(channel, m_audioData->numSamples());
    }
    qDebug() << "precomputation took " << timer.elapsed() << "milliseconds";
}

float AudioWaveData::getAverage(int channel, int sampleIndex, int size)
{
    Q_ASSERT((0 <= channel) && (channel < 2));
    float sum = calculateSum(channel, sampleIndex, size);
    return sum / size;
}

void AudioWaveData::calculateAmplitudeScale()
{
    float sumsq = 0.0f;
    for (int i = 0; i < m_audioData->numSamples(); i++)
    {
        for (int ch = 0; ch < m_audioData->numChannels(); ch++)
        {
            float sample = m_audioData->sample(ch, i);
            sumsq += sample * sample;
        }
    }
    int n = m_audioData->numSamples() * m_audioData->numChannels();
    float stddev = qSqrt(sumsq / n);

    if (stddev > 0.0001)
    {
        m_amplitudeScale = 1.0f / (3*stddev);
    }
    else
    {
        m_amplitudeScale = 1.0f;
    }
}

QPair<float, float> AudioWaveData::calculate(int channel, int sampleIndex, int size)
{
    if (m_audioData == nullptr)
    {
        return qMakePair(0.0f, 0.0f);
    }

    float sumPos = 0, sumNeg = 0;
    int cntPos = 0, cntNeg = 0;
    for (int j = 0; j < size; j++)
    {
        float sample = m_amplitudeScale * m_audioData->sample(channel, sampleIndex + j);
        if (sample < 0)
        {
            sumNeg += sample;
            cntNeg++;
        }
        else
        {
            sumPos += sample;
            cntPos++;
        }
    }

    float avgPos = (cntPos>0)? (sumPos/cntPos) : 0;
    float avgNeg = (cntNeg>0)? (qAbs(sumNeg)/cntNeg) : 0;
    return qMakePair(avgPos, avgNeg);
}

QPair<float, float> AudioWaveData::calculateCached(int channel, int sampleIndex, int size)
{
    if (size != m_lastSize)
    {
        m_sampleAvgCache[0].clear();
        m_sampleAvgCache[1].clear();
    }
    m_lastSize = size;

    if (!m_sampleAvgCache[channel].contains(sampleIndex))
    {
        m_sampleAvgCache[channel][sampleIndex] = calculate(channel, sampleIndex, size);
    }
    return m_sampleAvgCache[channel][sampleIndex];
}

float AudioWaveData::calculateSum(int channel, int startIndex, int size)
{
    if (size <= 128)
    {
        float sum = 0.0;
        for (int i = 0; i < size; i++)
        {
            sum += qAbs(m_amplitudeScale * m_audioData->sample(channel, startIndex+i));
        }
        return sum;
    }
    int pow = qFloor(qLn(size) / qLn(2.0));
    if (powers[pow] == size)
    {
        int k = startIndex / size;
        if (k * size == startIndex)
        {
            if (!m_sumCache[channel][pow].contains(k))
            {
                float s0 = calculateSum(channel, startIndex, size/2);
                float s1 = calculateSum(channel, startIndex + size/2, size/2);
                m_sumCache[channel][pow][k] = s0 + s1;
            }
            float sum = m_sumCache[channel][pow][k];
            return sum;
        }
    }
    return calculateSum(channel, startIndex+size+1) - calculateSum(channel, startIndex);
}

float AudioWaveData::calculateSum(int channel, int size)
{
    int offset = 0;
    float sum = 0.0;
    for (int level = LEVELS-1; level >= 0; level--)
    {
        int pow = powers[level];
        if (size & pow)
        {
            sum += calculateSum(channel, offset, pow);
            offset += pow;
        }
    }
    return sum;
}
