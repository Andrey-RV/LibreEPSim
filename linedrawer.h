#ifndef LINEDRAWER_H
#define LINEDRAWER_H

#include <QGraphicsScene>
#include <QPointF>
#include "grid.h"
#include "componentmanager.h"

class LineDrawer
{
public:
    LineDrawer();
    void startDrawing();
    void changeLineDirection(QGraphicsScene* graphicsScene, const QPointF &scenePos);
    void continueLineDrawing(const QPointF &scenePos);
    QPointF findNearestTerminal(const QPointF &point, bool &snapped, QList<ComponentManager::Component> components);
    void finalizeLine(const QPointF &scenePos, QList<ComponentManager::Component> components);
    void cancelDrawing(QGraphicsScene* graphicsScene);

    bool getLineDrawing() const;
    QGraphicsLineItem *getCurrentLine() const;

private:
    Grid grid;
    bool lineDrawing = false;
    QGraphicsLineItem* currentLine = nullptr;
    QPointF startPoint;
    Qt::Orientation constraintDirection = static_cast<Qt::Orientation>(-1);
    static constexpr qreal SNAP_THRESHOLD = 15.0;
};

#endif // LINEDRAWER_H
