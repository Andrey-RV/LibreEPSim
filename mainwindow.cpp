#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMouseEvent>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    moveTimer = new QTimer(this);
    moveTimer->setInterval(16);  // 60 FPS timer for smooth movement

    // Connect the timer's timeout signal to the updateImagePosition slot
    connect(moveTimer, &QTimer::timeout, this, &MainWindow::updateImagePosition);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actiongen_triggered()
{
    if (!currentLabel) {
        currentLabel = new QLabel(this);
        QPixmap gen(":Icons/gen.png");
        currentLabel->setPixmap(gen);
        currentLabel->resize(gen.size());
        currentLabel->show();

        componentsLabels.append(currentLabel);
        componentIsMoving = true;
        lastMousePos = this->mapFromGlobal(QCursor::pos());

        if (!moveTimer->isActive()) {
            moveTimer->start(10);  // Update every 10 milliseconds
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (componentIsMoving && currentLabel) {
        componentIsMoving = false;

        if (moveTimer->isActive()) {
            moveTimer->stop();
        }

        QPoint mousePos = event->pos();
        currentLabel->move(mousePos - currentLabel->rect().center());

        currentLabel = nullptr;
    }
}

void MainWindow::updateImagePosition()
{
    if (componentIsMoving && currentLabel) {
        QPoint mousePos = this->mapFromGlobal(QCursor::pos());
        currentLabel->move(currentLabel->pos() + (mousePos - lastMousePos));
        lastMousePos = mousePos;
    }
}




