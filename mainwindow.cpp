#include "mainwindow.h"
#include "mygraphicsview.h"
#include "./ui_mainwindow.h"
#include <QMouseEvent>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolBar->setFloatable(false);
    ui->toolBar->setMovable(false);

    graphicsView = new MyGraphicsView();
    graphicsScene = new QGraphicsScene(this);
    graphicsView->setScene(graphicsScene);
    graphicsView->setStyleSheet("background-color: white;");
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCentralWidget(graphicsView);

    moveTimer = new QTimer(this);
    moveTimer->setInterval(16);  // 60 FPS timer for smooth movement

    connect(graphicsView, &MyGraphicsView::mouseMoved, this, &MainWindow::onMouseMoved);
    connect(graphicsView, &MyGraphicsView::mousePressed, this, &MainWindow::onMousePressed);
    connect(graphicsView, &MyGraphicsView::mouseDoubleClicked, this, &MainWindow::onMouseDoubleClicked);
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

void MainWindow::on_actionline_triggered()
{
    lineDrawing = true;
    currentLine = nullptr;
}

void MainWindow::createComponent(const QString& imagePath)
{
    QPixmap component(imagePath);
    QGraphicsPixmapItem *newItem = new QGraphicsPixmapItem(component);
    QPointF mousePos = graphicsView->mapToScene(graphicsView->mapFromGlobal(QCursor::pos()));
    newItem->setPos(mousePos - QPointF(component.width() / 2, component.height() / 2));
    newItem->setTransformationMode(Qt::SmoothTransformation);
    graphicsScene->addItem(newItem);

    currentComponent = newItem;
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
        currentComponent->setPos(mousePos - QPointF(currentComponent->pixmap().width() / 2, currentComponent->pixmap().height() / 2));
    }
}

QPointF MainWindow::findNearestTerminal(const QPointF &point, bool &snapped)
{
    const qreal snapThreshold = 15.0; // Distance within which snapping occurs
    QPointF nearestTerminal;
    qreal minDistance = snapThreshold;
    snapped = false;

    for (const auto &component : components) {
        for (const QPointF &terminal : component.terminals) {
            qreal distance = QLineF(point, terminal).length();
            if (distance < minDistance) {
                minDistance = distance;
                nearestTerminal = terminal;
                snapped = true;
            }
        }
    }
    return nearestTerminal;
}

void MainWindow::onMousePressed(const QPointF &scenePos)
{
    if (lineDrawing) {
        drawNextLine(scenePos);
    }

    if (componentIsMoving && currentComponent) {
        componentIsMoving = false;
        placeComponent();
    }
}

void MainWindow::drawNextLine(const QPointF &scenePos) {
    if (!currentLine) {
        startPoint = scenePos;
        currentLine = new QGraphicsLineItem(QLineF(startPoint, startPoint));
        currentLine->setPen(QPen(Qt::black, 2));
        graphicsScene->addItem(currentLine);
    } else {
        QPointF endPoint = scenePos;
        // Enforce the active constraint
        if (constraintDirection == Qt::Horizontal) {
            endPoint.setY(startPoint.y());  // Horizontal constraint
        } else if (constraintDirection == Qt::Vertical) {
            endPoint.setX(startPoint.x());  // Vertical constraint
        }

        QLineF newLine(startPoint, endPoint);
        currentLine->setLine(newLine);

        // Prepare for the next line
        startPoint = endPoint;
        currentLine = new QGraphicsLineItem(QLineF(startPoint, startPoint));
        currentLine->setPen(QPen(Qt::black, 2));
        graphicsScene->addItem(currentLine);
        constraintDirection = static_cast<Qt::Orientation>(-1);  // Reset to no direction defined
    }
}

void MainWindow::placeComponent() {
    if (moveTimer->isActive()) {
        moveTimer->stop();
    }

    Component newComponent;
    newComponent.item = currentComponent;
    newComponent.terminals = {
        // !TODO: These coordinates only work for the generator. Need to be generalized for other components
        QPointF(30, 3),  // Up terminal
        QPointF(30, 79)   // Down terminal
    };

    for (QPointF &terminal : newComponent.terminals) {
        terminal = currentComponent->mapToScene(terminal);
    }

    components.append(newComponent);
    currentComponent = nullptr;
    unsetCursor();
}

void MainWindow::onMouseMoved(const QPointF &scenePos)
{
    if (lineDrawing && currentLine) {
        QLineF newLine(startPoint, scenePos);

        // Restrict to horizontal or vertical based on initial movement
        if (constraintDirection == static_cast<Qt::Orientation>(-1)) {
            if (qAbs(newLine.dx()) > qAbs(newLine.dy())) {
                constraintDirection = Qt::Horizontal;  // Switch to horizontal
            } else {
                constraintDirection = Qt::Vertical;  // Switch to vertical
            }
        }

        // Enforce the active constraint
        if (constraintDirection == Qt::Horizontal) {
            newLine.setP2(QPointF(scenePos.x(), startPoint.y()));  // Horizontal
        } else if (constraintDirection == Qt::Vertical) {
            newLine.setP2(QPointF(startPoint.x(), scenePos.y()));  // Vertical
        }

        currentLine->setLine(newLine);
    }
}

void MainWindow::onMouseDoubleClicked(const QPointF &scenePos)
{
    if (lineDrawing && currentLine) {
        bool snapped;
        QPointF snapPos = findNearestTerminal(scenePos, snapped);

        QPointF endPoint = scenePos;  // Default to the clicked position

        if (snapped) {
            // Calculate differences for horizontal and vertical alignment
            qreal dx = qAbs(snapPos.x() - startPoint.x());
            qreal dy = qAbs(snapPos.y() - startPoint.y());

            if (constraintDirection == Qt::Horizontal && dy < 1e-6) {
                // Snap only if snapping results in a horizontal line
                endPoint = snapPos;
            } else if (constraintDirection == Qt::Vertical && dx < 1e-6) {
                // Snap only if snapping results in a vertical line
                endPoint = snapPos;
            } else if (constraintDirection == static_cast<Qt::Orientation>(-1)) {
                // If no direction is enforced, determine alignment dynamically
                if (dx < 1e-6) {
                    constraintDirection = Qt::Vertical;
                    endPoint = snapPos;
                } else if (dy < 1e-6) {
                    constraintDirection = Qt::Horizontal;
                    endPoint = snapPos;
                }
            }
        }

        // Enforce the active constraint for non-snapped positions
        if (constraintDirection == Qt::Horizontal) {
            endPoint.setY(startPoint.y());  // Horizontal constraint
        } else if (constraintDirection == Qt::Vertical) {
            endPoint.setX(startPoint.x());  // Vertical constraint
        }

        // Finalize the line
        QLineF newLine(startPoint, endPoint);
        currentLine->setLine(newLine);

        // Reset for the next action
        lineDrawing = false;
        constraintDirection = static_cast<Qt::Orientation>(-1);  // Reset to no direction defined
        currentLine = nullptr;
    }
}

void MainWindow::zoomIn()
{
    const double zoomStep = 1.1; // 20% zoom increment
    if (currentZoomFactor * zoomStep <= maxZoomFactor) {
        graphicsView->scale(zoomStep, zoomStep);
        currentZoomFactor *= zoomStep;
    }
}

void MainWindow::zoomOut()
{
    const double zoomStep = 1.1; // 20% zoom decrement
    if (currentZoomFactor / zoomStep >= minZoomFactor) {
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
}
