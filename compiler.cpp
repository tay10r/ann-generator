#include "compiler.h"

#include "model.h"

#include <QTextStream>

namespace {

class Compiler final
{
public:
  Compiler(const Model& model)
    : m_model(model)
  {}

  auto compile() -> Program
  {

    for (const auto& outputNode : m_model.getOutputNodes())
    {
      m_outputIndices.emplace_back(compileNode(*outputNode));
    }

    return Program(std::move(m_exprs), std::move(m_outputIndices));
  }

private:
  auto pushExpr(Expr* expr) -> std::uint32_t
  {
    m_exprs.emplace_back(expr);

    return m_exprs.size() - 1;
  }

  auto compileNode(const Node& node) -> std::uint32_t
  {
    std::vector<std::uint32_t> inputs;

    inputs.emplace_back(pushExpr(new ZeroExpr()));

    for (const auto& connection : node.getConnections())
    {
      const auto prev = inputs.back();

      const auto tmp = compileNode(*connection);
      const auto b = pushExpr(new BiasExpr(0));
      const auto w = pushExpr(new WeightExpr(0));
      const auto result = pushExpr(new MultiplyAddExpr(tmp, b, w));

      inputs.emplace_back(pushExpr(new AddExpr(prev, result)));
    }

    return pushExpr(new ActivationExpr(inputs.back()));
  }

private:
  const Model& m_model;

  ExprVector m_exprs;

  std::vector<std::uint32_t> m_outputIndices;
};

} // namespace

CompilerWidget::CompilerWidget(QWidget* parent)
  : QWidget(parent)
{
  m_layout.addWidget(&m_irView);
}

void
CompilerWidget::compile(const Model& model)
{
  Compiler compiler(model);

  m_program = compiler.compile();

  updateIR();

  emit programCompiled();
}

namespace {

class IRViewBuilder final : public ExprVisitor
{
public:
  IRViewBuilder(QString* output)
    : m_irStream(output)
  {}

  void visit(const ActivationExpr& activationExpr) override
  {
    m_irStream << reg(m_dstIndex) << " = activate " << reg(activationExpr.getInputExpr()) << "\n";
    m_dstIndex++;
  }

  void visit(const AddExpr& addExpr) override
  {
    m_irStream << reg(m_dstIndex) << " = add " << reg(addExpr.getInputExpr1()) << " " << reg(addExpr.getInputExpr2()) << "\n";
    m_dstIndex++;
  }

  void visit(const MultiplyAddExpr& multiplyAddExpr) override
  {
    m_irStream << reg(m_dstIndex) << " <- madd";
    m_irStream << ' ';
    m_irStream << reg(multiplyAddExpr.getInputExpr1());
    m_irStream << ' ';
    m_irStream << reg(multiplyAddExpr.getInputExpr2());
    m_irStream << ' ';
    m_irStream << reg(multiplyAddExpr.getInputExpr3());
    m_irStream << '\n';
    m_dstIndex++;
  }

  void visit(const InputExpr& inputExpr) override
  {
    m_irStream << reg(m_dstIndex) << " = input " << number(inputExpr.getInputIndex()) << "\n";
    m_dstIndex++;
  }

  void visit(const BiasExpr& biasExpr) override
  {
    m_irStream << reg(m_dstIndex) << " = bias " << number(biasExpr.getBiasIndex()) << "\n";
    m_dstIndex++;
  }

  void visit(const WeightExpr& weightExpr) override
  {
    m_irStream << reg(m_dstIndex) << " = weight " << number(weightExpr.getWeightIndex()) << "\n";
    m_dstIndex++;
  }

  void visit(const ZeroExpr& zeroExpr) override
  {
    m_irStream << reg(m_dstIndex) << " = zero\n";
    m_dstIndex++;
  }

private:
  QString number(std::uint32_t value)
  {
    return QString::number(value);
  }

  QString reg(std::uint32_t value)
  {
    return QString("%") + QString::number(value);
  }

private:
  QTextStream m_irStream;

  std::uint32_t m_dstIndex = 0;
};

} // namespace

void
CompilerWidget::updateIR()
{
  QString ir;

  IRViewBuilder builder(&ir);

  for (const auto &expr : m_program.getExprs())
  {
    expr->accept(builder);
  }

  m_irView.setPlainText(ir);
}
