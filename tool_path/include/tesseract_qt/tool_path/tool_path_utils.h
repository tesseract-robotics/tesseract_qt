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
#ifndef TOOL_PATH_TOOL_PATH_UTILS_H
#define TOOL_PATH_TOOL_PATH_UTILS_H

#include <Qt>

class QStandardItem;

namespace tesseract_gui
{
class TransformStandardItem;
class ToolPathSegmentStandardItem;
class ToolPathStandardItem;

/**
 * @brief Find upstream parent transform item given item
 * @throw If no parent is a transform item
 * @param item The item to search upstream for parent that is a transform item
 * @return The upstream parent transform item
 */
TransformStandardItem* findTransformItem(QStandardItem* item);

/**
 * @brief Find upstream parent tool path segment item given item
 * @throw If no upstream parent is a tool path segment item
 * @param item The item to search upstream for parent that is a tool path segment item
 * @return The upstream parent tool path segment item
 */
ToolPathSegmentStandardItem* findToolPathSegmentItem(QStandardItem* item);

/**
 * @brief Find upstream parent tool path item given item
 * @throw If no upstream parent is a tool path item
 * @param item The item to search upstream for parent that is a tool path item
 * @return The upstream parent tool path item
 */
ToolPathStandardItem* findToolPathItem(QStandardItem* item);

/**
 * @brief Recursively set the checked state
 * @param item The item to set checked state recursively
 * @param st The state to assign
 */
void setCheckedStateRecursive(QStandardItem* item, Qt::CheckState st);

}  // namespace tesseract_gui

#endif  // TOOL_PATH_TOOL_PATH_UTILS_H
