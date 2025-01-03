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

    graphicsView = std::make_shared<MyGraphicsView>(this);
    graphicsScene = std::make_shared<QGraphicsScene>(this);
    graphicsView->setScene(graphicsScene.get());
    graphicsView->setStyleSheet("background-color: white;");
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    lineDrawer = std::make_unique<LineDrawer>(graphicsScene);
    componentManager = std::make_unique<ComponentManager>(graphicsView, graphicsScene);

    setCentralWidget(graphicsView.get());

    componentManager->setMoveTimer(new QTimer(this));
    componentManager->getMoveTimer()->setInterval(componentManager->TIMER_INTERVAL);

    connect(graphicsView.get(), &MyGraphicsView::mouseMoved, this, &MainWindow::onMouseMoved);
    connect(graphicsView.get(), &MyGraphicsView::mousePressed, this, &MainWindow::onMousePressed);
    connect(graphicsView.get(), &MyGraphicsView::mouseDoubleClicked, this, &MainWindow::onMouseDoubleClicked);
    connect(componentManager->getMoveTimer(), &QTimer::timeout, componentManager.get(), &ComponentManager::updateImagePosition);
}

MainWindow::~MainWindow() {}

void MainWindow::on_actiongen_triggered()
{
    componentManager->startComponentPlacement(":Icons/gen.png", QPixmap(), nullptr);
}

void MainWindow::on_actionbus_triggered()
{
    componentManager->startComponentPlacement(":Icons/bus2.png", QPixmap(), nullptr);
}

void MainWindow::on_actiontrafo_triggered()
{
    componentManager->startComponentPlacement(":Icons/trafo2.png", QPixmap(), nullptr);
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

    if (isDeletionMode) {
        QGraphicsItem *item = graphicsView->itemAt(graphicsView->mapFromScene(scenePos));

        if (item) {
            if (auto pixmapItem = dynamic_cast<QGraphicsPixmapItem*>(item)) {
                componentManager->removeComponent(pixmapItem);

            } else if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
                graphicsScene->removeItem(lineItem);
                delete lineItem;
            }
        }
    }

    if (isMovingMode) {
        QGraphicsItem *item = graphicsView->itemAt(graphicsView->mapFromScene(scenePos));

        if (item) {
            if (auto pixmapItem = dynamic_cast<QGraphicsPixmapItem*>(item)) {
                QPixmap pixmap = pixmapItem->pixmap();
                std::shared_ptr<QGraphicsPixmapItem> sharedPixmapItem(pixmapItem);
                isMovingMode = false;
                componentManager->startComponentPlacement("", pixmap, sharedPixmapItem);

            } else if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
                // graphicsScene->startLineMoving(lineItem);
            }
        }
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
    const qreal zoomStep = ZOOM_STEP; // 20% zoom increment
    if (currentZoomFactor * zoomStep <= MAX_ZOOM) {
        graphicsView->scale(zoomStep, zoomStep);
        currentZoomFactor *= zoomStep;
    }
}

void MainWindow::zoomOut()
{
    const qreal zoomStep = ZOOM_STEP; // 20% zoom decrement
    if (currentZoomFactor / zoomStep >= MIN_ZOOM) {
        graphicsView->scale(1 / zoomStep, 1 / zoomStep);
        currentZoomFactor /= zoomStep;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_Plus:
            zoomIn();
            break;
        case Qt::Key_Minus:
            zoomOut();
            break;
        default:
            break;
        }
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
        case Qt::Key_M:
            isMovingMode = true;
            graphicsView->setCursor(Qt::OpenHandCursor);
            break;
        case Qt::Key_Escape:
            if (lineDrawer->getCurrentLine()) {
                lineDrawer->cancelDrawing();
            }
            if (componentManager->getCurrentComponent()) {
                graphicsScene->removeItem(componentManager->getCurrentComponent().get());
                componentManager->setCurrentComponent(nullptr);
                componentManager->setComponentIsMoving(false);
                graphicsView->unsetCursor();
            }
            if (isDeletionMode) {
                isDeletionMode = false;
                graphicsView->unsetCursor();
            }
            if (isMovingMode) {
                isMovingMode = false;
                graphicsView->unsetCursor();
            }
            break;
        case Qt::Key_Delete:
            isDeletionMode = true;
            graphicsView->setCursor(Qt::CrossCursor);
            break;
        default:
            break;
        }
    }
}
