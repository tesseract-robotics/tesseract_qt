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
#include <tesseract_qt/scene_graph/models/scene_state_standard_item.h>
#include <tesseract_qt/common/models/transform_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_scene_graph/scene_state.h>

namespace tesseract_gui
{
SceneStateStandardItem::SceneStateStandardItem(const tesseract_scene_graph::SceneState& scene_state)
  : QStandardItem(icons::getModelIcon(), "Scene State")
{
  ctor(scene_state);
}

SceneStateStandardItem::SceneStateStandardItem(const QString& text,
                                               const tesseract_scene_graph::SceneState& scene_state)
  : QStandardItem(icons::getModelIcon(), text)
{
  ctor(scene_state);
}

SceneStateStandardItem::SceneStateStandardItem(const QIcon& icon,
                                               const QString& text,
                                               const tesseract_scene_graph::SceneState& scene_state)
  : QStandardItem(icon, text)
{
  ctor(scene_state);
}

int SceneStateStandardItem::type() const { return static_cast<int>(StandardItemType::SG_SCENE_STATE); }

void SceneStateStandardItem::ctor(const tesseract_scene_graph::SceneState& scene_state)
{
  auto* values_item = new QStandardItem(icons::getJointVectorIcon(), "Values");
  auto* links_item = new QStandardItem(icons::getLinkVectorIcon(), "Links");
  auto* joints_item = new QStandardItem(icons::getJointVectorIcon(), "Joints");

  for (const auto& joint : scene_state.joints)
    values_item->appendRow(createStandardItemFloat(joint.first, joint.second));

  for (const auto& link : scene_state.link_transforms)
    links_item->appendRow(new TransformStandardItem(QString::fromStdString(link.first), link.second));

  for (const auto& joint : scene_state.joint_transforms)
    joints_item->appendRow(new TransformStandardItem(QString::fromStdString(joint.first), joint.second));

  values_item->sortChildren(0);
  links_item->sortChildren(0);
  joints_item->sortChildren(0);

  appendRow(values_item);
  appendRow(links_item);
  appendRow(joints_item);
}

}  // namespace tesseract_gui
