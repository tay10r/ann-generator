#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int
main(int argc, char* argv[])
{
  QApplication a(argc, argv);

  a.setApplicationName(QObject::tr("ANN Generator"));

  QTranslator translator;

  const QStringList uiLanguages = QLocale::system().uiLanguages();

  for (const QString& locale : uiLanguages) {
    const QString baseName = "nngen_" + QLocale(locale).name();
    if (translator.load(":/i18n/" + baseName)) {
      a.installTranslator(&translator);
      break;
    }
  }

  MainWindow w;

#ifdef __EMSCRIPTEN__
  w.showFullScreen();
#else
  w.showMaximized();
#endif

  return a.exec();
}
