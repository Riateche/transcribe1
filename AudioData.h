#ifndef AUDIODATA_H
#define AUDIODATA_H

#include <QBuffer>
#include <QString>
#include <QVector>

class AudioData : public QBuffer
{
    Q_OBJECT
public:
    explicit AudioData(QObject *parent = nullptr);

    bool isValid() const;
    bool load(const QString& filePath);
    QString filePath() const;
    int numChannels() const;
    int sampleRate() const;
    int numSamples() const;

    float sample(int channel, int sampleIndex) const;

private:
    QString m_filePath;
    int m_numChannels;
    int m_sampleRate;
    int m_numSamples;
    QVector<float> m_samples;
};

#endif // AUDIODATA_H
