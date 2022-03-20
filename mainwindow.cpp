#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  m_layout.addWidget(&m_tabWidget);

  m_tabWidget.addTab(&m_modelView, tr("Designer"));

  m_tabWidget.addTab(&m_compilerWidget, tr("IR Compiler"));

  m_tabWidget.addTab(&m_codeGenerator, tr("C++ Code Generator"));

  setCentralWidget(&m_centralWidget);

  connect(&m_model, &Model::modelChanged, [this]() {

    m_codeGenerator.generate(m_model);

    m_compilerWidget.compile(m_model);
  });

  connect(&m_compilerWidget, &CompilerWidget::programCompiled, [this]() {

    //
  });

  connect(&m_codeGenerator, &CodeGenerator::propertiesChanged, [this]() { m_codeGenerator.generate(m_model); });

  m_codeGenerator.generate(m_model);
}

MainWindow::~MainWindow() {}
