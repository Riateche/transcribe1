#include "AudioWaveWidget.h"
#include <QPaintEvent>
#include <QPainter>
#include <QBrush>
#include <QtMath>
#include <QtDebug>

AudioWaveWidget::AudioWaveWidget(QWidget *parent) : QWidget(parent)
{
    m_audioData = 0;
    m_amplitudeScale = 1.0f;
    m_samplesPerPixel = 1;
}

void AudioWaveWidget::setAudioData(const AudioData *audioData)
{
    m_audioData = audioData;
    if (m_audioData != 0)
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
        qDebug() << "m_amplitudeScale=" << m_amplitudeScale;

        m_samplesPerPixel = m_audioData->numSamples() / width();
    }
    update();
}

void AudioWaveWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::lightGray);

    if (m_audioData == 0)
    {
        return;
    }

    QRect rect(event->rect().x(), 0,
               event->rect().width(), height());
    qDebug() << rect;
    if (m_audioData->numChannels() == 2)
    {
        int spacing = 10;
        int partHeight = (rect.height() - spacing) / 2;
        rect.setHeight(partHeight);
        drawWave(painter, rect, 0);
        rect.setY(partHeight + spacing);
        drawWave(painter, rect, 1);
    }
    else
    {
        drawWave(painter, rect, 0);
    }
}

void AudioWaveWidget::drawWave(QPainter &painter, const QRect &rect, int channel)
{
    QPen pen(Qt::darkGreen);
    painter.setPen(pen);
    for (int i = 0; i < rect.width(); i++)
    {
        int sampleIndex = (i + rect.x()) * m_samplesPerPixel;
        float sumPos = 0, sumNeg = 0;
        int cntPos = 0, cntNeg = 0;
        for (int j = 0; j < m_samplesPerPixel; j++)
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

        int yc = rect.height() / 2;
        int y1 = yc - (avgPos * rect.height() / 2);
        int y2 = yc + (avgNeg * rect.height() / 2);
        painter.drawLine(rect.x()+i, y1, rect.x()+i, y2);
    }
}
