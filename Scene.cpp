#include "Scene.h"
#include "SplitterItem.h"

#define MAGIC_X 500

Scene::Scene()
{

    m_rollItem = addRect(QRectF(0, 0, MAGIC_X, 20), Qt::NoPen, QBrush(Qt::red));
    m_waveItem = addRect(QRectF(0, 0, MAGIC_X, 20), Qt::NoPen, QBrush(Qt::blue));
    m_waveItem->setPos(0, 30);
    m_splitter = new SplitterItem();
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

void Scene::updateLayout()
{
    const int MIN_SIZE = 10;
    if (!m_viewSize.isValid()) {
        return;
    }
    setSceneRect(QRectF(QPointF(), m_viewSize));
    if (m_splitter->pos().y() < MIN_SIZE) {
        m_splitter->setPos(0, MIN_SIZE);
    }
    if (m_splitter->pos().y() + m_splitter->boundingRect().height() >=
            m_viewSize.height() - MIN_SIZE) {
        m_splitter->setPos(0, m_viewSize.height() - MIN_SIZE - 1 - m_splitter->boundingRect().height());
    }

    m_rollItem->setRect(0, 0, MAGIC_X, m_splitter->pos().y());
    m_waveItem->setPos(0, m_splitter->pos().y() + m_splitter->boundingRect().height());
    m_waveItem->setRect(0, 0, MAGIC_X, m_viewSize.height() - m_waveItem->pos().y());
}
