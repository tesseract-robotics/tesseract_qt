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
#include <tesseract_qt/scene_graph/models/calibration_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_scene_graph/joint.h>

namespace tesseract::gui
{
CalibrationStandardItem::CalibrationStandardItem(std::shared_ptr<tesseract::scene_graph::JointCalibration> calibration)
  : QStandardItem(icons::getCalibrationIcon(), "Calibration"), calibration(std::move(calibration))
{
  ctor();
}

CalibrationStandardItem::CalibrationStandardItem(const QString& text,
                                                 std::shared_ptr<tesseract::scene_graph::JointCalibration> calibration)
  : QStandardItem(icons::getCalibrationIcon(), text), calibration(std::move(calibration))
{
  ctor();
}

CalibrationStandardItem::CalibrationStandardItem(const QIcon& icon,
                                                 const QString& text,
                                                 std::shared_ptr<tesseract::scene_graph::JointCalibration> calibration)
  : QStandardItem(icon, text), calibration(std::move(calibration))
{
  ctor();
}

int CalibrationStandardItem::type() const { return static_cast<int>(StandardItemType::SG_CALIBRATION); }

void CalibrationStandardItem::ctor()
{
  appendRow(createStandardItemFloat("reference_position", calibration->reference_position));
  appendRow(createStandardItemFloat("rising", calibration->rising));
  appendRow(createStandardItemFloat("falling", calibration->falling));
}
}  // namespace tesseract::gui
