#include "linedrawer.h"
#include "componentmanager.h"
#include <QGraphicsLineItem>


LineDrawer::LineDrawer(std::shared_ptr<QGraphicsScene> graphicsScene)
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
        // First point of the line
        startPoint = grid.snapToGrid(scenePos, grid.GRID_SIZE);
        currentLine = new QGraphicsLineItem(QLineF(startPoint, startPoint));
        currentLine->setPen(QPen(Qt::black, 2));
        graphicsScene->addItem(currentLine);
    } else {
        // Next points
        QPointF endPoint = grid.snapToGrid(scenePos, grid.GRID_SIZE);;
        if (constraintDirection == Qt::Horizontal) {
            endPoint.setY(startPoint.y());
        } else if (constraintDirection == Qt::Vertical) {
            endPoint.setX(startPoint.x());
        }

        QLineF newLine(startPoint, endPoint);
        currentLine->setLine(newLine);

        // Starts next segment starting in the end of the last segment
        startPoint = endPoint;
        currentLine = new QGraphicsLineItem(QLineF(startPoint, startPoint));
        currentLine->setPen(QPen(Qt::black, 2));
        graphicsScene->addItem(currentLine);
        constraintDirection = static_cast<Qt::Orientation>(-1);
    }
}

void LineDrawer::continueLineDrawing(const QPointF &scenePos)
{
    QPointF activePos = grid.snapToGrid(scenePos, grid.GRID_SIZE);
    QLineF newLine(startPoint, activePos); // From wherever the click occurred to wherever the mouse is

    // Restrict to horizontal or vertical based on initial movement
    if (constraintDirection == static_cast<Qt::Orientation>(-1)) {
        if (qAbs(newLine.dx()) > qAbs(newLine.dy())) {
            constraintDirection = Qt::Horizontal;
        } else {
            constraintDirection = Qt::Vertical;
        }
    }

    if (constraintDirection == Qt::Horizontal) {
        newLine.setP2(QPointF(activePos.x(), startPoint.y()));
    } else if (constraintDirection == Qt::Vertical) {
        newLine.setP2(QPointF(startPoint.x(), activePos.y()));
    }

    currentLine->setLine(newLine);
}

QPointF LineDrawer::findNearestTerminal(const QPointF &point, bool &snapped, QList<ComponentManager::Component> components)
{
    const qreal snapThreshold = SNAP_THRESHOLD; // Distance from a terminal which snapping occurs
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
    QPointF endPoint = scenePos;  // Default to the clicked position if no snapping

    if (snapped) {
        // Snap only if snapping results in a horizontal or vertical line
        qreal dx = qAbs(snapPos.x() - startPoint.x());
        qreal dy = qAbs(snapPos.y() - startPoint.y());

        if (constraintDirection == Qt::Horizontal && dy < 1e-6) {
            endPoint = snapPos;
        } else if (constraintDirection == Qt::Vertical && dx < 1e-6) {
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

    if (constraintDirection == Qt::Horizontal) {
        endPoint.setY(startPoint.y());
    } else if (constraintDirection == Qt::Vertical) {
        endPoint.setX(startPoint.x());
    }

    QLineF newLine(startPoint, endPoint);
    currentLine->setLine(newLine);

    lineDrawing = false;
    constraintDirection = static_cast<Qt::Orientation>(-1);  // Reset to no direction defined
    currentLine = nullptr;
}

void LineDrawer::cancelDrawing() {
    if (currentLine) {
        graphicsScene->removeItem(currentLine);
        delete currentLine;
        currentLine = nullptr;
    }
    lineDrawing = false;
    constraintDirection = static_cast<Qt::Orientation>(-1);
}

void LineDrawer::removeLine(QGraphicsLineItem* item) {
    graphicsScene->removeItem(item);
}

bool LineDrawer::getLineDrawing() const
{
    return lineDrawing;
}

QGraphicsLineItem *LineDrawer::getCurrentLine() const
{
    return currentLine;
}
