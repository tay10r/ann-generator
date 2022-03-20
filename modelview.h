#ifndef MODELVIEW_H
#define MODELVIEW_H

#include <QWidget>
#include <QVector>

class Node;
class Model;
class QTransform;
class QPen;

class ModelView : public QWidget
{
  Q_OBJECT
public:
  using NodeVector = QVector<std::shared_ptr<Node>>;

  explicit ModelView(Model* model, QWidget* parent = nullptr);

  void setZoomSpeed(float speed) { m_zoomSpeed = speed; }

  auto getZoomSpeed() const -> float { return m_zoomSpeed; }

signals:

private:
  void paintEvent(QPaintEvent*) override;

  void mousePressEvent(QMouseEvent*) override;

  void mouseReleaseEvent(QMouseEvent*) override;

  void mouseMoveEvent(QMouseEvent*) override;

  void wheelEvent(QWheelEvent*) override;

  void paintGrid(QPainter&);

  void paintNodes(QPainter&);

  void paintConnections(QPainter&);

  void showContextMenu(const QPoint&);

  void showBackgroundContextMenu(const QPoint&);

  void showNodeContextMenu(std::shared_ptr<Node> node, const QPoint&);

  void integrateNewNode(Node* node, const QPoint&);

  auto isNodeIntersected(const QVector2D& point, const Node& node, float nodeRadius) -> bool;

  auto findNodeIntersection(const QVector2D& point, const NodeVector& nodes, float nodeRadius) -> std::shared_ptr<Node>;

  auto findNodeIntersection(const QVector2D& point, float nodeRadius = getNodeRadius()) -> std::shared_ptr<Node>;

  void destroyNode(Node* node);

  auto aspect() const -> float { return float(width()) / height(); }

  static constexpr float getStrokeSize() { return 2.0f; }

  static constexpr float getCellSize() { return 50.0f; }

  static constexpr float getNodeRadius() { return getCellSize() / 2.0f; }

  auto getGridTransform() const -> QTransform;

  auto getViewTransform() const -> QTransform;

  static auto createPen(Qt::PenStyle, const QColor& color) -> QPen;

  struct ControlState final
  {
    bool inBackgroundDrag = false;

    std::shared_ptr<Node> moveTarget;

    std::shared_ptr<Node> connectTarget;

    float scale = 1;

    float translation[2]{ 0, 0 };

    float mouseX = 0;
    float mouseY = 0;
  };

  float m_zoomSpeed = 0.25;

  const float m_minScale = 5e-1;

  const float m_maxScale = 1.5;

  ControlState m_controlState;

  Model* m_model;
};

#endif // MODELVIEW_H
