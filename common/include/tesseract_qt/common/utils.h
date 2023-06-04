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
#ifndef TESSERACT_QT_COMMON_UTILS_H
#define TESSERACT_QT_COMMON_UTILS_H

#include <vector>
#include <string>

#include <tesseract_common/types.h>
#include <tesseract_common/allowed_collision_matrix.h>
#include <tesseract_collision/core/discrete_contact_manager.h>

class QMainWindow;
class QMenuBar;
class QMenu;
class QStatusBar;
class QAction;
class QString;

namespace tesseract_gui
{
/**
 * @brief Extract a vector of namespace separated by the provided separator
 * @param namespace_str The string to extract namespaces from
 * @param separator The string to split on
 * @return A vector of namespaces
 */
std::vector<std::string> getNamespaces(const std::string& namespace_str, const std::string& separator = "::");

/**
 * @brief Convert a dot string to image and save
 * @param dot_string The dot string to convert
 * @param save_path The location to save
 * @return True if successful, otherwise false
 */
bool saveDotImage(const std::string& dot_string,
                  const tesseract_common::fs::path& save_path,
                  std::string format = "ps");

/**
 * @brief Convert a dot file to image and save
 * @param dot_path The location of the dot file
 * @param save_path The location to save
 * @return True if successful, otherwise false
 */
bool saveDotImage(const tesseract_common::fs::path& dot_path,
                  const tesseract_common::fs::path& save_path,
                  std::string format = "ps");

/**
 * @brief Get environment namespace from topic string
 * @param topic The topic
 * @return The environment namespace
 */
std::string getEnvNamespaceFromTopic(const std::string& topic);

/**
 * @brief Get the main window
 * @return The main window
 */
QMainWindow* getMainWindow();

/**
 * @brief Get the MainWindow menu bar
 * @return The studio menu bar
 */
QMenuBar* getMainWindowMenuBar();

/**
 * @brief Get MainWindow status bar
 * @return The status bar
 */
QStatusBar* getMainWindowStatusBar();

/**
 * @brief Find menu given text
 * @param text The menue text
 * @return If not found nullptr, otherwise menu item
 */
QMenu* getMainWindowMenu(const QString& text);

/**
 * @brief Find menu action given text
 * @param text The menue action text
 * @return If not found nullptr, otherwise menu action item
 */
QAction* getMainWindowMenuAction(QMenu* menu, const QString& text);
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_COMMON_UTILS_H
