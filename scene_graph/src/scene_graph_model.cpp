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
#include <tesseract_qt/scene_graph/scene_graph_model.h>
#include <tesseract_qt/scene_graph/scene_graph_standard_item.h>
#include <tesseract_qt/scene_graph/link_standard_item.h>
#include <tesseract_qt/scene_graph/joint_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_scene_graph/graph.h>

namespace tesseract_gui
{
struct SceneGraphModelImpl
{
  tesseract_scene_graph::SceneGraph::UPtr scene_graph;
  SceneGraphStandardItem* scene_graph_item;
  std::unordered_map<std::string, LinkStandardItem*> links;

  void clear()
  {
    scene_graph = nullptr;
    scene_graph_item = nullptr;
    links.clear();
  }
};

SceneGraphModel::SceneGraphModel(QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<SceneGraphModelImpl>())
{
  clear();
}

SceneGraphModel::~SceneGraphModel() = default;

SceneGraphModel::SceneGraphModel(const SceneGraphModel& other) : SceneGraphModel(other.d_ptr->parent) {}

SceneGraphModel& SceneGraphModel::operator=(const SceneGraphModel& other) { return *this; }

void SceneGraphModel::setSceneGraph(std::unique_ptr<tesseract_scene_graph::SceneGraph> scene_graph)
{
  clear();
  data_->scene_graph = std::move(scene_graph);

  data_->scene_graph_item = new SceneGraphStandardItem();  // NOLINT
  data_->scene_graph_item->appendRow(createStandardItemString("name", data_->scene_graph->getName()));
  data_->scene_graph_item->appendRow(createStandardItemString("root_link", data_->scene_graph->getRoot()));

  auto* links_item = new QStandardItem(icons::getLinkVectorIcon(), "Links");
  for (const auto& link : data_->scene_graph->getLinks())
  {
    auto* item = new LinkStandardItem(
        QString::fromStdString(link->getName()), std::make_shared<tesseract_scene_graph::Link>(link->clone()), true);

    item->setCheckable(true);
    item->setCheckState(Qt::CheckState::Checked);
    links_item->appendRow(item);

    data_->links[link->getName()] = item;
  }
  links_item->sortChildren(0);
  data_->scene_graph_item->appendRow(links_item);

  auto* joints_item = new QStandardItem(icons::getJointVectorIcon(), "Joints");
  for (const auto& joint : data_->scene_graph->getJoints())
  {
    auto* item = new JointStandardItem(QString::fromStdString(joint->getName()),
                                       std::make_shared<tesseract_scene_graph::Joint>(joint->clone()));
    joints_item->appendRow(item);
  }
  joints_item->sortChildren(0);
  data_->scene_graph_item->appendRow(joints_item);

  appendRow(data_->scene_graph_item);
}

void SceneGraphModel::clear()
{
  QStandardItemModel::clear();
  data_->clear();

  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
}

void SceneGraphModel::onLinkCheckedStateChanged(const QString& link_name, bool checked)
{
  auto it = data_->links.find(link_name.toStdString());
  if (it != data_->links.end())
    it->second->setCheckState((checked) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

void SceneGraphModel::onLinkVisualsCheckedStateChanged(const QString& link_name, bool checked)
{
  auto it = data_->links.find(link_name.toStdString());
  if (it != data_->links.end())
  {
    for (int i = 0; i < it->second->rowCount(); ++i)
    {
      QStandardItem* child = it->second->child(i);
      if (child->type() == static_cast<int>(StandardItemType::SG_VISUALS))
        child->setCheckState((checked) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    }
  }
}

void SceneGraphModel::onLinkCollisionsCheckedStateChanged(const QString& link_name, bool checked)
{
  auto it = data_->links.find(link_name.toStdString());
  if (it != data_->links.end())
  {
    for (int i = 0; i < it->second->rowCount(); ++i)
    {
      QStandardItem* child = it->second->child(i);
      if (child->type() == static_cast<int>(StandardItemType::SG_COLLISIONS))
        child->setCheckState((checked) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    }
  }
}

}  // namespace tesseract_gui
