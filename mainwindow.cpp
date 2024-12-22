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
    createComponent(":Icons/gen.png");
}

void MainWindow::on_actionbus_triggered()
{
    createComponent(":Icons/bus2.png");
}

void MainWindow::createComponent(const QString& imagePath)
{
    if (!currentLabel) {
        currentLabel = new QLabel(this);
        QPixmap component(imagePath);
        currentLabel->setPixmap(component);
        currentLabel->resize(component.size());

        QPoint mousePos = this->mapFromGlobal(QCursor::pos());
        currentLabel->move(mousePos - QPoint(component.width() / 2, component.height() / 2));

        currentLabel->show();

        componentsLabels.append(currentLabel);
        componentIsMoving = true;
        lastMousePos = mousePos;
        setCursor(Qt::BlankCursor);

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
        unsetCursor();
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

