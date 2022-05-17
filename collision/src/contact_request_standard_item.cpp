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
#include <tesseract_qt/collision/contact_request_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
ContactRequestStandardItem::ContactRequestStandardItem(const tesseract_collision::ContactRequest& contact_request)
  : QStandardItem(icons::getCollisionIcon(), "Contact Request"), contact_request(contact_request)
{
  ctor();
}

ContactRequestStandardItem::ContactRequestStandardItem(const QString& text,
                                                       const tesseract_collision::ContactRequest& contact_request)
  : QStandardItem(icons::getCollisionIcon(), text), contact_request(contact_request)
{
  ctor();
}

ContactRequestStandardItem::ContactRequestStandardItem(const QIcon& icon,
                                                       const QString& text,
                                                       const tesseract_collision::ContactRequest& contact_request)
  : QStandardItem(icon, text), contact_request(contact_request)
{
  ctor();
}

int ContactRequestStandardItem::type() const { return static_cast<int>(StandardItemType::COLLISION_CONTACT_REQUEST); }

std::string toString(tesseract_collision::ContactTestType c_type)
{
  switch (c_type)
  {
    case tesseract_collision::ContactTestType::ALL:
      return "ALL";
    case tesseract_collision::ContactTestType::CLOSEST:
      return "CLOSEST";
    case tesseract_collision::ContactTestType::FIRST:
      return "FIRST";
    case tesseract_collision::ContactTestType::LIMITED:
      return "LIMITED";
    default:
      throw std::runtime_error("Unhandled tesseract_collision::ContactTestType");
  }
}
void ContactRequestStandardItem::ctor()
{
  appendRow(createStandardItemString("type", toString(contact_request.type)));
  appendRow(createStandardItemBool("calculate_distance", contact_request.calculate_distance));
  appendRow(createStandardItemBool("calculate_penetration", contact_request.calculate_penetration));
  appendRow(createStandardItemString("is_valid", (contact_request.is_valid == nullptr) ? "NULL" : "Object"));
}
}  // namespace tesseract_gui
