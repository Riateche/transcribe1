#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "soundtouch/SoundTouch.h"
#include "AudioFile.h"
#include <QAudioOutput>
#include <QBuffer>
#include <QDebug>
#include <QFileDialog>
#include "SoundDevice.h"

using namespace soundtouch;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_soundDevice = new SoundDevice();
    auto args = qApp->arguments();
    if (args.size() > 1)
    {
        m_soundDevice->loadFile(args[1]);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_test1_toggled(bool checked)
{
    m_soundDevice->setPlaying(checked);
}

void MainWindow::on_openFile_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this,
        tr("Open sound file"), "", tr("Cool sounds (*.wav)"));
    if (fileName.isEmpty())
    {
        return;
    }
    m_soundDevice->loadFile(fileName);
}
