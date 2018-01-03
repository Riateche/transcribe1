#ifndef SPLITTERITEM_H
#define SPLITTERITEM_H

#include <QGraphicsRectItem>

class SplitterItem : public QGraphicsRectItem
{
public:
    SplitterItem();

    // QGraphicsItem interface
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
};

#endif // SPLITTERITEM_H
