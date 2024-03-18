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

#include <tesseract_qt/common/models/tool_path_segment_standard_item.h>
#include <tesseract_qt/common/models/transform_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/tool_path_pose.h>
#include <tesseract_qt/common/tool_path_segment.h>

namespace tesseract_gui
{
ToolPathSegmentStandardItem::ToolPathSegmentStandardItem(const ToolPathSegment& segment)
  : QStandardItem(icons::getToolPathIcon(), "Tool Path Segment")
{
  ctor(segment);
}

ToolPathSegmentStandardItem::ToolPathSegmentStandardItem(const QString& text, const ToolPathSegment& segment)
  : QStandardItem(icons::getToolPathIcon(), text)
{
  ctor(segment);
}

ToolPathSegmentStandardItem::ToolPathSegmentStandardItem(const QIcon& icon,
                                                         const QString& text,
                                                         const ToolPathSegment& segment)
  : QStandardItem(icon, text)
{
  ctor(segment);
}

int ToolPathSegmentStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_TOOL_PATH_SEGMENT); }

const boost::uuids::uuid& ToolPathSegmentStandardItem::getUUID() const { return uuid_; }
const boost::uuids::uuid& ToolPathSegmentStandardItem::getParentUUID() const { return parent_uuid_; }

ToolPathSegment ToolPathSegmentStandardItem::getToolPathSegment() const
{
  ToolPathSegment segment(uuid_, description_);
  if (!parent_uuid_.is_nil())
    segment.setParentUUID(parent_uuid_);

  segment.reserve(rowCount());
  for (std::size_t i = 0; i < rowCount(); ++i)
  {
    if (child(i, 0)->type() == static_cast<int>(StandardItemType::COMMON_TRANSFORM))
    {
      auto* pose_item = dynamic_cast<TransformStandardItem*>(child(i, 0));
      ToolPathPose pose(pose_item->getTransfrom(), pose_item->getUUID(), pose_item->getDescription());
      if (!pose_item->getParentUUID().is_nil())
        pose.setParentUUID(pose_item->getParentUUID());

      segment.push_back(pose);
    }
  }
  return segment;
}

tesseract_common::VectorIsometry3d ToolPathSegmentStandardItem::getCommonToolPathSegment() const
{
  tesseract_common::VectorIsometry3d segment;
  segment.reserve(rowCount());
  for (std::size_t i = 0; i < rowCount(); ++i)
  {
    if (child(i, 0)->type() == static_cast<int>(StandardItemType::COMMON_TRANSFORM))
      segment.push_back(dynamic_cast<TransformStandardItem*>(child(i, 0))->getTransfrom());
  }
  return segment;
}

void ToolPathSegmentStandardItem::ctor(const ToolPathSegment& segment)
{
  setCheckable(true);
  setCheckState(Qt::CheckState::Checked);
  for (std::size_t i = 0; i < segment.size(); ++i)
  {
    auto& pose = segment[i];
    auto* pose_item = new TransformStandardItem(QString("pose[%1]").arg(i), pose);
    auto* pose_description_item = new QStandardItem(QString::fromStdString(pose.getDescription()));
    appendRow({ pose_item, pose_description_item });
  }

  uuid_ = segment.getUUID();
  parent_uuid_ = segment.getParentUUID();
  description_ = segment.getDescription();
}
}  // namespace tesseract_gui
