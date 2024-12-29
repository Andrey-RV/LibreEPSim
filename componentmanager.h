#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <QGraphicsPixmapItem>

class ComponentManager
{
public:
    ComponentManager();
    struct Component {
        QGraphicsPixmapItem *item;
        QList<QPointF> terminals; // List of terminal positions
    } component;

    void appendComponent(ComponentManager::Component c);

    QList<Component> getComponents() const;

private:
    QList<Component> components;

};

#endif // COMPONENTMANAGER_H
