#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMainWindow>
#include "mygraphicsview.h"
#include "grid.h"
#include <memory>
#include <QTimer>

class ComponentManager : public QMainWindow
{
    Q_OBJECT
public:
    ComponentManager(std::shared_ptr<MyGraphicsView> graphicsView, std::shared_ptr<QGraphicsScene> graphicsScene);
    static constexpr int TIMER_INTERVAL = 16;  // 60 FPS (update every 16ms)

    struct Component {
        std::shared_ptr<QGraphicsPixmapItem> item;
        QList<QPointF> terminals;
    } component;

    void startComponentPlacement(const QString& imagePath);
    void finalizeComponentPlacement();
    void appendComponent(ComponentManager::Component c);

    QList<Component> getComponents() const;

    bool getComponentIsMoving() const;
    void setComponentIsMoving(bool newComponentIsMoving);

    std::shared_ptr<QGraphicsPixmapItem> getCurrentComponent() const;
    void setCurrentComponent(std::shared_ptr<QGraphicsPixmapItem> newCurrentComponent);

    QTimer *getMoveTimer() const;
    void setMoveTimer(QTimer *newMoveTimer);

public slots:
    void updateImagePosition();

private:
    Grid grid;
    QList<Component> components;
    std::shared_ptr<QGraphicsPixmapItem> currentComponent = nullptr;
    QList<std::shared_ptr<QGraphicsPixmapItem>> componentsItems;
    std::shared_ptr<MyGraphicsView> graphicsView;
    std::shared_ptr<QGraphicsScene> graphicsScene;
    bool componentIsMoving = false;
    QTimer *moveTimer = nullptr;
};

#endif //
