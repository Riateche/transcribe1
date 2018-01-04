#ifndef AUDIOWAVEDATA_H
#define AUDIOWAVEDATA_H

#include <QPair>
#include <QHash>
#include <QMutex>
#include <QWaitCondition>
#include "AudioData.h"

class AudioWaveData
{
public:
    AudioWaveData();

    void setAudioData(const AudioData* audioData);
    float getAverage(int channel, int sampleIndex, int size);

private:
    void calculateAmplitudeScale();
    QPair<float, float> calculate(int channel, int sampleIndex, int size);
    QPair<float, float> calculateCached(int channel, int sampleIndex, int size);
    float calculateSum(int channel, int startIndex, int size);
    float calculateSum(int channel, int size);

private:
    typedef QHash<int, float> LevelCache;
    typedef QHash<int, LevelCache> SumCache;
    QMutex m_mutex;
    QWaitCondition m_workCondition;

    const AudioData* m_audioData;

    float m_amplitudeScale;
    int m_lastSize;
    QHash<int, QPair<float, float>> m_sampleAvgCache[2];
    SumCache m_sumCache[2];
};

#endif // AUDIOWAVEDATA_H
