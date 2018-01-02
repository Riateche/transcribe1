#ifndef AUDIOWAVEWIDGET_H
#define AUDIOWAVEWIDGET_H

#include <QWidget>
#include "AudioData.h"

class AudioWaveWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AudioWaveWidget(QWidget *parent = nullptr);

    void setAudioData(const AudioData* audioData);

signals:

public slots:

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);

private:
    const AudioData* m_audioData;
    float m_amplitudeScale;
    int m_samplesPerPixel;

    void drawWave(QPainter& painter, const QRect& rect, int channel);
};

#endif // AUDIOWAVEWIDGET_H
