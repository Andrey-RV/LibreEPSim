#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMainWindow>
#include "mygraphicsview.h"
#include "grid.h"

class ComponentManager: public QMainWindow
{
    Q_OBJECT
public:
    ComponentManager(MyGraphicsView* graphicsView, QGraphicsScene* graphicsScene);
    static constexpr int TIMER_INTERVAL = 16;  // 60 FPS

    struct Component {
        QGraphicsPixmapItem *item;
        QList<QPointF> terminals; // List of terminal positions
    } component;

    void startComponentPlacement(const QString& imagePath);
    void finalizeComponentPlacement();
    void appendComponent(ComponentManager::Component c);
    QList<Component> getComponents() const;

    bool getComponentIsMoving() const;
    void setComponentIsMoving(bool newComponentIsMoving);

    QGraphicsPixmapItem *getCurrentComponent() const;
    void setCurrentComponent(QGraphicsPixmapItem *newCurrentComponent);

    QTimer *getMoveTimer() const;
    void setMoveTimer(QTimer *newMoveTimer);

public slots:
    void updateImagePosition();

private:
    Grid grid;
    QList<Component> components;
    QGraphicsPixmapItem *currentComponent = nullptr;
    QList<QGraphicsPixmapItem*> componentsItems;
    MyGraphicsView* graphicsView;
    QGraphicsScene* graphicsScene;
    bool componentIsMoving = false;
    QTimer *moveTimer = nullptr;
};

#endif // COMPONENTMANAGER_H
