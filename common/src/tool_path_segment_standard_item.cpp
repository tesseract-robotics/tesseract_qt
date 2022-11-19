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

#include <tesseract_qt/common/tool_path_segment_standard_item.h>
#include <tesseract_qt/common/transform_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
ToolPathSegmentStandardItem::ToolPathSegmentStandardItem(const tesseract_common::ToolPathSegment& segment)
  : QStandardItem(icons::getTrajectoryIcon(), "Tool Path Segment")
{
  ctor(segment);
}

ToolPathSegmentStandardItem::ToolPathSegmentStandardItem(const QString& text,
                                                         const tesseract_common::ToolPathSegment& segment)
  : QStandardItem(icons::getTrajectoryIcon(), text)
{
  ctor(segment);
}

ToolPathSegmentStandardItem::ToolPathSegmentStandardItem(const QIcon& icon,
                                                         const QString& text,
                                                         const tesseract_common::ToolPathSegment& segment)
  : QStandardItem(icon, text)
{
  ctor(segment);
}

int ToolPathSegmentStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_TOOL_PATH_SEGMENT); }

tesseract_common::ToolPathSegment ToolPathSegmentStandardItem::getToolPathSegment() const
{
  tesseract_common::ToolPathSegment segment;
  segment.reserve(rowCount());
  for (std::size_t i = 0; i < rowCount(); ++i)
  {
    if (child(i, 0)->type() == static_cast<int>(StandardItemType::COMMON_TRANSFORM))
      segment.push_back(dynamic_cast<TransformStandardItem*>(child(i, 0))->getTransfrom());
  }
  return segment;
}

void ToolPathSegmentStandardItem::ctor(const tesseract_common::ToolPathSegment& segment)
{
  for (std::size_t i = 0; i < segment.size(); ++i)
    appendRow(new TransformStandardItem(QString("pose[%1]").arg(i), segment[i]));
}
}  // namespace tesseract_gui
