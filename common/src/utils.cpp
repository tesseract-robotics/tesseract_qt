/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU Lesser General Public License Version 3, 29 June 2007
 * @par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * @par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * @par
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <tesseract_qt/common/utils.h>

#include <tesseract_common/utils.h>
#include <boost/algorithm/string.hpp>
#include <graphviz/gvc.h>

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QApplication>

namespace tesseract_gui
{
std::vector<std::string> getNamespaces(const std::string& namespace_str, const std::string& separator)
{
  std::vector<std::string> namespaces;
  boost::split(namespaces, namespace_str, boost::is_any_of(separator), boost::token_compress_on);
  return namespaces;
}

// https://www.graphviz.org/pdf/libguide.pdf
// 2.3 Rendering the graph
bool saveDotImage(const std::string& dot_string, const tesseract_common::fs::path& save_path, std::string format)
{
  tesseract_common::fs::path dot_filepath =
      tesseract_common::getTempPath() + "save_dot_image_" + tesseract_common::getTimestampString() + ".dot";
  std::ofstream out(dot_filepath);
  out << dot_string;
  out.close();
  return saveDotImage(dot_filepath, save_path, format);
}

bool saveDotImage(const tesseract_common::fs::path& dot_path,
                  const tesseract_common::fs::path& save_path,
                  std::string format)
{
  GVC_t* gvc;
  Agraph_t* g;
  FILE* fp;

  gvc = gvContext();
  fp = fopen(dot_path.c_str(), "r");
  g = agread(fp, 0);
  agsafeset(g, const_cast<char*>("dpi"), const_cast<char*>("300"), const_cast<char*>("300"));
  gvLayout(gvc, g, "dot");
  gvRender(gvc, g, format.c_str(), fopen(save_path.c_str(), "w"));
  gvFreeLayout(gvc, g);
  agclose(g);
  return (gvFreeContext(gvc));
}

std::string getEnvNamespaceFromTopic(const std::string& topic)
{
  std::vector<std::string> tokens;
  boost::split(tokens, topic, boost::is_any_of("/"), boost::token_compress_on);
  tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [](const std::string& token) { return token.empty(); }),
               tokens.end());
  if (!tokens.empty())
    return tokens.at(0);

  return std::string();
}

QMainWindow* getMainWindow()
{
  QWidgetList widgets = qApp->allWidgets();
  for (auto* widget : widgets)
  {
    if (widget->windowTitle() == QApplication::applicationName())
      return qobject_cast<QMainWindow*>(widget);
  }
  return nullptr;
}

QMenuBar* getMainWindowMenuBar()
{
  QMainWindow* main_window = getMainWindow();
  return main_window->menuBar();
}

QStatusBar* getMainWindowStatusBar()
{
  QMainWindow* main_window = getMainWindow();
  return main_window->statusBar();
}

QMenu* getMainWindowMenu(const QString& text)
{
  QMenuBar* menu_bar = getMainWindowMenuBar();
  QList<QMenu*> menus = menu_bar->findChildren<QMenu*>();
  for (auto* menu : menus)
  {
    if (menu->title() == text)
      return menu;
  }

  return nullptr;
}

QAction* getMainWindowMenuAction(QMenu* menu, const QString& text)
{
  QList<QAction*> actions = menu->actions();
  for (auto& action : actions)
  {
    if (action->text() == text)
      return action;
  }

  return nullptr;
}

}  // namespace tesseract_gui
