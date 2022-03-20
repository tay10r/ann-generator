#include "modelview.h"

#include "model.h"
#include "node.h"

#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>

#ifndef M_PI
#define M_PI 3.1415
#endif

#include <algorithm>

#include <QDebug>

ModelView::ModelView(Model* model, QWidget* parent)
  : QWidget(parent)
  , m_model(model)
{
  setMouseTracking(true);

  connect(this, &QWidget::customContextMenuRequested, this, &ModelView::showContextMenu);

  setContextMenuPolicy(Qt::CustomContextMenu);
}

void
ModelView::paintEvent(QPaintEvent*)
{
  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing);

  painter.fillRect(rect(), QBrush(QColor(0x28, 0x2a, 0x36)));

  paintGrid(painter);

  paintConnections(painter);

  paintNodes(painter);
}

void
ModelView::wheelEvent(QWheelEvent* wheelEvent)
{
  const float degrees = wheelEvent->angleDelta().y() / 8;

  m_controlState.scale += (degrees / 360) * m_zoomSpeed;

  m_controlState.scale = std::min(std::max(m_controlState.scale, m_minScale), m_maxScale);

  update();

  QWidget::wheelEvent(wheelEvent);
}

void
ModelView::mousePressEvent(QMouseEvent* mouseEvent)
{
  if (mouseEvent->button() == Qt::LeftButton) {

    const auto p = getViewTransform().inverted().map(mouseEvent->position());

    if (std::shared_ptr<Node> node = findNodeIntersection(QVector2D(p))) {
      if (m_controlState.connectTarget && m_model->connect(node.get(), m_controlState.connectTarget)) {
        m_controlState.connectTarget.reset();
      } else {
        m_controlState.moveTarget = std::move(node);
      }
    } else if (m_controlState.connectTarget) {
      m_controlState.connectTarget.reset();
    } else {
      m_controlState.inBackgroundDrag = true;
    }

    m_controlState.mouseX = mouseEvent->position().x();
    m_controlState.mouseY = mouseEvent->position().y();

    update();
  }

  QWidget::mousePressEvent(mouseEvent);
}

void
ModelView::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
  m_controlState.inBackgroundDrag = false;
  m_controlState.moveTarget = nullptr;
  QWidget::mouseReleaseEvent(mouseEvent);
}

void
ModelView::mouseMoveEvent(QMouseEvent* mouseEvent)
{
  if (m_controlState.inBackgroundDrag || m_controlState.moveTarget || m_controlState.connectTarget) {

    const float dx = mouseEvent->position().x() - m_controlState.mouseX;
    const float dy = mouseEvent->position().y() - m_controlState.mouseY;

    const QPointF delta(dx, dy);

    if (m_controlState.inBackgroundDrag) {
      m_controlState.translation[0] += delta.x();
      m_controlState.translation[1] += delta.y();
    } else if (m_controlState.moveTarget) {
      Node* node = m_controlState.moveTarget.get();
      node->setPosition(node->getPosition() + QVector2D(delta.x(), delta.y()));
    }

    m_controlState.mouseX = mouseEvent->position().x();
    m_controlState.mouseY = mouseEvent->position().y();

    update();
  }

  QWidget::mouseMoveEvent(mouseEvent);
}

void
ModelView::paintGrid(QPainter& painter)
{
  const float cellSize = getCellSize();

  const float scale = m_controlState.scale;

  painter.setTransform(getGridTransform());

  painter.setPen(createPen(Qt::DashLine, QColor(0x62, 0x72, 0xa4, 0x40)));

  const float w = width() / scale;
  const float h = height() / scale;

  const float xMin = 0;
  const float xMax = w;
  const float yMin = 0;
  const float yMax = h;

  const int columnCount = (w / cellSize) + 3;

  for (int i = 0; i < columnCount; i++) {
    const float x0 = xMin + (i * cellSize);
    const float x1 = x0;
    const float y0 = yMin - cellSize;
    const float y1 = yMax + cellSize;
    painter.drawLine(QPointF(x0, y0), QPointF(x1, y1));
  }

  const int rowCount = (h / cellSize) + 3;

  for (int i = 0; i < rowCount; i++) {
    const float x0 = xMin - cellSize;
    const float x1 = xMax + cellSize;
    const float y0 = yMin + (i * cellSize);
    const float y1 = y0;
    painter.drawLine(QPointF(x0, y0), QPointF(x1, y1));
  }
}

void
ModelView::paintNodes(QPainter& painter)
{
  painter.setTransform(getViewTransform());

  painter.setPen(QPen(Qt::NoPen));

  painter.setBrush(QBrush(QColor(0xff, 0xcc, 0)));

  for (const auto& node : m_model->getInputNodes()) {
    const auto p = node->getPosition().toPointF();
    const auto r = getNodeRadius();
    painter.drawEllipse(p, r, r);
  }

  painter.setBrush(QBrush(QColor(0xcc, 0xcc, 0x00)));

  for (const auto& node : m_model->getHiddenNodes()) {
    const auto p = node->getPosition().toPointF();
    const auto r = getNodeRadius();
    painter.drawEllipse(p, r, r);
  }

  painter.setBrush(QBrush(QColor(0xff, 0x66, 0x00)));

  for (const auto& node : m_model->getOutputNodes()) {
    const auto p = node->getPosition().toPointF();
    const auto r = getNodeRadius();
    painter.drawEllipse(p, r, r);
  }
}

