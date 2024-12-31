#include "grid.h"

Grid::Grid() {}

QPointF Grid::snapToGrid(const QPointF &point, qreal gridSize)
{
    // Approximates the current point to the next multiple of gridSize in pixels
    qreal x = qRound(point.x() / gridSize) * gridSize;
    qreal y = qRound(point.y() / gridSize) * gridSize;
    return QPointF(x, y);
}
