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
#include <tesseract_qt/collision/contact_result_vector_standard_item.h>
#include <tesseract_qt/collision/contact_result_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
struct ContactResultVectorStandardItem::Implementation
{
  boost::uuids::uuid uuid;
};

ContactResultVectorStandardItem::ContactResultVectorStandardItem(const ContactResultVector& contact_results)
  : QStandardItem(icons::getCollisionIcon(), "Contact Results"), data_(std::make_unique<Implementation>())
{
  ctor(contact_results);
}

ContactResultVectorStandardItem::ContactResultVectorStandardItem(const QString& text,
                                                                 const ContactResultVector& contact_results)
  : QStandardItem(icons::getCollisionIcon(), text), data_(std::make_unique<Implementation>())
{
  ctor(contact_results);
}

ContactResultVectorStandardItem::ContactResultVectorStandardItem(const QIcon& icon,
                                                                 const QString& text,
                                                                 const ContactResultVector& contact_results)
  : QStandardItem(icon, text), data_(std::make_unique<Implementation>())
{
  ctor(contact_results);
}

int ContactResultVectorStandardItem::type() const
{
  return static_cast<int>(StandardItemType::COLLISION_CONTACT_RESULT_VECTOR);
}

const boost::uuids::uuid& ContactResultVectorStandardItem::getUUID() const { return data_->uuid; }

void ContactResultVectorStandardItem::ctor(const ContactResultVector& contact_results)
{
  setCheckable(true);
  data_->uuid == contact_results.getUUID();

  for (std::size_t i = 0; i < contact_results().size(); ++i)
  {
    auto* cr_item = new ContactResultStandardItem(QString("[%1]").arg(i), contact_results()[i]);
    auto* cr_value_item = new QStandardItem(QString("{ distance: %1 }").arg(contact_results()[i]().distance));
    appendRow({ cr_item, cr_value_item });
  }
}
}  // namespace tesseract_gui
