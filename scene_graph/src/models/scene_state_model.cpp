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
#include <tesseract_qt/scene_graph/models/scene_state_model.h>
#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/models/transform_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/link_visibility.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_scene_graph/scene_state.h>
#include <tesseract_environment/environment.h>

#include <QApplication>

namespace tesseract::gui
{
struct SceneStateModel::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;

  std::vector<std::string> link_names;
  std::vector<std::string> joint_names_tf;
  std::vector<std::string> joint_names_value;

  QStandardItem* values_item;
  QStandardItem* links_item;
  QStandardItem* joints_item;

  std::unordered_map<std::string, QStandardItem*> values;
  std::unordered_map<std::string, TransformStandardItem*> links;
  std::unordered_map<std::string, TransformStandardItem*> joints;

  void clear()
  {
    link_names.clear();
    joint_names_tf.clear();
    joint_names_value.clear();
    values_item = nullptr;
    links_item = nullptr;
    joints_item = nullptr;
    values.clear();
    links.clear();
    joints.clear();
  }
};

SceneStateModel::SceneStateModel(QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  clear();

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

SceneStateModel::SceneStateModel(std::shared_ptr<const ComponentInfo> component_info, QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  clear();
  data_->component_info = std::move(component_info);

  // If an environment has already been assigned load the data
  auto env_wrapper = EnvironmentManager::get(data_->component_info);
  if (env_wrapper != nullptr && env_wrapper->getEnvironment()->isInitialized())
    setState(env_wrapper->getEnvironment()->getState());

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

SceneStateModel::~SceneStateModel() = default;

SceneStateModel::SceneStateModel(const SceneStateModel& other)
  : SceneStateModel(other.data_->component_info, other.d_ptr->parent)
{
}

SceneStateModel& SceneStateModel::operator=(const SceneStateModel& other) { return *this; }

std::shared_ptr<const ComponentInfo> SceneStateModel::getComponentInfo() const { return data_->component_info; }

void SceneStateModel::setState(const tesseract::scene_graph::SceneState& scene_state)
{
  bool sort_required{ false };

  std::vector<std::string> joint_names_value;
  joint_names_value.reserve(scene_state.joints.size());
  for (const auto& joint : scene_state.joints)
  {
    auto it = data_->values.find(joint.first);
    if (it != data_->values.end())
    {
      it->second->setData(joint.second, Qt::DisplayRole);
    }
    else
    {
      auto row = createStandardItemFloat(joint.first, joint.second);
      data_->values[joint.first] = row[1];
      data_->values_item->appendRow(row);
      data_->joint_names_value.push_back(joint.first);
      sort_required = true;
    }
    joint_names_value.push_back(joint.first);
  }

  std::vector<std::string> link_names;
  link_names.reserve(scene_state.link_transforms.size());
  for (const auto& link : scene_state.link_transforms)
  {
    auto it = data_->links.find(link.first);
    if (it != data_->links.end())
    {
      it->second->setTransform(link.second);
    }
    else
    {
      auto* item = new TransformStandardItem(QString::fromStdString(link.first), link.second);
      item->setCheckable(true);
      item->setCheckState(Qt::CheckState::Unchecked);
      data_->links_item->appendRow(item);
      data_->links[link.first] = item;
      data_->link_names.push_back(link.first);

      sort_required = true;
    }
    link_names.push_back(link.first);
  }

  std::vector<std::string> joint_names_tf;
  joint_names_tf.reserve(scene_state.joint_transforms.size());
  for (const auto& joint : scene_state.joint_transforms)
  {
    auto it = data_->joints.find(joint.first);
    if (it != data_->joints.end())
    {
      it->second->setTransform(joint.second);
    }
    else
    {
      auto* item = new TransformStandardItem(QString::fromStdString(joint.first), joint.second);
      data_->joints_item->appendRow(item);
      data_->joints[joint.first] = item;
      data_->joint_names_tf.push_back(joint.first);
      sort_required = true;
    }
    joint_names_tf.push_back(joint.first);
  }

  for (const auto& link_name : data_->link_names)
  {
    auto it = scene_state.link_transforms.find(link_name);
    if (it == scene_state.link_transforms.end())
    {
      auto it = data_->links.find(link_name);
      if (it != data_->links.end())
      {
        QModelIndex idx = indexFromItem(data_->links[link_name]);
        removeRow(idx.row(), idx.parent());
        data_->links.erase(link_name);
        sort_required = true;
      }
    }
  }
  data_->link_names = link_names;

  for (const auto& joint_name : data_->joint_names_tf)
  {
    auto jt_it = scene_state.joint_transforms.find(joint_name);
    if (jt_it == scene_state.joint_transforms.end())
    {
      auto it = data_->joints.find(joint_name);
      if (it != data_->joints.end())
      {
        QModelIndex joint_idx = indexFromItem(it->second);
        removeRow(joint_idx.row(), joint_idx.parent());
        data_->joints.erase(joint_name);
        sort_required = true;
      }
    }
  }
  data_->joint_names_tf = joint_names_tf;

  for (const auto& joint_name : data_->joint_names_value)
  {
    auto jt_it = scene_state.joints.find(joint_name);
    if (jt_it == scene_state.joints.end())
    {
      auto it = data_->values.find(joint_name);
      if (it != data_->values.end())
      {
        QModelIndex joint_idx = indexFromItem(it->second);
        removeRow(joint_idx.row(), joint_idx.parent());
        data_->values.erase(joint_name);
        sort_required = true;
      }
    }
  }
  data_->joint_names_value = joint_names_value;

  if (sort_required)
  {
    data_->values_item->sortChildren(0);
    data_->links_item->sortChildren(0);
    data_->joints_item->sortChildren(0);
  }
}

bool SceneStateModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // Need emit application event to change visible
  if (role == Qt::CheckStateRole)
  {
    QStandardItem* item = itemFromIndex(index);
    if (item->type() == static_cast<int>(StandardItemType::COMMON_TRANSFORM))
    {
      assert(dynamic_cast<TransformStandardItem*>(item) != nullptr);
      auto* derived_item = static_cast<TransformStandardItem*>(item);
      events::SceneGraphModifyLinkVisibility event(data_->component_info,
                                                   { derived_item->text().toStdString() },
                                                   LinkVisibilityFlags::AXIS,
                                                   value.value<Qt::CheckState>() == Qt::Checked);
      QApplication::sendEvent(qApp, &event);
    }
  }
  return QStandardItemModel::setData(index, value, role);
}

void SceneStateModel::clear()
{
  QStandardItemModel::clear();
  data_->clear();

  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });

  data_->values_item = new QStandardItem(icons::getJointVectorIcon(), "Values");  // NOLINT
  data_->links_item = new QStandardItem(icons::getLinkVectorIcon(), "Links");     // NOLINT
  data_->joints_item = new QStandardItem(icons::getJointVectorIcon(), "Joints");  // NOLINT

  appendRow(data_->values_item);
  appendRow(data_->links_item);
  appendRow(data_->joints_item);
}

bool SceneStateModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::EventType::SCENE_GRAPH_CLEAR)
  {
    assert(dynamic_cast<events::SceneGraphClear*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphClear*>(event);
    if (e->getComponentInfo() == data_->component_info)
      clear();
  }
  else if (event->type() == events::EventType::SCENE_GRAPH_STATE_CHANGED)
  {
    assert(dynamic_cast<events::SceneStateChanged*>(event) != nullptr);
    auto* e = static_cast<events::SceneStateChanged*>(event);
    if (e->getComponentInfo() == data_->component_info)
      setState(e->getState());
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract::gui
