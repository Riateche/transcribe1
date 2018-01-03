#ifndef AUDIOWAVEITEM_H
#define AUDIOWAVEITEM_H

#include <QGraphicsItem>

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
    int m_playerCursor;
    float m_amplitudeScale;
    QSize m_size;
    Scene *m_scene;



    void drawWave(QPainter *painter, const QRect& rect, int channel);
    void drawUserCursor();


    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif // AUDIOWAVEITEM_H
