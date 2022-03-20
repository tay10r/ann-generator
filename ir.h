#pragma once

#include <iosfwd>
#include <memory>
#include <vector>

#include <cstdint>

class ActivationExpr;
class AddExpr;
class MultiplyAddExpr;
class ZeroExpr;
class BiasExpr;
class WeightExpr;
class InputExpr;

class ExprVisitor
{
public:
  virtual ~ExprVisitor() = default;

  virtual void visit(const ActivationExpr&) = 0;

  virtual void visit(const AddExpr&) = 0;

  virtual void visit(const MultiplyAddExpr&) = 0;

  virtual void visit(const ZeroExpr&) = 0;

  virtual void visit(const BiasExpr&) = 0;

  virtual void visit(const WeightExpr&) = 0;

  virtual void visit(const InputExpr&) = 0;
};

class Expr
{
public:
  virtual ~Expr() = default;

  virtual void accept(ExprVisitor& visitor) const = 0;
};

using UniqueExprPtr = std::unique_ptr<Expr>;

using ExprVector = std::vector<UniqueExprPtr>;

template<typename Derived>
class NullaryExpr : public Expr
{
public:
  virtual ~NullaryExpr() = default;

  void accept(ExprVisitor& visitor) const override { visitor.visit(static_cast<const Derived&>(*this)); }
};

template<typename Derived>
class UnaryExpr : public Expr
{
public:
  UnaryExpr(std::uint32_t inExpr)
    : m_inExpr(inExpr)
  {}

  virtual ~UnaryExpr() = default;

  void accept(ExprVisitor& visitor) const override { visitor.visit(static_cast<const Derived&>(*this)); }

  auto getInputExpr() const noexcept { return m_inExpr; }

private:
  std::uint32_t m_inExpr = 0;
};

template<typename Derived>
class BinaryExpr : public Expr
{
public:
  BinaryExpr(std::uint32_t expr1, std::uint32_t expr2)
    : m_expr1(expr1)
    , m_expr2(expr2)
  {}

  virtual ~BinaryExpr() = default;

  void accept(ExprVisitor& visitor) const override { visitor.visit(static_cast<const Derived&>(*this)); }

  auto getInputExpr1() const noexcept { return m_expr1; }

  auto getInputExpr2() const noexcept { return m_expr2; }

private:
  std::uint32_t m_expr1 = 0;

  std::uint32_t m_expr2 = 0;
};

template<typename Derived>
class TernaryExpr : public Expr
{
public:
  TernaryExpr(std::uint32_t expr1, std::uint32_t expr2, std::uint32_t expr3)
    : m_expr1(expr1)
    , m_expr2(expr2)
    , m_expr3(expr3)
  {}

  virtual ~TernaryExpr() = default;

  void accept(ExprVisitor& visitor) const override { visitor.visit(static_cast<const Derived&>(*this)); }

  auto getInputExpr1() const noexcept { return m_expr1; }

  auto getInputExpr2() const noexcept { return m_expr2; }

  auto getInputExpr3() const noexcept { return m_expr3; }

private:
  std::uint32_t m_expr1 = 0;

  std::uint32_t m_expr2 = 0;

  std::uint32_t m_expr3 = 0;
};

class ZeroExpr final : public NullaryExpr<ZeroExpr>
{};

class ActivationExpr final : public UnaryExpr<ActivationExpr>
{
public:
  using UnaryExpr<ActivationExpr>::UnaryExpr;
};

class AddExpr final : public BinaryExpr<AddExpr>
{
public:
  using BinaryExpr<AddExpr>::BinaryExpr;
};

class MultiplyAddExpr final : public TernaryExpr<MultiplyAddExpr>
{
public:
  using TernaryExpr<MultiplyAddExpr>::TernaryExpr;
};

class InputExpr final : public Expr
{
public:
  InputExpr(std::uint32_t inputIndex)
    : m_inputIndex(inputIndex)
  {}

  void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }

  auto getInputIndex() const noexcept -> std::uint32_t { return m_inputIndex; }

private:
  std::uint32_t m_inputIndex = 0;
};

class WeightExpr final : public Expr
{
public:
  WeightExpr(std::uint32_t weightIndex)
    : m_weightIndex(weightIndex)
  {}

  void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }

  auto getWeightIndex() const noexcept -> std::uint32_t { return m_weightIndex; }

private:
  std::uint32_t m_weightIndex = 0;
};

class BiasExpr final : public Expr
{
public:
  BiasExpr(std::uint32_t biasIndex)
    : m_biasIndex(biasIndex)
  {}

  void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }

  auto getBiasIndex() const noexcept -> std::uint32_t { return m_biasIndex; }

private:
  std::uint32_t m_biasIndex = 0;
};

class Program final
{
public:
  Program() = default;

  Program(ExprVector&& exprList, std::vector<std::uint32_t>&& outputExprs);

  auto getExprs() const -> const ExprVector& { return m_exprs; }

  auto getOutputExprIndices() const -> const std::vector<std::uint32_t>& { return m_outputExprs; }

private:
  ExprVector m_exprs;

  std::vector<std::uint32_t> m_outputExprs;
};
