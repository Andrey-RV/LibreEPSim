#include "mainwindow.h"
#include "mygraphicsview.h"
#include "./ui_mainwindow.h"
#include "linedrawer.h"
#include "componentmanager.h"
#include <QMouseEvent>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolBar->setFloatable(false);
    ui->toolBar->setMovable(false);
    // showMaximized();

    graphicsView = std::make_shared<MyGraphicsView>(this); // Create as shared_ptr
    graphicsScene = std::make_shared<QGraphicsScene>(this);
    graphicsView->setScene(graphicsScene.get());
    graphicsView->setStyleSheet("background-color: white;");
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    lineDrawer = std::make_unique<LineDrawer>(graphicsScene);
    componentManager = std::make_unique<ComponentManager>(graphicsView, graphicsScene);

    setCentralWidget(graphicsView.get());

    componentManager->setMoveTimer(new QTimer(this)); // QTimer is managed by Qt's object hierarchy
    componentManager->getMoveTimer()->setInterval(componentManager->TIMER_INTERVAL); // 60 FPS timer for smooth movement

    connect(graphicsView.get(), &MyGraphicsView::mouseMoved, this, &MainWindow::onMouseMoved);
    connect(graphicsView.get(), &MyGraphicsView::mousePressed, this, &MainWindow::onMousePressed);
    connect(graphicsView.get(), &MyGraphicsView::mouseDoubleClicked, this, &MainWindow::onMouseDoubleClicked);
    connect(componentManager->getMoveTimer(), &QTimer::timeout, componentManager.get(), &ComponentManager::updateImagePosition);
}

MainWindow::~MainWindow()
{
    // Smart pointers will automatically handle deletion
}

void MainWindow::on_actiongen_triggered()
{
    componentManager->startComponentPlacement(":Icons/gen.png");
}

void MainWindow::on_actionbus_triggered()
{
    componentManager->startComponentPlacement(":Icons/bus2.png");
}

void MainWindow::on_actiontrafo_triggered()
{
    componentManager->startComponentPlacement(":Icons/trafo2.png");
}

void MainWindow::on_actionline_triggered()
{
    lineDrawer->startDrawing();
}

void MainWindow::onMousePressed(const QPointF &scenePos)
{
    if (lineDrawer->getLineDrawing()) {
        lineDrawer->changeLineDirection(scenePos);
    }

    if (componentManager->getComponentIsMoving() && componentManager->getCurrentComponent()) {
        componentManager->finalizeComponentPlacement();
    }
}

void MainWindow::onMouseMoved(const QPointF &scenePos)
{
    static QElapsedTimer lastUpdate;
    if (lastUpdate.isValid() && lastUpdate.elapsed() < 16) { // ~60 FPS
        return;
    }
    lastUpdate.restart();

    if (lineDrawer->getLineDrawing() && lineDrawer->getCurrentLine()) {
        lineDrawer->continueLineDrawing(scenePos);
    }
}

void MainWindow::onMouseDoubleClicked(const QPointF &scenePos)
{
    if (lineDrawer->getLineDrawing() && lineDrawer->getCurrentLine()) {
        lineDrawer->finalizeLine(scenePos, componentManager->getComponents());
    }
}

void MainWindow::zoomIn()
{
    const double zoomStep = ZOOM_STEP; // 20% zoom increment
    if (currentZoomFactor * zoomStep <= MAX_ZOOM) {
        graphicsView->scale(zoomStep, zoomStep);
        currentZoomFactor *= zoomStep;
    }
}

void MainWindow::zoomOut()
{
    const double zoomStep = ZOOM_STEP; // 20% zoom decrement
    if (currentZoomFactor / zoomStep >= MIN_ZOOM) {
        graphicsView->scale(1 / zoomStep, 1 / zoomStep);
        currentZoomFactor /= zoomStep;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_Plus) {
        zoomIn();
    }
    else if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_Minus) {
        zoomOut();
    }
    else {
        switch (event->key()) {
        case Qt::Key_G:
            MainWindow::on_actiongen_triggered();
            break;
        case Qt::Key_B:
            MainWindow::on_actionbus_triggered();
            break;
        case Qt::Key_T:
            MainWindow::on_actiontrafo_triggered();
            break;
        case Qt::Key_L:
            MainWindow::on_actionline_triggered();
            break;
        case Qt::Key_Escape:
            if (lineDrawer->getCurrentLine()) {
                lineDrawer->cancelDrawing();
            }
            if (componentManager->getCurrentComponent()) {
                graphicsScene->removeItem(componentManager->getCurrentComponent().get());
                // getCurrentComponent doesn't return a smart pointer, so we need to manage the memory depending on the implementation of componentManager
                // delete componentManager->getCurrentComponent();
                // componentManager->setCurrentComponent(nullptr);
                componentManager->setCurrentComponent(nullptr);
                componentManager->setComponentIsMoving(false);
                unsetCursor();
            }
            break;
        default:
            break;
        }
    }
}
