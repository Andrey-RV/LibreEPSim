#include "mygraphicsview.h"
#include <QMouseEvent>

MyGraphicsView::MyGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
    setMouseTracking(true);
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    emit mouseMoved(scenePos);
    QGraphicsView::mouseMoveEvent(event);
}

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    emit mousePressed(scenePos);
    QGraphicsView::mousePressEvent(event);
}

void MyGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    emit mouseDoubleClicked(scenePos);
    QGraphicsView::mouseDoubleClickEvent(event);
}
