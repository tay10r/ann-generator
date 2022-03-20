#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QVector>

#include <memory>

#include <cstddef>

#include "node.h"

enum class NodeKind
{
  Input,
  Hidden,
  Output
};

class Model : public QObject
{
  Q_OBJECT
public:
  using size_type = std::size_t;

  explicit Model(QObject* parent = nullptr);

  auto connect(Node* parent, std::shared_ptr<Node> child) -> bool;

  auto canConnect(const Node* parent, const Node* child) const -> bool;

  auto createInputNode() -> Node*;

  auto createHiddenNode() -> Node*;

  auto createOutputNode() -> Node*;

  auto getInputNodes() const -> const QVector<std::shared_ptr<Node>>& { return m_inputNodes; }

  auto getHiddenNodes() const -> const QVector<std::shared_ptr<Node>>& { return m_hiddenNodes; }

  auto getOutputNodes() const -> const QVector<std::shared_ptr<Node>>& { return m_outputNodes; }

  void destroyNode(Node* node);

  auto getNodeKind(const Node* node) const -> NodeKind;

  auto getConnectionCount() const -> size_type;

  auto getInputCount() const -> size_type;

  auto getOutputCount() const -> size_type;

  template<typename Counter>
  auto countNodeProperty(Counter counter) const -> size_type;

signals:
  void modelChanged();

private:
  QVector<std::shared_ptr<Node>> m_inputNodes;

  QVector<std::shared_ptr<Node>> m_outputNodes;

  QVector<std::shared_ptr<Node>> m_hiddenNodes;
};

template<typename Counter>
auto
Model::countNodeProperty(Counter counter) const -> size_type
{
  size_type result = 0;

  for (const auto& node : m_inputNodes)
    result += counter(*node, NodeKind::Input);

  for (const auto& node : m_hiddenNodes)
    result += counter(*node, NodeKind::Hidden);

  for (const auto& node : m_outputNodes)
    result += counter(*node, NodeKind::Output);

  return result;
}

#endif // MODEL_H
