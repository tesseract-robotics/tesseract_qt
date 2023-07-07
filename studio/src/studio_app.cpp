#include <tesseract_qt/studio/studio.h>
#include <tesseract_qt/common/theme_utils.h>
#include <QApplication>
#include <QDebug>
#include <QString>
#include <console_bridge/console.h>
#include <boost/program_options.hpp>
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

  console_bridge::setLogLevel(console_bridge::CONSOLE_BRIDGE_LOG_DEBUG);

  Q_INIT_RESOURCE(tesseract_qt_resources);
  Q_INIT_RESOURCE(qdarkstyle_dark);
  Q_INIT_RESOURCE(qdarkstyle_light);

  // setup stylesheet
  app.setStyleSheet(tesseract_gui::themes::getDarkTheme());

  namespace po = boost::program_options;
  std::string config;
  po::options_description desc("Options");
  desc.add_options()("help,h",
                     "Print help messages")("config,c", po::value<std::string>(&config), "File path to config.");

  po::variables_map vm;
  try
  {
    po::store(po::parse_command_line(argc, argv, desc), vm);  // can throw

    /** --help option */
    if (vm.count("help") != 0U)
    {
      std::stringstream ss;
      ss << "Basic Command Line Parameter App" << std::endl << desc << std::endl;
      CONSOLE_BRIDGE_logInform(ss.str().c_str());
      return 0;
    }

    // throws on error, so do after help in case there are any problems
    po::notify(vm);
  }
  catch (po::error& e)
  {
    std::stringstream ss;
    ss << "ERROR: " << e.what() << std::endl << std::endl;
    ss << desc << std::endl;
    CONSOLE_BRIDGE_logError(ss.str().c_str());
    return 1;
  }

  tesseract_gui::Studio widget;
  widget.show();
  if (!config.empty())
    widget.loadConfig(QString::fromStdString(config));

  return QApplication::exec();
}
