#ifndef GRID_H
#define GRID_H

#include <QPointF>

class Grid
{
public:
    // I suggest making the constructor explicit.
    explicit Grid();

    // Since GRID_SIZE is static, you might consider making it a static member function
    // or moving it outside the class as a free-standing constant.
    static constexpr qreal GRID_SIZE = 7.0;

    // snapToGrid could be a static function since it doesn't depend on the state of a Grid object.
    static QPointF snapToGrid(const QPointF& point, qreal gridSize);
};

#endif // GRID_H
