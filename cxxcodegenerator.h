#ifndef CXXCODEGENERATOR_H
#define CXXCODEGENERATOR_H

#include "codegenerator.h"

#include <QLineEdit>
#include <QString>
#include <QStringList>

#include <QCXXHighlighter>

class QString;

class CxxCodeGenerator : public CodeGenerator
{
  Q_OBJECT
public:
  explicit CxxCodeGenerator(QWidget* parent = nullptr);

  void generate(const Model& model) override;

private:
  auto getModelClassName() const -> QString;

  auto beginFuncDef(const QString& funcName, const QStringList& params, const QString& result) -> QString;

private:
  QLineEdit m_namespaceEdit{ getFormWidget() };

  QLineEdit m_modelEdit{ getFormWidget() };

  QCXXHighlighter m_highlighter;
};

#endif // CXXCODEGENERATOR_H
