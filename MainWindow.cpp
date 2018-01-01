#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "soundtouch/SoundTouch.h"
#include "AudioFile.h"
#include <QAudioOutput>
#include <QBuffer>
#include <QDebug>
#include "SoundDevice.h"

using namespace soundtouch;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //auto soundTouch = new SoundTouch();
    //soundTouch->

    //qDebug() << audioFile->samples[0][100] << audioFile->samples[0][101] << audioFile->samples[0][102] << audioFile->samples[0][103];
    m_soundDevice = new SoundDevice();
    m_soundDevice->loadFile("/home/ri/tmp/1.wav");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_test1_toggled(bool checked)
{
    m_soundDevice->setPlaying(checked);
}
