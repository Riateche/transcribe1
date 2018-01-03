#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

class SplitterItem;
class AudioWaveItem;

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    Scene();
    void setViewSize(const QSize& size);
    QSize viewSize() const;

    void updateLayout();

    AudioWaveItem *waveItem();



private:
    QSize m_viewSize;
    QGraphicsRectItem *m_rollItem;
    AudioWaveItem *m_waveItem;
    SplitterItem *m_splitter;



};

#endif // SCENE_H
