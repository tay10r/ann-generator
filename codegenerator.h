#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <QFormLayout>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <QCodeEditor>

class Model;
class QString;

class CodeGenerator : public QWidget
{
  Q_OBJECT
public:
  explicit CodeGenerator(QWidget* parent = nullptr);

  virtual ~CodeGenerator() = default;

  virtual void generate(const Model&) = 0;

signals:
  void propertiesChanged();

protected:
  void addFormWidget(const QString& label, QWidget* widget);

  void setCode(const QString& code);

  QWidget* getFormWidget() { return &m_form; }

  QCodeEditor* getCodeView() { return &m_codeView; }

private:
  QCodeEditor m_codeView{this};

  QWidget m_form{ this };

  QVBoxLayout m_layout{ this };

  QFormLayout m_formLayout{ &m_form };
};

#endif // CODEGENERATOR_H
