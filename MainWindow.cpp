#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "soundtouch/SoundTouch.h"
#include "AudioFile.h"
#include <QAudioOutput>
#include <QBuffer>
#include <QDebug>
#include <QFileDialog>
#include "SoundDevice.h"
#include "Scene.h"
#include "AudioWaveItem.h"

using namespace soundtouch;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->installEventFilter(this);
    m_scene = new Scene();
    ui->graphicsView->setScene(m_scene);
    ui->graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_soundDevice = new SoundDevice();
    auto args = qApp->arguments();
    if (args.size() > 1)
    {
        loadFile(args[1]);
    }
    show();
    m_scene->setViewSize(ui->graphicsView->size());

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFile(const QString &path)
{
    m_soundDevice->loadFile(path);
    m_scene->waveItem()->setAudioData(m_soundDevice->audioData());
    updateHoritontalScrollBarRange();
}

void MainWindow::on_test1_toggled(bool checked)
{
    if (checked) {
        m_soundDevice->start();
    } else {
        m_soundDevice->stop();
    }
}

void MainWindow::on_openFile_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this,
        tr("Open sound file"), "", tr("Cool sounds (*.wav)"));
    if (fileName.isEmpty())
    {
        return;
    }
    loadFile(fileName);
}

void MainWindow::on_play_clicked()
{
    m_soundDevice->start();
}

void MainWindow::on_stop_clicked()
{
    m_soundDevice->stop();
}

void MainWindow::on_pause_toggled(bool checked)
{
    if (checked) {
        m_soundDevice->pause();
    } else {
        m_soundDevice->resume();
    }
}

void MainWindow::on_goToStart_clicked()
{
    //...
}

void MainWindow::on_speed25_toggled(bool checked)
{
    if (checked) m_soundDevice->setTempo(0.25);
}

void MainWindow::on_speed50_toggled(bool checked)
{
    if (checked) m_soundDevice->setTempo(0.5);
}

void MainWindow::on_speed100_toggled(bool checked)
{
    if (checked) m_soundDevice->setTempo(1.0);
}


bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->graphicsView && event->type() == QEvent::Resize) {
        m_scene->setViewSize(ui->graphicsView->size());
        updateHoritontalScrollBarRange();
    }
    return false;
}

void MainWindow::on_horizontalScrollBar_valueChanged(int value)
{
    m_scene->setHoritontalScrollPos(value);
}

void MainWindow::updateHoritontalScrollBarRange()
{
    // TODO: make a class that keeps track of horizontal scale
    ui->horizontalScrollBar->setRange(0,
      m_scene->waveItem()->boundingRect().width() - ui->graphicsView->width());
}
