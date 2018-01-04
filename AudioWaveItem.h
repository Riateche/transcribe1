#ifndef AUDIOWAVEITEM_H
#define AUDIOWAVEITEM_H

#include <QGraphicsItem>
#include <QHash>
#include <QPair>

class AudioData;
class Scene;

class AudioWaveItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    AudioWaveItem(Scene *scene);
    void setHeight(int height);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPlayerCursor(int sampleIndex);

private slots:
    void audioDataChanged();
    void samplesPerPixelChanged();

private:
    int m_userCursor;
    int m_userCursorEnd;
    int m_playerCursor;
    float m_amplitudeScale;
    QSize m_size;
    Scene *m_scene;
    QHash<int, QPair<float, float>> m_sampleAvgCache[2];



    void drawWave(QPainter *painter, const QRect& rect, int channel);
    void drawUserCursor();
    QPair<float, float> calculateAvg(int channel, int sampleIndex);
    void updateUserCursor(int sampleIndexFirst, int sampleIndexLast);


    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
};

#endif // AUDIOWAVEITEM_H