void
ModelView::paintConnections(QPainter& painter)
{
  painter.setTransform(getViewTransform());

  painter.setPen(createPen(Qt::SolidLine, QColor(100, 100, 100)));

  if (m_controlState.connectTarget) {
    const auto a = m_controlState.connectTarget->getPosition();
    const auto b = getViewTransform().inverted().map(QPointF(m_controlState.mouseX, m_controlState.mouseY));
    painter.drawLine(a.toPointF(), b);
  }

  for (const auto& node : m_model->getInputNodes()) {
    for (const auto& connectedNode : node->getConnections()) {
      const auto p0 = node->getPosition().toPointF();
      const auto p1 = connectedNode->getPosition().toPointF();
      painter.drawLine(p0, p1);
    }
  }

  for (const auto& node : m_model->getHiddenNodes()) {
    for (const auto& connectedNode : node->getConnections()) {
      const auto p0 = node->getPosition().toPointF();
      const auto p1 = connectedNode->getPosition().toPointF();
      painter.drawLine(p0, p1);
    }
  }

  for (const auto& node : m_model->getOutputNodes()) {
    for (const auto& connectedNode : node->getConnections()) {
      const auto p0 = node->getPosition().toPointF();
      const auto p1 = connectedNode->getPosition().toPointF();
      painter.drawLine(p0, p1);
    }
  }
}

auto
ModelView::createPen(Qt::PenStyle style, const QColor& color) -> QPen
{
  QPen pen(color);
  pen.setStyle(style);
  pen.setWidthF(getStrokeSize());
  return pen;
}

void
ModelView::destroyNode(Node* node)
{
  if (node == m_controlState.connectTarget.get())
    m_controlState.connectTarget = nullptr;

  if (node == m_controlState.moveTarget.get())
    m_controlState.moveTarget = nullptr;

  m_model->destroyNode(node);
}

void
ModelView::showContextMenu(const QPoint& windowPoint)
{
  const auto point = getViewTransform().inverted().map(windowPoint);

  if (auto node = findNodeIntersection(QVector2D(point), getNodeRadius()))
    showNodeContextMenu(std::move(node), windowPoint);
  else
    showBackgroundContextMenu(windowPoint);
}

void
ModelView::showNodeContextMenu(std::shared_ptr<Node> node, const QPoint& windowPoint)
{
  QMenu contextMenu(tr("Node Menu"), this);

  QAction* connectAction = contextMenu.addAction(tr("Connect"));

  QAction* deleteAction = contextMenu.addAction(tr("Delete"));

  connect(connectAction, &QAction::triggered, [this, node]() { m_controlState.connectTarget = std::move(node); });

  connect(deleteAction, &QAction::triggered, [this, &node]() {
    destroyNode(node.get());
    update();
  });

  contextMenu.exec(mapToGlobal(windowPoint));
}

void
ModelView::showBackgroundContextMenu(const QPoint& windowPoint)
{
  const auto point = getViewTransform().inverted().map(windowPoint);

  QMenu contextMenu(tr("Add a Node"), this);

  QAction* inputAction = contextMenu.addAction(tr("Input Node"));
  QAction* hiddenAction = contextMenu.addAction(tr("Hidden Node"));
  QAction* outputAction = contextMenu.addAction(tr("Output Node"));

  connect(inputAction, &QAction::triggered, [this, point] { integrateNewNode(m_model->createInputNode(), point); });
  connect(hiddenAction, &QAction::triggered, [this, point] { integrateNewNode(m_model->createHiddenNode(), point); });
  connect(outputAction, &QAction::triggered, [this, point] { integrateNewNode(m_model->createOutputNode(), point); });

  contextMenu.exec(mapToGlobal(windowPoint));
}

void
ModelView::integrateNewNode(Node* node, const QPoint& point)
{
  node->setPosition(QVector2D(point));

  update();
}

auto
ModelView::isNodeIntersected(const QVector2D& point, const Node& node, float nodeRadius) -> bool
{
  const auto delta = point - node.getPosition();

  return QVector2D::dotProduct(delta, delta) < (nodeRadius * nodeRadius);
}

auto
ModelView::findNodeIntersection(const QVector2D& point, const NodeVector& nodes, float nodeRadius) -> std::shared_ptr<Node>
{
  for (auto& node : nodes) {
    if (isNodeIntersected(point, *node, nodeRadius))
      return node;
  }

  return nullptr;
}

auto
ModelView::findNodeIntersection(const QVector2D& point, float nodeRadius) -> std::shared_ptr<Node>
{
  if (auto node = findNodeIntersection(point, m_model->getInputNodes(), nodeRadius))
    return node;

  if (auto node = findNodeIntersection(point, m_model->getHiddenNodes(), nodeRadius))
    return node;

  if (auto node = findNodeIntersection(point, m_model->getOutputNodes(), nodeRadius))
    return node;

  return nullptr;
}

QTransform
ModelView::getViewTransform() const
{
  const float scale = m_controlState.scale;
  const float tx = m_controlState.translation[0];
  const float ty = m_controlState.translation[1];
  return QTransform::fromTranslate(tx, ty) * QTransform::fromScale(scale, scale);
}

QTransform
ModelView::getGridTransform() const
{
  const float scale = m_controlState.scale;
  const float tx = std::fmod(m_controlState.translation[0], getCellSize());
  const float ty = std::fmod(m_controlState.translation[1], getCellSize());
  return QTransform::fromTranslate(tx, ty) * QTransform::fromScale(scale, scale);
}
