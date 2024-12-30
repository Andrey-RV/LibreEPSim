#include "linedrawer.h"
#include "componentmanager.h"
#include <QGraphicsLineItem>


LineDrawer::LineDrawer(QGraphicsScene* graphicsScene)
    : graphicsScene(graphicsScene)
{}

void LineDrawer::startDrawing()
{
    lineDrawing = true;
    currentLine = nullptr;
}

void LineDrawer::changeLineDirection(const QPointF &scenePos)
{
    if (!currentLine) {
        startPoint = grid.snapToGrid(scenePos, grid.GRID_SIZE);
        currentLine = new QGraphicsLineItem(QLineF(startPoint, startPoint));
        currentLine->setPen(QPen(Qt::black, 2));
        graphicsScene->addItem(currentLine);
    } else {
        QPointF endPoint = grid.snapToGrid(scenePos, grid.GRID_SIZE);;
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

void LineDrawer::continueLineDrawing(const QPointF &scenePos)
{
    QPointF activePos = grid.snapToGrid(scenePos, grid.GRID_SIZE);
    QLineF newLine(startPoint, activePos);

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
        newLine.setP2(QPointF(activePos.x(), startPoint.y()));  // Horizontal
    } else if (constraintDirection == Qt::Vertical) {
        newLine.setP2(QPointF(startPoint.x(), activePos.y()));  // Vertical
    }

    currentLine->setLine(newLine);
}

QPointF LineDrawer::findNearestTerminal(const QPointF &point, bool &snapped, QList<ComponentManager::Component> components)
{
    const qreal snapThreshold = SNAP_THRESHOLD; // Distance within which snapping occurs
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

void LineDrawer::finalizeLine(const QPointF &scenePos, QList<ComponentManager::Component> components)
{
    bool snapped;
    QPointF snapPos = findNearestTerminal(scenePos, snapped, components);

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

void LineDrawer::cancelDrawing()
{
    graphicsScene->removeItem(currentLine);
    delete currentLine;
    currentLine = nullptr;
    lineDrawing = false;
    qDebug() << "Removed a line";
}

bool LineDrawer::getLineDrawing() const
{
    return lineDrawing;
}

QGraphicsLineItem *LineDrawer::getCurrentLine() const
{
    return currentLine;
}
