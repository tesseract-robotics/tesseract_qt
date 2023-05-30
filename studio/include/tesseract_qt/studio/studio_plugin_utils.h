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
#ifndef TESSERACT_QT_STUDIO_STUDIO_PLUGIN_UTILS_H
#define TESSERACT_QT_STUDIO_STUDIO_PLUGIN_UTILS_H

class QMainWindow;
class QMenuBar;
class QMenu;
class QStatusBar;
class QAction;
class QString;

namespace tesseract_gui
{
QMainWindow* getStudioMainWindow();
QMenuBar* getStudioMenuBar();
QStatusBar* getStudioStatusBar();

/**
 * @brief Find menu given text
 * @param text The menue text
 * @return If not found nullptr, otherwise menu item
 */
QMenu* getStudioMenu(const QString& text);

/**
 * @brief Find menu action given text
 * @param text The menue action text
 * @return If not found nullptr, otherwise menu action item
 */
QAction* getStudioMenuAction(QMenu* menu, const QString& text);
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_STUDIO_STUDIO_PLUGIN_UTILS_H
