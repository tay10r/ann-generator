#include "node.h"

auto
Node::addConnection(std::shared_ptr<Node> node) -> bool
{
  if ((node.get() != this) && !connected(node.get())) {
    m_connections.emplace_back(std::move(node));
    return true;
  }

  return false;
}

auto
Node::connected(const Node* node) const -> bool
{
  for (const auto& n : m_connections) {
    if (n.get() == node)
      return true;
  }
  return false;
}
