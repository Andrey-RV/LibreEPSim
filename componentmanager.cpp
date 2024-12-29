#include "componentmanager.h"

ComponentManager::ComponentManager() {}

void ComponentManager::appendComponent(ComponentManager::Component c)
{
    this->components.append(c);
}

QList<ComponentManager::Component> ComponentManager::getComponents() const
{
    return components;
}
