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
#include <tesseract_qt/common/models/calibration_info_standard_item.h>
#include <tesseract_qt/common/models/transform_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
CalibrationInfoStandardItem::CalibrationInfoStandardItem(tesseract_common::CalibrationInfo calibration_info)
  : QStandardItem(icons::getCubeIcon(), "Calibration Info"), calibration_info(std::move(calibration_info))
{
  ctor();
}

CalibrationInfoStandardItem::CalibrationInfoStandardItem(const QString& text,
                                                         tesseract_common::CalibrationInfo calibration_info)
  : QStandardItem(icons::getCubeIcon(), text), calibration_info(std::move(calibration_info))
{
  ctor();
}

CalibrationInfoStandardItem::CalibrationInfoStandardItem(const QIcon& icon,
                                                         const QString& text,
                                                         tesseract_common::CalibrationInfo calibration_info)
  : QStandardItem(icon, text), calibration_info(std::move(calibration_info))
{
  ctor();
}

int CalibrationInfoStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_CALIBRATION_INFO); }

void CalibrationInfoStandardItem::ctor()
{
  auto* joints_item = new QStandardItem(icons::getJointIcon(), "Joints");
  for (auto& joint : calibration_info.joints)
  {
    auto* item = new TransformStandardItem(QString::fromStdString(joint.first), joint.second);
    joints_item->appendRow(item);
  }
  joints_item->sortChildren(0);
  appendRow(joints_item);
}
}  // namespace tesseract_gui
