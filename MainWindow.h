#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class SoundDevice;
class Scene;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadFile(const QString& path);

private slots:
    void playerDataChanged();
    void playerStateChanged(bool isPlaying);
    void playerPositionChanged(int sampleId);

private slots:
    void on_test1_toggled(bool checked);

    void on_openFile_triggered();

    void on_play_toggled(bool checked);

    void on_stop_clicked();

    void on_pause_toggled(bool checked);

    void on_goToStart_clicked();

    void on_speed25_toggled(bool checked);

    void on_speed50_toggled(bool checked);

    void on_speed100_toggled(bool checked);

    void on_position_sliderMoved(int position);

    void on_horizontalScrollBar_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    SoundDevice *m_soundDevice;
    Scene *m_scene;
    void updateHoritontalScrollBarRange();

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // MAINWINDOW_H
