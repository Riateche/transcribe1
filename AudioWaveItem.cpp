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
    m_userCursorEnd = 0;
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
    int startSampleIndex = rect.x() * m_scene->samplesPerPixel();
    int endSampleIndex = (rect.x() + rect.width()) * m_scene->samplesPerPixel();

    // Draw selection
    if (m_userCursor != m_userCursorEnd)
    {
        int selectionStart = qMax(m_userCursor+1, startSampleIndex);
        int selectionEnd = qMin(m_userCursorEnd-1, endSampleIndex);
        if (selectionStart < selectionEnd)
        {
            int x = selectionStart / m_scene->samplesPerPixel();
            int width = (selectionEnd - selectionStart) / m_scene->samplesPerPixel() + 1;
            QRect selectionRect(x, rect.y(), width, rect.height());
            painter->fillRect(selectionRect, Qt::lightGray);
        }
    }

    // Draw waves
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

    // Draw cursors
    if ((startSampleIndex <= m_userCursor) && (m_userCursor < endSampleIndex))
    {
        QPen pen(Qt::red);
        painter->setPen(pen);
        int x = m_scene->sampleToPixelX(m_userCursor);
        painter->drawLine(x, 0, x, m_size.height()-1);
    }
    if (m_userCursor != m_userCursorEnd)
    {
        if ((startSampleIndex <= m_userCursorEnd) && (m_userCursorEnd < endSampleIndex))
        {
            QPen pen(Qt::red);
            painter->setPen(pen);
            int x = m_scene->sampleToPixelX(m_userCursorEnd);
            painter->drawLine(x, 0, x, m_size.height()-1);
        }
    }
    if ((startSampleIndex <= m_playerCursor) && (m_playerCursor < endSampleIndex))
    {
        QPen pen(Qt::black);
        painter->setPen(pen);
        int x = m_scene->sampleToPixelX(m_playerCursor);
        painter->drawLine(x, 0, x, m_size.height()-1);
    }
}


void AudioWaveItem::audioDataChanged()
{
    if (!m_scene->audioData()) return;

    m_userCursor = 0;
    m_userCursorEnd = 0;
    m_playerCursor = 0;
    m_audioWaveData.setAudioData(m_scene->audioData());

    prepareGeometryChange();
    m_size.setWidth(m_scene->totalTrackWidthInPixels());
    update();
}

void AudioWaveItem::samplesPerPixelChanged()
{
    prepareGeometryChange();
    m_size.setWidth(m_scene->totalTrackWidthInPixels());
    m_sampleAvgCache[0].clear();
    m_sampleAvgCache[1].clear();
}

void AudioWaveItem::setPlayerCursor(int sampleIndex)
{
    m_playerCursor = sampleIndex;
    update();
}

void AudioWaveItem::drawWave(QPainter *painter, const QRect &rect, int channel)
{
    //qDebug() << "drawWave: " << rect;
    QPen pen(Qt::darkGreen);
    painter->setPen(pen);
    for (int i = 0; i < rect.width(); i++)
    {
        int sampleIndex = (i + rect.x()) * m_scene->samplesPerPixel();
        /*bool isSelected = (m_userCursor < sampleIndex) && (sampleIndex < m_userCursorEnd);
        if (isSelected)
        {
            QPen pen2(Qt::lightGray);
            painter->setPen(pen2);
            painter->drawLine(rect.x()+i, rect.y(), rect.x()+i, rect.y()+rect.height()-1);
            painter->setPen(pen);
        }*/


        float avg = m_audioWaveData.getAverage(channel, sampleIndex, m_scene->samplesPerPixel());
        int yc = rect.height() / 2;
        int y1 = yc - (avg * rect.height() / 2);
        int y2 = yc + (avg * rect.height() / 2);
        painter->drawLine(rect.x()+i, rect.y()+y1, rect.x()+i, rect.y()+y2);
    }
}

void AudioWaveItem::updateUserCursor(int sampleIndexFirst, int sampleIndexLast)
{
    if (sampleIndexFirst != m_userCursor)
    {
        m_scene->userCursorChanged(sampleIndexFirst);
    }
    if (sampleIndexLast != m_userCursorEnd)
    {
        if (sampleIndexFirst == sampleIndexLast)
        {
            m_scene->userSelectionCleared();
        }
        else
        {
            m_scene->userSelectionChanged(sampleIndexFirst, sampleIndexLast);
        }
    }
    m_userCursor = sampleIndexFirst;
    m_userCursorEnd = sampleIndexLast;
    update();
}

void AudioWaveItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        int sampleId = m_scene->pixelXToSample(event->pos().x());
        updateUserCursor(sampleId, sampleId);
    }
}

void AudioWaveItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        int sampleIndexFirst = m_scene->pixelXToSample(event->pos().x());
        int sampleIndexLast = m_scene->pixelXToSample(event->buttonDownPos(Qt::LeftButton).x());

        if (sampleIndexLast < sampleIndexFirst)
        {
            qSwap(sampleIndexFirst, sampleIndexLast);
        }
        updateUserCursor(sampleIndexFirst, sampleIndexLast);
    }
}

void AudioWaveItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    m_scene->processMouseWheelEvent(event);
}
