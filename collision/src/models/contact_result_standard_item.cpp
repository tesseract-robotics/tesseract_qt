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
#include <tesseract_qt/collision/models/contact_result_standard_item.h>
#include <tesseract_qt/common/transform_standard_item.h>
#include <tesseract_qt/common/position_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
ContactResultStandardItem::ContactResultStandardItem(const ContactResult& contact_result)
  : QStandardItem(icons::getCollisionIcon(), "Contact Result"), contact_result(contact_result)
{
  ctor();
}

ContactResultStandardItem::ContactResultStandardItem(const QString& text, const ContactResult& contact_result)
  : QStandardItem(icons::getCollisionIcon(), text), contact_result(contact_result)
{
  ctor();
}

ContactResultStandardItem::ContactResultStandardItem(const QIcon& icon,
                                                     const QString& text,
                                                     const ContactResult& contact_result)
  : QStandardItem(icon, text), contact_result(contact_result)
{
  ctor();
}

int ContactResultStandardItem::type() const { return static_cast<int>(StandardItemType::COLLISION_CONTACT_RESULT); }

std::string toString(tesseract_collision::ContinuousCollisionType cc_type)
{
  switch (cc_type)
  {
    case tesseract_collision::ContinuousCollisionType::CCType_None:
      return "CCType_None";
    case tesseract_collision::ContinuousCollisionType::CCType_Time0:
      return "CCType_Time0";
    case tesseract_collision::ContinuousCollisionType::CCType_Time1:
      return "CCType_Time1";
    case tesseract_collision::ContinuousCollisionType::CCType_Between:
      return "CCType_Between";
    default:
      throw std::runtime_error("Unhandled tesseract_collision::ContinuousCollisionType");
  }
}
void ContactResultStandardItem::ctor()
{
  setCheckable(true);
  setCheckState(Qt::CheckState::Checked);
  appendRow(createStandardItemFloat("distance", contact_result().distance));
  appendRow(new PositionStandardItem("normal", contact_result().normal));
  appendRow(createStandardItemBool("single_contact_point", contact_result().single_contact_point));

  {
    auto* item = new QStandardItem(icons::getNumericIcon(), "type_id");
    item->appendRow(createStandardItemInt("[0]", contact_result().type_id[0]));
    item->appendRow(createStandardItemInt("[1]", contact_result().type_id[1]));
    appendRow(item);
  }

  {
    auto* item = new QStandardItem(icons::getTextIcon(), "link_names");
    item->appendRow(createStandardItemString("[0]", contact_result().link_names[0]));
    item->appendRow(createStandardItemString("[1]", contact_result().link_names[1]));
    appendRow(item);
  }

  {
    auto* item = new QStandardItem(icons::getNumericIcon(), "shape_id");
    item->appendRow(createStandardItemInt("[0]", contact_result().shape_id[0]));
    item->appendRow(createStandardItemInt("[1]", contact_result().shape_id[1]));
    appendRow(item);
  }

  {
    auto* item = new QStandardItem(icons::getNumericIcon(), "subshape_id");
    item->appendRow(createStandardItemInt("[0]", contact_result().subshape_id[0]));
    item->appendRow(createStandardItemInt("[1]", contact_result().subshape_id[1]));
    appendRow(item);
  }

  {
    auto* item = new QStandardItem(icons::getPositionIcon(), "nearest_points");
    item->appendRow(new PositionStandardItem("[0]", contact_result().nearest_points[0]));
    item->appendRow(new PositionStandardItem("[1]", contact_result().nearest_points[1]));
    appendRow(item);
  }

  {
    auto* item = new QStandardItem(icons::getPositionIcon(), "nearest_points_local");
    item->appendRow(new PositionStandardItem("[0]", contact_result().nearest_points_local[0]));
    item->appendRow(new PositionStandardItem("[1]", contact_result().nearest_points_local[1]));
    appendRow(item);
  }

  {
    auto* item = new QStandardItem(icons::getOriginIcon(), "transform");
    item->appendRow(new TransformStandardItem("[0]", contact_result().transform[0]));
    item->appendRow(new TransformStandardItem("[1]", contact_result().transform[1]));
    appendRow(item);
  }

  {
    auto* item = new QStandardItem(icons::getNumericIcon(), "cc_time");
    item->appendRow(createStandardItemFloat("[0]", contact_result().cc_time[0]));
    item->appendRow(createStandardItemFloat("[1]", contact_result().cc_time[1]));
    appendRow(item);
  }

  {
    auto* item = new QStandardItem(icons::getTextIcon(), "cc_type");
    item->appendRow(createStandardItemString("[0]", toString(contact_result().cc_type[0])));
    item->appendRow(createStandardItemString("[1]", toString(contact_result().cc_type[1])));
    appendRow(item);
  }

  {
    auto* item = new QStandardItem(icons::getOriginIcon(), "cc_transform");
    item->appendRow(new TransformStandardItem("[0]", contact_result().cc_transform[0]));
    item->appendRow(new TransformStandardItem("[1]", contact_result().cc_transform[1]));
    appendRow(item);
  }
}
}  // namespace tesseract_gui
