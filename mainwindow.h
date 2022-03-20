#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>

#include "cxxcodegenerator.h"
#include "model.h"
#include "modelview.h"
#include "compiler.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);

  ~MainWindow();

private:
  QWidget m_centralWidget{ this };

  QVBoxLayout m_layout{ &m_centralWidget };

  QTabWidget m_tabWidget{ &m_centralWidget };

  Model m_model;

  ModelView m_modelView{ &m_model, &m_tabWidget };

  CxxCodeGenerator m_codeGenerator{ this };

  CompilerWidget m_compilerWidget{ this };
};

#endif // MAINWINDOW_H
