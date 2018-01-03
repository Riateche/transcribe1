#include "Scene.h"
#include "SplitterItem.h"
#include "AudioWaveItem.h"

#define MAGIC_X 500

Scene::Scene()
{

    m_rollItem = addRect(QRectF(0, 0, MAGIC_X, 20), Qt::NoPen, QBrush(Qt::red));
    m_waveItem = new AudioWaveItem();
    addItem(m_waveItem);
    m_splitter = new SplitterItem(this);
    m_splitter->setRect(QRectF(0, 0, MAGIC_X, 10));
    m_splitter->setPos(0, 20);
    addItem(m_splitter);
    updateLayout();
}

void Scene::setViewSize(const QSize &size)
{
    m_viewSize = size;
    updateLayout();
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
