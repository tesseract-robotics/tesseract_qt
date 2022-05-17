/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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
#include <tesseract_qt/common/manipulator_info_standard_item.h>
#include <tesseract_qt/common/transform_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_common/manipulator_info.h>

namespace tesseract_gui
{
ManipulatorInfoStandardItem::ManipulatorInfoStandardItem(const tesseract_common::ManipulatorInfo& manip_info)
  : QStandardItem(icons::getRobotArmIcon(), "Manipulator Info")
{
  ctor(manip_info);
}

ManipulatorInfoStandardItem::ManipulatorInfoStandardItem(const QString& text,
                                                         const tesseract_common::ManipulatorInfo& manip_info)
  : QStandardItem(icons::getRobotArmIcon(), text)
{
  ctor(manip_info);
}

ManipulatorInfoStandardItem::ManipulatorInfoStandardItem(const QIcon& icon,
                                                         const QString& text,
                                                         const tesseract_common::ManipulatorInfo& manip_info)
  : QStandardItem(icon, text)
{
  ctor(manip_info);
}

int ManipulatorInfoStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_MANIPULATOR_INFO); }

void ManipulatorInfoStandardItem::ctor(const tesseract_common::ManipulatorInfo& manip_info)
{
  appendRow(createStandardItemString("manipulator", manip_info.manipulator));
  appendRow(createStandardItemString("working_frame", manip_info.working_frame));
  appendRow(createStandardItemString("tcp_frame", manip_info.tcp_frame));

  if (manip_info.tcp_offset.index() == 0)
    appendRow(createStandardItemString("tcp_offset", std::get<std::string>(manip_info.tcp_offset)));
  else
    appendRow(new TransformStandardItem("tcp_offset", std::get<Eigen::Isometry3d>(manip_info.tcp_offset)));

  appendRow(createStandardItemString("manipulator_ik_solver", manip_info.manipulator_ik_solver));
}
}  // namespace tesseract_gui
