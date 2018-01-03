#include "SplitterItem.h"
#include <QPen>
#include <QBrush>
#include <QTimer>
#include <QCursor>
#include "Scene.h"
#include <QDebug>

SplitterItem::SplitterItem(Scene *scene)
{
    m_scene = scene;
    setPen(Qt::NoPen);
    setBrush(QBrush(Qt::green));
    setFlags(flags() | ItemIsMovable | ItemSendsScenePositionChanges);
    setCursor(Qt::SizeVerCursor);

}


QVariant SplitterItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange) {
        const int MIN_SIZE = 10;
        if (m_scene) {
            QTimer::singleShot(0, m_scene, &Scene::updateLayout);
            QPointF pos = value.toPointF();
            pos.setX(0);
            if (m_scene->viewSize().isValid()) {
                if (pos.y() < MIN_SIZE) {
                    pos.setY(MIN_SIZE);
                }
                if (pos.y() + boundingRect().height() >= m_scene->viewSize().height() - MIN_SIZE) {
                    pos.setY(m_scene->viewSize().height() - MIN_SIZE - 1 - boundingRect().height());
                }
            }
            qDebug() << pos;
            return pos;
        }
    }
    return QGraphicsRectItem::itemChange(change, value);
}
