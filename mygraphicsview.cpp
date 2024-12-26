#include "mygraphicsview.h"
#include <QMouseEvent>

MyGraphicsView::MyGraphicsView(QWidget *parent)
    : QGraphicsView(parent)  // Constructor definition
{
    setMouseTracking(true);  // Enable mouse tracking for this view
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    {
        QPointF scenePos = mapToScene(event->pos());
        emit mouseMoved(scenePos); // Emit the signal
        QGraphicsView::mouseMoveEvent(event);
    }
}

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    emit mousePressed(scenePos); // Emit the signal
    QGraphicsView::mousePressEvent(event);
}
