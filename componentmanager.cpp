#include "componentmanager.h"
#include "mygraphicsview.h"
#include "grid.h"
#include <QGraphicsScene>
#include <QTimer>

ComponentManager::ComponentManager(std::shared_ptr<MyGraphicsView> graphicsView, std::shared_ptr<QGraphicsScene> graphicsScene)
    : graphicsView(graphicsView)
    , graphicsScene(graphicsScene)
{}

void ComponentManager::startComponentPlacement(const QString& imagePath, QPixmap componentPixmap = QPixmap(),
                                               std::shared_ptr<QGraphicsPixmapItem> component = nullptr)
{
    // Load the component image
    if (!component) {
        QPixmap componentPixmap(imagePath);
        if (componentPixmap.isNull()) {
            qWarning() << "Failed to load component image:" << imagePath;
            return;
        }
        currentComponent = std::make_shared<QGraphicsPixmapItem>(componentPixmap);
    }

    else {
        currentComponent = component;
    }

    QPointF mousePos = graphicsView->mapToScene(graphicsView->mapFromGlobal(QCursor::pos()));
    QPointF gridSnappedPos = grid.snapToGrid(mousePos, grid.GRID_SIZE);
    currentComponent->setPos(gridSnappedPos - QPointF(componentPixmap.width() / 2, componentPixmap.height() / 2));
    currentComponent->setTransformationMode(Qt::SmoothTransformation);
    if (!graphicsScene->items().contains(currentComponent.get())) {
       graphicsScene->addItem(currentComponent.get());
    }
    componentIsMoving = true;
    graphicsView->setCursor(Qt::BlankCursor);

    // Timer for image position update
    if (!moveTimer->isActive()) {
        moveTimer->start(10);  // Update every 10 milliseconds
    }
}

void ComponentManager::finalizeComponentPlacement() {
    componentIsMoving = false;

    if (moveTimer->isActive()) {
        moveTimer->stop();
    }

    // Save the component in a list of structs
    component.item = currentComponent;
    component.terminals = {
        // !TODO: These coordinates only work for the generator. Need to be generalized for other components
        QPointF(30, 3),  // Up terminal
        QPointF(30, 79)   // Down terminal
    };

    for (QPointF &terminal : component.terminals) {
        terminal = currentComponent->mapToScene(terminal);
    }

    appendComponent(component);
    currentComponent.reset();
    graphicsView->unsetCursor();
}

void ComponentManager::updateImagePosition()
{
    if (componentIsMoving && currentComponent) {
        QPointF mousePos = graphicsView->mapToScene(graphicsView->mapFromGlobal(QCursor::pos()));
        QPointF gridSnappedPos = grid.snapToGrid(mousePos, grid.GRID_SIZE);
        currentComponent->setPos(gridSnappedPos - QPointF(currentComponent->pixmap().width() / 2, currentComponent->pixmap().height() / 2));
    }
}

void ComponentManager::appendComponent(ComponentManager::Component c)
{
    this->components.append(c);
}

void ComponentManager::removeComponent(QGraphicsPixmapItem* item) {
    for (auto& component : components) {
        if (component.item.get() == item) {
            graphicsScene->removeItem(component.item.get());

            auto it = std::find_if(components.begin(), components.end(),
                                   [&](const Component& c) { return c.item.get() == item; });
            if (it != components.end()) {
                components.erase(it);
            }

            break;
        }
    }
}

void ComponentManager::startComponentMoving(std::shared_ptr<QGraphicsPixmapItem> item) {
    componentIsMoving = true;
    currentComponent = item;
}

bool ComponentManager::getComponentIsMoving() const
{
    return componentIsMoving;
}

void ComponentManager::setComponentIsMoving(bool newComponentIsMoving)
{
    componentIsMoving = newComponentIsMoving;
}

std::shared_ptr<QGraphicsPixmapItem> ComponentManager::getCurrentComponent() const
{
    return currentComponent;
}

void ComponentManager::setCurrentComponent(std::shared_ptr<QGraphicsPixmapItem> newCurrentComponent)
{
    currentComponent = newCurrentComponent;
}

QTimer *ComponentManager::getMoveTimer() const
{
    return moveTimer;
}

void ComponentManager::setMoveTimer(QTimer *newMoveTimer)
{
    moveTimer = newMoveTimer;
}

QList<ComponentManager::Component> ComponentManager::getComponents() const
{
    return components;
}
