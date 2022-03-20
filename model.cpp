#include "model.h"

#include "node.h"

Model::Model(QObject* parent)
  : QObject{ parent }
{}

auto
Model::canConnect(const Node* a, const Node* b) const -> bool
{
  const auto aKind = getNodeKind(a);
  const auto bKind = getNodeKind(b);

  switch (aKind) {
    case NodeKind::Input:
      break;
    case NodeKind::Hidden:
      return (bKind == NodeKind::Input);
    case NodeKind::Output:
      return (bKind == NodeKind::Hidden) || (bKind == NodeKind::Input);
  }

  return false;
}

auto
Model::connect(Node* parent, std::shared_ptr<Node> child) -> bool
{
  if (canConnect(parent, child.get())) {
    parent->addConnection(std::move(child));
    emit modelChanged();
    return true;
  }

  return false;
}

auto
Model::createInputNode() -> Node*
{
  m_inputNodes.emplace_back(std::make_shared<Node>());

  emit modelChanged();

  return m_inputNodes.back().get();
}

auto
Model::createHiddenNode() -> Node*
{
  m_hiddenNodes.emplace_back(std::make_shared<Node>());

  emit modelChanged();

  return m_hiddenNodes.back().get();
}

auto
Model::createOutputNode() -> Node*
{
  m_outputNodes.emplace_back(std::make_shared<Node>());

  emit modelChanged();

  return m_outputNodes.back().get();
}

auto
Model::getConnectionCount() const -> size_type
{
  auto counter = [](const Node& node, NodeKind) -> size_type { return node.getConnections().size(); };

  return countNodeProperty(counter);
}

auto
Model::getInputCount() const -> size_type
{
  auto counter = [](const Node& node, NodeKind kind) -> size_type { return (kind == NodeKind::Input) ? 1 : 0; };

  return countNodeProperty(counter);
}

auto
Model::getOutputCount() const -> size_type
{
  auto counter = [](const Node& node, NodeKind kind) -> size_type { return (kind == NodeKind::Output) ? 1 : 0; };

  return countNodeProperty(counter);
}

void
Model::destroyNode(Node* node)
{
  for (auto it = m_inputNodes.cbegin(); it != m_inputNodes.cend(); it++) {
    if (it->get() == node) {
      m_inputNodes.erase(it);
      emit modelChanged();
      return;
    }
  }

  for (auto it = m_hiddenNodes.cbegin(); it != m_hiddenNodes.cend(); it++) {
    if (it->get() == node) {
      m_hiddenNodes.erase(it);
      emit modelChanged();
      return;
    }
  }

  for (auto it = m_outputNodes.cbegin(); it != m_outputNodes.cend(); it++) {
    if (it->get() == node) {
      m_outputNodes.erase(it);
      emit modelChanged();
      return;
    }
  }
}

auto
Model::getNodeKind(const Node* node) const -> NodeKind
{
  for (auto it = m_inputNodes.cbegin(); it != m_inputNodes.cend(); it++) {
    if (it->get() == node)
      return NodeKind::Input;
  }

  for (auto it = m_hiddenNodes.cbegin(); it != m_hiddenNodes.cend(); it++) {
    if (it->get() == node)
      return NodeKind::Hidden;
  }

  for (auto it = m_outputNodes.cbegin(); it != m_outputNodes.cend(); it++) {
    if (it->get() == node)
      return NodeKind::Output;
  }

  return NodeKind::Input;
}
