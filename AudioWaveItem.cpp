#include "AudioWaveItem.h"
#include <QDebug>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include "AudioData.h"
#include <qmath.h>
#include <QPainter>
#include "Scene.h"
#include <QGraphicsSceneMouseEvent>
#include "Scene.h"

AudioWaveItem::AudioWaveItem(Scene *scene)
{
    m_scene = scene;
    m_userCursor = 0;
    m_playerCursor = 0;
    m_amplitudeScale = 1.0f;
    setFlag(ItemUsesExtendedStyleOption);
    connect(scene, &Scene::audioDataChanged,
            this, &AudioWaveItem::audioDataChanged);
    audioDataChanged();
    connect(scene, &Scene::samplesPerPixelChanged,
            this, &AudioWaveItem::samplesPerPixelChanged);
    samplesPerPixelChanged();
}

void AudioWaveItem::setHeight(int height)
{
    prepareGeometryChange();
    m_size.setHeight(height);
}


QRectF AudioWaveItem::boundingRect() const
{
    return QRectF(QPointF(), m_size);
}

void AudioWaveItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    //painter.fillRect(event->rect(), Qt::lightGray);

    if (m_scene->audioData() == 0)
    {
        return;
    }

    QRect rect(option->exposedRect.x(), 0,
               option->exposedRect.width(), m_size.height());
    //qDebug() << rect;
    if (m_scene->audioData()->numChannels() == 2)
    {
        int spacing = rect.height() / 20;
        int partHeight = (rect.height() - spacing) / 2;
        rect.setHeight(partHeight);
        drawWave(painter, rect, 0);
        rect.setY(partHeight + spacing);
        rect.setHeight(partHeight);
        drawWave(painter, rect, 1);
    }
    else
    {
        drawWave(painter, rect, 0);
    }

    int startSampleIndex = rect.x() * m_scene->samplesPerPixel();
    int endSampleIndex = (rect.x() + rect.width()) * m_scene->samplesPerPixel();
    if ((startSampleIndex <= m_userCursor) && (m_userCursor < endSampleIndex))
    {
        QPen pen(Qt::red);
        painter->setPen(pen);
        int x = m_userCursor / m_scene->samplesPerPixel();
        painter->drawLine(x, 0, x, m_size.height()-1);
    }
    if ((startSampleIndex <= m_playerCursor) && (m_playerCursor < endSampleIndex))
    {
        QPen pen(Qt::black);
        painter->setPen(pen);
        int x = m_playerCursor / m_scene->samplesPerPixel();
        painter->drawLine(x, 0, x, m_size.height()-1);
    }
}


void AudioWaveItem::audioDataChanged()
{
    if (!m_scene->audioData()) return;

    m_userCursor = 0;
    m_playerCursor = 0;
    {
        float sumsq = 0.0f;
        for (int i = 0; i < m_scene->audioData()->numSamples(); i++)
        {
            for (int ch = 0; ch < m_scene->audioData()->numChannels(); ch++)
            {
                float sample = m_scene->audioData()->sample(ch, i);
                sumsq += sample * sample;
            }
        }
        int n = m_scene->audioData()->numSamples() * m_scene->audioData()->numChannels();
        float stddev = qSqrt(sumsq / n);

        if (stddev > 0.0001)
        {
            m_amplitudeScale = 1.0f / (3*stddev);
        }
        else
        {
            m_amplitudeScale = 1.0f;
        }

        prepareGeometryChange();
        m_size.setWidth(m_scene->totalTrackWidthInPixels());
    }
    update();

}

void AudioWaveItem::samplesPerPixelChanged()
{
    prepareGeometryChange();
    m_size.setWidth(m_scene->totalTrackWidthInPixels());
}

void AudioWaveItem::setPlayerCursor(int sampleIndex)
{
    m_playerCursor = sampleIndex;
    scene()->update();
}

void AudioWaveItem::drawWave(QPainter *painter, const QRect &rect, int channel)
{
    //qDebug() << "drawWave: " << rect;
    QPen pen(Qt::darkGreen);
    painter->setPen(pen);
    for (int i = 0; i < rect.width(); i++)
    {
        int sampleIndex = (i + rect.x()) * m_scene->samplesPerPixel();
        float sumPos = 0, sumNeg = 0;
        int cntPos = 0, cntNeg = 0;
        for (int j = 0; j < m_scene->samplesPerPixel(); j++)
        {
            float sample = m_amplitudeScale * m_scene->audioData()->sample(channel, sampleIndex + j);
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
        painter->drawLine(rect.x()+i, rect.y()+y1, rect.x()+i, rect.y()+y2);
    }
}

void AudioWaveItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_userCursor = m_scene->samplesPerPixel() * event->pos().x();
        scene()->update();

        Scene* parent = static_cast<Scene*>(scene());
        parent->userCursorChanged(m_userCursor);
    }
}
