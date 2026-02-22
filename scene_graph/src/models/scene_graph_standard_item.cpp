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
#include <tesseract_qt/scene_graph/models/scene_graph_standard_item.h>
#include <tesseract_qt/scene_graph/models/link_standard_item.h>
#include <tesseract_qt/scene_graph/models/joint_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_scene_graph/graph.h>
#include <tesseract_scene_graph/link.h>
#include <tesseract_scene_graph/joint.h>

namespace tesseract::gui
{
class SceneGraphStandardItem::Implementation
{
public:
  QList<QStandardItem*> name_item;
  QStandardItem* links_item;
  QStandardItem* joints_item;
  std::unordered_map<std::string, LinkStandardItem*> links;
  std::unordered_map<std::string, JointStandardItem*> joints;
  bool checkable{ true };
};

SceneGraphStandardItem::SceneGraphStandardItem(bool checkable)
  : QStandardItem(icons::getModelIcon(), "SceneGraph"), data_(std::make_unique<Implementation>())
{
  ctor(checkable);
}

SceneGraphStandardItem::SceneGraphStandardItem(const QString& text, bool checkable)
  : QStandardItem(icons::getModelIcon(), text), data_(std::make_unique<Implementation>())
{
  ctor(checkable);
}

SceneGraphStandardItem::SceneGraphStandardItem(const QIcon& icon, const QString& text, bool checkable)
  : QStandardItem(icon, text), data_(std::make_unique<Implementation>())
{
  ctor(checkable);
}

void SceneGraphStandardItem::setSceneGraph(const tesseract::scene_graph::SceneGraph& scene_graph)
{
  clear();

  data_->name_item[1]->setData(scene_graph.getName().c_str(), Qt::DisplayRole);

  for (const auto& link : scene_graph.getLinks())
    addLink(*link);

  for (const auto& joint : scene_graph.getJoints())
    addJoint(*joint);
}

void SceneGraphStandardItem::setName(const std::string& name)
{
  data_->name_item[1]->setData(name.c_str(), Qt::DisplayRole);
}

void SceneGraphStandardItem::addLink(const tesseract::scene_graph::Link& link)
{
  auto* item = new LinkStandardItem(
      QString::fromStdString(link.getName()), std::make_shared<tesseract::scene_graph::Link>(link.clone()), true);

  if (data_->checkable)
  {
    item->setCheckable(true);
    item->setCheckState(Qt::CheckState::Checked);
  }
  data_->links_item->appendRow(item);
  data_->links_item->sortChildren(0);
  data_->links[link.getName()] = item;
}

void SceneGraphStandardItem::addJoint(const tesseract::scene_graph::Joint& joint)
{
  auto* item = new JointStandardItem(QString::fromStdString(joint.getName()),
                                     std::make_shared<tesseract::scene_graph::Joint>(joint.clone()));
  data_->joints_item->appendRow(item);
  data_->joints_item->sortChildren(0);
  data_->joints[joint.getName()] = item;
}

void SceneGraphStandardItem::removeLink(const std::string& link_name)
{
  QStandardItem* item = data_->links.at(link_name);
  for (int i = 0; i < data_->links_item->rowCount(); ++i)
  {
    if (item == data_->links_item->child(i))
    {
      data_->links_item->removeRow(i);
      data_->links.erase(link_name);
      break;
    }
  }
}

void SceneGraphStandardItem::removeJoint(const std::string& joint_name)
{
  QStandardItem* item = data_->joints.at(joint_name);
  for (int i = 0; i < data_->joints_item->rowCount(); ++i)
  {
    if (item == data_->joints_item->child(i))
    {
      data_->joints_item->removeRow(i);
      data_->joints.erase(joint_name);
      break;
    }
  }
}

const std::unordered_map<std::string, LinkStandardItem*>& SceneGraphStandardItem::getLinks() const
{
  return data_->links;
}

const std::unordered_map<std::string, JointStandardItem*>& SceneGraphStandardItem::getJoints() const
{
  return data_->joints;
}

int SceneGraphStandardItem::type() const { return static_cast<int>(StandardItemType::SG_SCENE_GRAPH); }

void SceneGraphStandardItem::ctor(bool checkable)
{
  data_->name_item = createStandardItemString("name", "Empty");
  appendRow(data_->name_item);

  data_->links_item = new QStandardItem(icons::getLinkVectorIcon(), "Links");
  appendRow(data_->links_item);

  data_->joints_item = new QStandardItem(icons::getJointVectorIcon(), "Joints");
  appendRow(data_->joints_item);

  data_->checkable = checkable;
}

void SceneGraphStandardItem::clear()
{
  data_->name_item[1]->setData("Empty", Qt::DisplayRole);

  for (int i = data_->links_item->rowCount(); i > 0; --i)
    data_->links_item->removeRow(i);

  for (int i = data_->joints_item->rowCount(); i > 0; --i)
    data_->joints_item->removeRow(i);

  data_->links.clear();
  data_->joints.clear();
}

}  // namespace tesseract::gui
