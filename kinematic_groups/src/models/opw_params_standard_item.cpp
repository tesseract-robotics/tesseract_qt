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
#include <tesseract_qt/kinematic_groups/models/opw_params_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
OPWParamsStandardItem::OPWParamsStandardItem(opw_kinematics::Parameters<double> params)
  : QStandardItem(icons::getCubeIcon(), "OPW"), params(params)
{
  ctor();
}

OPWParamsStandardItem::OPWParamsStandardItem(const QString& text, opw_kinematics::Parameters<double> params)
  : QStandardItem(icons::getCubeIcon(), text), params(params)
{
  ctor();
}

OPWParamsStandardItem::OPWParamsStandardItem(const QIcon& icon,
                                             const QString& text,
                                             opw_kinematics::Parameters<double> params)
  : QStandardItem(icon, text), params(params)
{
  ctor();
}

int OPWParamsStandardItem::type() const { return static_cast<int>(StandardItemType::SRDF_OPW_PARAMS); }

void OPWParamsStandardItem::ctor()
{
  auto* params_item = new QStandardItem("params");
  params_item->appendRow(createStandardItemFloat("a1", params.a1));
  params_item->appendRow(createStandardItemFloat("a2", params.a2));
  params_item->appendRow(createStandardItemFloat("b", params.b));
  params_item->appendRow(createStandardItemFloat("c1", params.c1));
  params_item->appendRow(createStandardItemFloat("c2", params.c2));
  params_item->appendRow(createStandardItemFloat("c3", params.c3));
  params_item->appendRow(createStandardItemFloat("c4", params.c4));
  appendRow(params_item);

  auto* offsets_item = new QStandardItem("offsets");
  for (std::size_t i = 0; i < 6; ++i)
    offsets_item->appendRow(createStandardItemFloat(QString("[%1]").arg(i).toStdString(), params.offsets[0]));

  appendRow(offsets_item);

  auto* correction_item = new QStandardItem("sign correction");
  for (std::size_t i = 0; i < 6; ++i)
    correction_item->appendRow(
        createStandardItemFloat(QString("[%1]").arg(i).toStdString(), params.sign_corrections[0]));

  appendRow(correction_item);
}
}  // namespace tesseract_gui
