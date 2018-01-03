#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

class SplitterItem;

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    Scene();
    void setViewSize(const QSize& size);

    void updateLayout();

private:
    QSize m_viewSize;
    QGraphicsRectItem *m_rollItem;
    QGraphicsRectItem *m_waveItem;
    SplitterItem *m_splitter;



};

#endif // SCENE_H
