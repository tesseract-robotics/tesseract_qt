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

#include <tesseract_qt/common/tool_path_standard_item.h>
#include <tesseract_qt/common/tool_path_segment_standard_item.h>
#include <tesseract_qt/common/transform_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
ToolPathStandardItem::ToolPathStandardItem(const tesseract_common::ToolPath& tool_path)
  : QStandardItem(icons::getTrajectoryIcon(), "Tool Path")
{
  ctor(tool_path);
}

ToolPathStandardItem::ToolPathStandardItem(const QString& text, const tesseract_common::ToolPath& tool_path)
  : QStandardItem(icons::getTrajectoryIcon(), text)
{
  ctor(tool_path);
}

ToolPathStandardItem::ToolPathStandardItem(const QIcon& icon,
                                           const QString& text,
                                           const tesseract_common::ToolPath& tool_path)
  : QStandardItem(icon, text)
{
  ctor(tool_path);
}

int ToolPathStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_TOOL_PATH); }

tesseract_common::ToolPath ToolPathStandardItem::getToolPath() const
{
  tesseract_common::ToolPath tool_path;
  tool_path.reserve(rowCount());
  for (std::size_t i = 0; i < rowCount(); ++i)
  {
    if (child(i, 0)->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH_SEGMENT))
      tool_path.push_back(dynamic_cast<ToolPathSegmentStandardItem*>(child(i, 0))->getToolPathSegment());
  }
  return tool_path;
}

void ToolPathStandardItem::ctor(const tesseract_common::ToolPath& tool_path)
{
  for (std::size_t j = 0; j < tool_path.size(); ++j)
    appendRow(new ToolPathSegmentStandardItem(QString("segment[%1]").arg(j), tool_path[j]));
}
}  // namespace tesseract_gui
