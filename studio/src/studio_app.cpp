#include <tesseract_qt/studio/studio.h>
#include <tesseract_qt/common/theme_utils.h>
#include <QApplication>
#include <QDebug>
#include <QString>

#include <sstream>

int main(int argc, char* argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if QT_VERSION >= 0x050600
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#endif
  QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

  QApplication app(argc, argv);
  QApplication::setApplicationName("Tesseract Studio");
  QApplication::setQuitOnLastWindowClosed(true);

  Q_INIT_RESOURCE(tesseract_qt_resources);
  Q_INIT_RESOURCE(qdarkstyle_dark);
  Q_INIT_RESOURCE(qdarkstyle_light);

  // setup stylesheet
  app.setStyleSheet(tesseract::gui::themes::getDarkTheme());

  tesseract::gui::Studio widget;
  if (!widget.init(argc, argv))
    return 0;

  widget.show();

  return QApplication::exec();
}
