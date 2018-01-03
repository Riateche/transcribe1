#include "SplitterItem.h"
#include <QPen>
#include <QBrush>
#include <QTimer>
#include "Scene.h"

SplitterItem::SplitterItem()
{
    setPen(Qt::NoPen);
    setBrush(QBrush(Qt::green));
    setFlags(flags() | ItemIsMovable | ItemSendsScenePositionChanges);


}


QVariant SplitterItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (scene()) {
        Scene *s = static_cast<Scene*>(scene());
        QTimer::singleShot(0, s, &Scene::updateLayout);
        QPointF pos = value.toPointF();
        return QPointF(0, pos.y());
    }
    return QGraphicsRectItem::itemChange(change, value);
}
