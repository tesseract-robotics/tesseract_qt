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
#include <tesseract_qt/scene_graph/scene_graph_standard_item.h>
#include <tesseract_qt/scene_graph/link_standard_item.h>
#include <tesseract_qt/scene_graph/joint_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_scene_graph/graph.h>

namespace tesseract_gui
{
SceneGraphStandardItem::SceneGraphStandardItem() : QStandardItem(icons::getModelIcon(), "SceneGraph") {}

SceneGraphStandardItem::SceneGraphStandardItem(const QString& text) : QStandardItem(icons::getModelIcon(), text) {}

SceneGraphStandardItem::SceneGraphStandardItem(const QIcon& icon, const QString& text) : QStandardItem(icon, text) {}

SceneGraphStandardItem::SceneGraphStandardItem(const tesseract_scene_graph::SceneGraph& scene_graph, bool checkable)
  : QStandardItem(icons::getModelIcon(), "SceneGraph")
{
  ctor(scene_graph, checkable);
}

SceneGraphStandardItem::SceneGraphStandardItem(const QString& text,
                                               const tesseract_scene_graph::SceneGraph& scene_graph,
                                               bool checkable)
  : QStandardItem(icons::getModelIcon(), text)
{
  ctor(scene_graph, checkable);
}

SceneGraphStandardItem::SceneGraphStandardItem(const QIcon& icon,
                                               const QString& text,
                                               const tesseract_scene_graph::SceneGraph& scene_graph,
                                               bool checkable)
  : QStandardItem(icon, text)
{
  ctor(scene_graph, checkable);
}

int SceneGraphStandardItem::type() const { return static_cast<int>(StandardItemType::SG_SCENE_GRAPH); }

void SceneGraphStandardItem::ctor(const tesseract_scene_graph::SceneGraph& scene_graph, bool checkable)
{
  appendRow(createStandardItemString("name", scene_graph.getName()));
  appendRow(createStandardItemString("root_link", scene_graph.getRoot()));

  auto* links_item = new QStandardItem(icons::getLinkVectorIcon(), "Links");
  for (const auto& link : scene_graph.getLinks())
  {
    auto* item = new LinkStandardItem(QString::fromStdString(link->getName()),
                                      std::make_shared<tesseract_scene_graph::Link>(link->clone()),
                                      checkable);
    if (checkable)
    {
      item->setCheckable(true);
      item->setCheckState(Qt::CheckState::Checked);
    }
    links_item->appendRow(item);
  }
  links_item->sortChildren(0);
  appendRow(links_item);

  auto* joints_item = new QStandardItem(icons::getJointVectorIcon(), "Joints");
  for (const auto& joint : scene_graph.getJoints())
  {
    auto* item = new JointStandardItem(QString::fromStdString(joint->getName()),
                                       std::make_shared<tesseract_scene_graph::Joint>(joint->clone()));
    joints_item->appendRow(item);
  }
  joints_item->sortChildren(0);
  appendRow(joints_item);
}

}  // namespace tesseract_gui
