#include "grid.h"

// The constructor is empty, so you could remove the definition.
Grid::Grid() {} // This is not necessary

// If you made snapToGrid static:
QPointF Grid::snapToGrid(const QPointF &point, qreal gridSize)
{
    qreal x = qRound(point.x() / gridSize) * gridSize;
    qreal y = qRound(point.y() / gridSize) * gridSize;
    return QPointF(x, y);
}
