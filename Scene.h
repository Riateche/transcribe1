#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QScrollBar>

class SplitterItem;
class AudioWaveItem;
class AudioData;

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    Scene();
    void setViewSize(const QSize& size);
    QSize viewSize() const;

    void updateLayout();

    AudioWaveItem *waveItem();

    void setAudioData(const AudioData *audioData);
    const AudioData *audioData();



    QScrollBar *horizontalScrollBar() const;
    void setHorizontalScrollBar(QScrollBar *horizontalScrollBar);

    int totalTrackWidthInPixels();
    //void setSamplesPerPixel(int v);
    int samplesPerPixel();
    void setSecondsPerPixel(float v);
    float secondsPerPixel();

    void setDefaultSecondsPerPixel();
    void fitTrackHorizontally();

    int pixelXToSample(int pixelX);
    int sampleToPixelX(int sample);

signals:
    void audioDataChanged();
    void samplesPerPixelChanged();
    void userCursorChanged(int sampleIndex);

private:
    QSize m_viewSize;
    QGraphicsRectItem *m_scrollContainer;
    QGraphicsRectItem *m_rollItem;
    AudioWaveItem *m_waveItem;
    SplitterItem *m_splitter;
    int m_samplesPerPixel;
    float m_secondsPerPixel;
    QScrollBar *m_horizontalScrollBar;
    const AudioData* m_audioData;



    void updateHorizontalScrollBarRange();

private slots:
    void horizontalScrollBarValueChanged();
};

#endif // SCENE_H
