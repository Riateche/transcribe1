#ifndef SPLITTERITEM_H
#define SPLITTERITEM_H

#include <QGraphicsRectItem>

class Scene;

class SplitterItem : public QGraphicsRectItem
{
public:
    SplitterItem(Scene *scene);

    // QGraphicsItem interface
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    Scene *m_scene;
};

#endif // SPLITTERITEM_H
