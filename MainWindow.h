#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class SoundDevice;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_test1_toggled(bool checked);

    void on_openFile_triggered();

    void on_start_clicked();

    void on_stop_clicked();

    void on_togglePause_toggled(bool checked);

    void on_goToStart_clicked();

private:
    Ui::MainWindow *ui;
    SoundDevice *m_soundDevice;
};

#endif // MAINWINDOW_H
