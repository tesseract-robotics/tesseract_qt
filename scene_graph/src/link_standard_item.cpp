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
#include <tesseract_qt/scene_graph/link_standard_item.h>
#include <tesseract_qt/scene_graph/inertial_standard_item.h>
#include <tesseract_qt/scene_graph/visual_standard_item.h>
#include <tesseract_qt/scene_graph/collision_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/type_standard_item.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
class LinkStandardItem::Implementation
{
public:
  QStandardItem* collisions_item{ nullptr };
  QStandardItem* visuals_item{ nullptr };
};

LinkStandardItem::LinkStandardItem(tesseract_scene_graph::Link::Ptr link, bool checkable)
  : QStandardItem(icons::getLinkIcon(), "Link"), link(std::move(link)), data_(std::make_unique<Implementation>())
{
  ctor(checkable);
}

LinkStandardItem::LinkStandardItem(const QString& text, tesseract_scene_graph::Link::Ptr link, bool checkable)
  : QStandardItem(icons::getLinkIcon(), text), link(std::move(link)), data_(std::make_unique<Implementation>())
{
  ctor(checkable);
}

LinkStandardItem::LinkStandardItem(const QIcon& icon,
                                   const QString& text,
                                   tesseract_scene_graph::Link::Ptr link,
                                   bool checkable)
  : QStandardItem(icon, text), link(std::move(link)), data_(std::make_unique<Implementation>())
{
  ctor(checkable);
}

int LinkStandardItem::type() const { return static_cast<int>(StandardItemType::SG_LINK); }

QStandardItem* LinkStandardItem::getCollisionsItem() { return data_->collisions_item; }

QStandardItem* LinkStandardItem::getVisualsItem() { return data_->visuals_item; }

void LinkStandardItem::ctor(bool checkable)
{
  if (checkable)
  {
    setCheckable(true);
    setCheckState(Qt::CheckState::Checked);
  }

  appendRow(createStandardItemString("name", link->getName()));

  if (link->inertial != nullptr)
    appendRow(new InertialStandardItem(link->inertial));

  if (!link->visual.empty())
  {
    data_->visuals_item =
        new TypeStandardItem(icons::getVisualVectorIcon(), "Visual", static_cast<int>(StandardItemType::SG_VISUALS));
    for (std::size_t i = 0; i < link->visual.size(); ++i)
      data_->visuals_item->appendRow(new VisualStandardItem(QString("[%1]").arg(i), link->visual.at(i)));

    if (checkable)
    {
      data_->visuals_item->setCheckable(true);
      data_->visuals_item->setCheckState(Qt::CheckState::Checked);
    }

    appendRow(data_->visuals_item);
  }

  if (!link->collision.empty())
  {
    data_->collisions_item = new TypeStandardItem(
        icons::getCollisionVectorIcon(), "Collision", static_cast<int>(StandardItemType::SG_COLLISIONS));
    for (std::size_t i = 0; i < link->collision.size(); ++i)
      data_->collisions_item->appendRow(new CollisionStandardItem(QString("[%1]").arg(i), link->collision.at(i)));

    if (checkable)
    {
      data_->collisions_item->setCheckable(true);
      data_->collisions_item->setCheckState(Qt::CheckState::Unchecked);
    }

    appendRow(data_->collisions_item);
  }
}
}  // namespace tesseract_gui
