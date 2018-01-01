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

private:
    Ui::MainWindow *ui;
    SoundDevice *m_soundDevice;
};

#endif // MAINWINDOW_H
