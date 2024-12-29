#include "mainwindow.h"
#include "mygraphicsview.h"
#include "./ui_mainwindow.h"
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
    showMaximized();

    lineDrawer = new LineDrawer();
    newComponent = new ComponentManager();

    graphicsView = new MyGraphicsView();
    graphicsScene = new QGraphicsScene(this);
    graphicsView->setScene(graphicsScene);
    graphicsView->setStyleSheet("background-color: white;");
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCentralWidget(graphicsView);

    moveTimer = new QTimer(this);
    moveTimer->setInterval(TIMER_INTERVAL);  // 60 FPS timer for smooth movement

    connect(graphicsView, &MyGraphicsView::mouseMoved, this, &MainWindow::onMouseMoved);
    connect(graphicsView, &MyGraphicsView::mousePressed, this, &MainWindow::onMousePressed);
    connect(graphicsView, &MyGraphicsView::mouseDoubleClicked, this, &MainWindow::onMouseDoubleClicked);
    connect(moveTimer, &QTimer::timeout, this, &MainWindow::updateImagePosition);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete lineDrawer;
    delete newComponent;
}

void MainWindow::on_actiongen_triggered()
{
    startComponentPlacement(":Icons/gen.png");
}

void MainWindow::on_actionbus_triggered()
{
    startComponentPlacement(":Icons/bus2.png");
}

void MainWindow::on_actiontrafo_triggered()
{
    startComponentPlacement(":Icons/trafo2.png");
}

void MainWindow::on_actionline_triggered()
{
    lineDrawer->startDrawing();
}

void MainWindow::startComponentPlacement(const QString& imagePath)
{
    QPixmap componentPixmap(imagePath);
    if (componentPixmap.isNull()) {
        qWarning() << "Failed to load component image:" << imagePath;
        return;
    }
    currentComponent = new QGraphicsPixmapItem(componentPixmap);

    QPointF mousePos = graphicsView->mapToScene(graphicsView->mapFromGlobal(QCursor::pos()));
    QPointF gridSnappedPos = grid.snapToGrid(mousePos, grid.GRID_SIZE);
    currentComponent->setPos(gridSnappedPos - QPointF(componentPixmap.width() / 2, componentPixmap.height() / 2));
    currentComponent->setTransformationMode(Qt::SmoothTransformation);
    graphicsScene->addItem(currentComponent);

    componentIsMoving = true;
    setCursor(Qt::BlankCursor);

    if (!moveTimer->isActive()) {
        moveTimer->start(10);  // Update every 10 milliseconds
    }
}

void MainWindow::updateImagePosition()
{
    if (componentIsMoving && currentComponent) {
        QPointF mousePos = graphicsView->mapToScene(graphicsView->mapFromGlobal(QCursor::pos()));
        QPointF gridSnappedPos = grid.snapToGrid(mousePos, grid.GRID_SIZE);
        currentComponent->setPos(gridSnappedPos - QPointF(currentComponent->pixmap().width() / 2, currentComponent->pixmap().height() / 2));
    }
}

void MainWindow::finalizeComponentPlacement() {
    componentIsMoving = false;

    if (moveTimer->isActive()) {
        moveTimer->stop();
    }

    newComponent->component.item = currentComponent;
    newComponent->component.terminals = {
        // !TODO: These coordinates only work for the generator. Need to be generalized for other components
        QPointF(30, 3),  // Up terminal
        QPointF(30, 79)   // Down terminal
    };

    for (QPointF &terminal : newComponent->component.terminals) {
        terminal = currentComponent->mapToScene(terminal);
    }

    newComponent->appendComponent(newComponent->component);
    currentComponent = nullptr;
    unsetCursor();
}

void MainWindow::onMousePressed(const QPointF &scenePos)
{
    if (lineDrawer->getLineDrawing()) {
        lineDrawer->changeLineDirection(graphicsScene, scenePos);
    }

    if (componentIsMoving && currentComponent) {
        finalizeComponentPlacement();
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
        lineDrawer->finalizeLine(scenePos, newComponent->getComponents());
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
                lineDrawer->cancelDrawing(graphicsScene);
            }
            if (currentComponent) {
                graphicsScene->removeItem(currentComponent);
                delete currentComponent;
                currentComponent = nullptr;
                componentIsMoving = false;
                unsetCursor();
            }
            break;
        default:
            break;
        }
    }
}
