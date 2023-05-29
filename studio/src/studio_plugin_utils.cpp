/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2023 Levi Armstrong <levi.armstrong@gmail.com>
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
#include <tesseract_qt/studio/studio_plugin_utils.h>

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QApplication>

namespace tesseract_gui
{
QMainWindow* getStudioMainWindow()
{
  QWidgetList widgets = qApp->allWidgets();
  for (auto* widget : widgets)
  {
    if (widget->windowTitle() == QApplication::applicationName())
      return qobject_cast<QMainWindow*>(widget);
  }
  return nullptr;
}

QMenuBar* getStudioMenuBar()
{
  QMainWindow* main_window = getStudioMainWindow();
  return main_window->menuBar();
}

QStatusBar* getStudioStatusBar()
{
  QMainWindow* main_window = getStudioMainWindow();
  return main_window->statusBar();
}

QMenu* getStudioMenu(const QString& text)
{
  QMenuBar* menu_bar = getStudioMenuBar();
  QList<QMenu*> menus = menu_bar->findChildren<QMenu*>();
  for (auto* menu : menus)
  {
    if (menu->title() == text)
      return menu;
  }

  return nullptr;
}

QAction* getStudioMenuAction(QMenu* menu, const QString& text)
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
