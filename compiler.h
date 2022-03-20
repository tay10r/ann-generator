#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QCodeEditor>

#include "ir.h"

class Model;

class CompilerWidget : public QWidget
{
  Q_OBJECT
public:
  explicit CompilerWidget(QWidget* parent);

  void compile(const Model&);

  auto getProgram() const -> const Program& { return m_program; }

signals:
  void programCompiled();

private:
  void updateIR();

private:
  Program m_program;

  QVBoxLayout m_layout{this};

  QCodeEditor m_irView{this};
};
