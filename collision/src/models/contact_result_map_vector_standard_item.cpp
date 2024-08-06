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
#include <tesseract_qt/collision/models/contact_result_map_vector_standard_item.h>
#include <tesseract_qt/collision/models/contact_result_map_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
ContactResultMapVectorStandardItem::ContactResultMapVectorStandardItem(
    const std::vector<ContactResultMap>& contact_results)
  : QStandardItem(icons::getSetIcon(), "Contact Results")
{
  ctor(contact_results);
}

ContactResultMapVectorStandardItem::ContactResultMapVectorStandardItem(
    const QString& text,
    const std::vector<ContactResultMap>& contact_results)
  : QStandardItem(icons::getSetIcon(), text)
{
  ctor(contact_results);
}

ContactResultMapVectorStandardItem::ContactResultMapVectorStandardItem(
    const QIcon& icon,
    const QString& text,
    const std::vector<ContactResultMap>& contact_results)
  : QStandardItem(icon, text)
{
  ctor(contact_results);
}

int ContactResultMapVectorStandardItem::type() const
{
  return static_cast<int>(StandardItemType::COLLISION_CONTACT_RESULT_MAP);
}

void ContactResultMapVectorStandardItem::ctor(const std::vector<ContactResultMap>& contact_results)
{
  for (std::size_t i = 0; i < contact_results.size(); ++i)
    appendRow(new ContactResultMapStandardItem(icons::getSetIcon(), QString("[%1]").arg(i), contact_results[i]));
}
}  // namespace tesseract_gui
