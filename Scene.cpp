#include "Scene.h"
#include "SplitterItem.h"
#include "AudioWaveItem.h"
#include "AudioData.h"
#define MAGIC_X 500
#include <qmath.h>
#include <QDebug>
#include <QGraphicsSceneWheelEvent>

Scene::Scene()
{
    m_samplesPerPixel = 1;
    m_horizontalScrollBar = nullptr;
    m_audioData = nullptr;

    m_scrollContainer = addRect(QRectF());
    m_rollItem = new QGraphicsRectItem(QRectF(0, 0, MAGIC_X, 20));
    m_rollItem->setPen(Qt::NoPen);
    m_rollItem->setBrush(QBrush(Qt::red));
    m_rollItem->setParentItem(m_scrollContainer);
    m_waveItem = new AudioWaveItem(this);
    m_waveItem->setParentItem(m_scrollContainer);
    m_splitter = new SplitterItem(this);
    m_splitter->setParentItem(m_scrollContainer);
    m_splitter->setRect(QRectF(0, 0, MAGIC_X, 10));
    m_splitter->setPos(0, 20);
    setDefaultSecondsPerPixel();
    updateLayout();
}

void Scene::setViewSize(const QSize &size)
{
    m_viewSize = size;
    updateLayout();
    updateHorizontalScrollBarRange();
}

QSize Scene::viewSize() const
{
    return m_viewSize;
}


void Scene::updateLayout()
{

    if (!m_viewSize.isValid()) {
        return;
    }
    setSceneRect(QRectF(QPointF(), m_viewSize));
    if (m_splitter->boundingRect().width() != m_waveItem->boundingRect().width()) {
        QRectF rect = m_splitter->rect();
        rect.setWidth(m_waveItem->boundingRect().width());
        m_splitter->setRect(rect);
    }

    m_rollItem->setRect(0, 0, MAGIC_X, m_splitter->pos().y());
    m_waveItem->setPos(0, m_splitter->pos().y() + m_splitter->boundingRect().height());
    m_waveItem->setHeight(m_viewSize.height() - m_waveItem->pos().y());
}

AudioWaveItem *Scene::waveItem()
{
    return m_waveItem;
}

void Scene::setAudioData(const AudioData *audioData)
{
    m_audioData = audioData;
    emit audioDataChanged();
    m_secondsPerPixel = -1; // force update
    setDefaultSecondsPerPixel();
    updateHorizontalScrollBarRange();
}

const AudioData *Scene::audioData()
{
    return m_audioData;
}

QScrollBar *Scene::horizontalScrollBar() const
{
    return m_horizontalScrollBar;
}

void Scene::setHorizontalScrollBar(QScrollBar *horizontalScrollBar)
{
    m_horizontalScrollBar = horizontalScrollBar;
    connect(m_horizontalScrollBar, &QScrollBar::valueChanged,
            this, &Scene::horizontalScrollBarValueChanged);
    updateHorizontalScrollBarRange();
}

int Scene::totalTrackWidthInPixels()
{
    if (!m_audioData) {
        return 0;
    }
    return qCeil(float(m_audioData->numSamples()) / m_samplesPerPixel);
}

//void Scene::setSamplesPerPixel(int v)
//{
//}

int Scene::samplesPerPixel()
{
    return m_samplesPerPixel;
}

void Scene::setSecondsPerPixel(float v, int scalePoint)
{
    if (m_secondsPerPixel == v) {
        return;
    }

    if (scalePoint == -1)
    {
        int center = -m_scrollContainer->pos().x() + m_viewSize.width() / 2;
        scalePoint = pixelXToSample(center);
    }
    float scalePointFraction = (sampleToPixelX(scalePoint) + m_scrollContainer->pos().x()) / (float)m_viewSize.width();

    m_secondsPerPixel = v;
    if (m_audioData) {
        m_samplesPerPixel = qRound(v * m_audioData->sampleRate());
        if (m_samplesPerPixel < 1)
        {
            m_samplesPerPixel = 1;
        }
    } else {
        m_samplesPerPixel = 1;
    }
    updateHorizontalScrollBarRange();
    emit samplesPerPixelChanged();
    qDebug() << "test" << m_secondsPerPixel << m_samplesPerPixel;

    int scalePointX = sampleToPixelX(scalePoint);
    int viewportX = m_viewSize.width() * scalePointFraction;
    setHorizontalScrollValue(scalePointX - viewportX);
}

float Scene::secondsPerPixel()
{
    return m_secondsPerPixel;
}

void Scene::setDefaultSecondsPerPixel()
{
    setSecondsPerPixel(0.01);
}

void Scene::fitTrackHorizontally()
{
    if (!m_audioData || !m_viewSize.isValid()) return;
    setSecondsPerPixel(float(m_audioData->numSamples()) / m_audioData->sampleRate() / m_viewSize.width());
}

int Scene::pixelXToSample(int pixelX)
{
    return m_samplesPerPixel * pixelX;
}

int Scene::sampleToPixelX(int sample)
{
    return qFloor(float(sample) / m_samplesPerPixel);
}

void Scene::setHorizontalScrollValue(int pixelOffset)
{
    if (pixelOffset > (totalTrackWidthInPixels() - m_viewSize.width()))
    {
        pixelOffset = totalTrackWidthInPixels() - m_viewSize.width();
    }
    if (pixelOffset < 0)
    {
        pixelOffset = 0;
    }
    m_scrollContainer->setPos(-pixelOffset, 0);

    if (m_horizontalScrollBar != nullptr)
    {
        m_horizontalScrollBar->setValue(pixelOffset);
    }
}

int Scene::horizontalScrollValue() const
{
    return -m_scrollContainer->pos().x();
}

void Scene::processMouseWheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        int scalePoint = pixelXToSample(event->pos().x());
        float multiplier = qExp(-event->delta() * 0.005);
        setSecondsPerPixel(secondsPerPixel() * multiplier, scalePoint);
    }
    if (event->modifiers() & Qt::ShiftModifier)
    {
        int offset = qFloor((event->delta() / 120.0) * (m_viewSize.width() / 10.0));
        setHorizontalScrollValue(horizontalScrollValue() - offset);
    }
}

void Scene::updateHorizontalScrollBarRange()
{
    if (!m_horizontalScrollBar || !m_viewSize.isValid()) {
        return;
    }
    m_horizontalScrollBar->setRange(0,
      totalTrackWidthInPixels() - m_viewSize.width());
    horizontalScrollBarValueChanged();
}

void Scene::horizontalScrollBarValueChanged()
{
    if (!m_horizontalScrollBar) {
        return;
    }
    setHorizontalScrollValue(m_horizontalScrollBar->value());
}
