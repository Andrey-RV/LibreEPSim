#ifndef GRID_H
#define GRID_H

#include <QPointF>

class Grid
{
public:
    Grid();
    static constexpr qreal GRID_SIZE = 7.0;
    QPointF snapToGrid(const QPointF& point, qreal gridSize);
};

#endif // GRID_H
