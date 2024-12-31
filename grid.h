#ifndef GRID_H
#define GRID_H

#include <QPointF>

class Grid
{
public:
    explicit Grid();
    static constexpr qreal GRID_SIZE = 7.0;
    static QPointF snapToGrid(const QPointF& point, qreal gridSize);
};

#endif
