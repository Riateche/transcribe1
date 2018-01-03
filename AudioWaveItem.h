#ifndef AUDIOWAVEITEM_H
#define AUDIOWAVEITEM_H

#include <QGraphicsItem>

class AudioData;

class AudioWaveItem : public QGraphicsItem
{
public:
    AudioWaveItem();
    void setHeight(int height);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setAudioData(const AudioData *audioData);
    void setPlayerCursor(int sampleIndex);

private:
    const AudioData* m_audioData;
    int m_userCursor;
    int m_playerCursor;
    float m_amplitudeScale;
    int m_samplesPerPixel;
    QSize m_size;

    void drawWave(QPainter *painter, const QRect& rect, int channel);
    void drawUserCursor();


    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif // AUDIOWAVEITEM_H
