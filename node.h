#ifndef NODE_H
#define NODE_H

#include "component.h"

#include <QVector2D>
#include <QVector>

#include <memory>

class Node final : public Component
{
public:
  using NodeVector = QVector<std::shared_ptr<Node>>;

  /// @brief Adds a connected node, if it isn't already connected.
  ///
  /// @return True if the node was connected, false if it was not.
  auto addConnection(std::shared_ptr<Node> connectedNode) -> bool;

  auto getConnections() const -> const NodeVector& { return m_connections; }

  auto getPosition() const -> QVector2D { return m_position; }

  void setPosition(const QVector2D& p) { m_position = p; }

  auto connected(const Node* node) const -> bool;

private:
  NodeVector m_connections;

  QVector2D m_position{ 0, 0 };
};

#endif // NODE_H
