#ifndef LAYER_H
#define LAYER_H

#include "component.h"

#include <QVector>

#include <memory>

class Node;

class Layer final : public Component
{
public:
private:
  QVector<std::shared_ptr<Node>> m_nodes;
};

#endif // LAYER_H
