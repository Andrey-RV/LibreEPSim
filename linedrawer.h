#ifndef LINEDRAWER_H
#define LINEDRAWER_H

#include <QGraphicsScene>
#include <QPointF>
#include <memory> // Include for smart pointers
#include "grid.h"
#include "componentmanager.h"

class LineDrawer {
public:
    LineDrawer(std::shared_ptr<QGraphicsScene> graphicsScene); // Use shared_ptr
    void startDrawing();
    void changeLineDirection(const QPointF &scenePos);
    void continueLineDrawing(const QPointF &scenePos);
    QPointF findNearestTerminal(const QPointF &point, bool &snapped, QList<ComponentManager::Component> components);
    void finalizeLine(const QPointF &scenePos, QList<ComponentManager::Component> components);
    void cancelDrawing();

    bool getLineDrawing() const;
    QGraphicsLineItem *getCurrentLine() const;

private:
    std::shared_ptr<QGraphicsScene> graphicsScene; // Use shared_ptr
    Grid grid;
    bool lineDrawing = false;
    QGraphicsLineItem* currentLine = nullptr; // No need for smart pointer here
    QPointF startPoint;
    Qt::Orientation constraintDirection = static_cast<Qt::Orientation>(-1);
    static constexpr qreal SNAP_THRESHOLD = 15.0;
};

#endif // LINEDRAWER_H
