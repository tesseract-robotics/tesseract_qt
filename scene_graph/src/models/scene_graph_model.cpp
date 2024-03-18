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
#include <tesseract_qt/scene_graph/models/scene_graph_model.h>
#include <tesseract_qt/scene_graph/models/scene_graph_standard_item.h>
#include <tesseract_qt/scene_graph/models/link_standard_item.h>
#include <tesseract_qt/scene_graph/models/joint_standard_item.h>
#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/link_visibility.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_scene_graph/graph.h>
#include <tesseract_scene_graph/link.h>
#include <tesseract_scene_graph/joint.h>

#include <tesseract_environment/environment.h>

#include <QApplication>

namespace tesseract_gui
{
struct SceneGraphModel::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;

  SceneGraphStandardItem* scene_graph_item;

  void clear() { scene_graph_item->clear(); }
};

SceneGraphModel::SceneGraphModel(QObject* parent) : SceneGraphModel(nullptr, parent) {}

SceneGraphModel::SceneGraphModel(std::shared_ptr<const ComponentInfo> component_info, QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });

  data_->component_info = std::move(component_info);
  data_->scene_graph_item = new SceneGraphStandardItem();  // NOLINT
  appendRow(data_->scene_graph_item);

  // If an environment has already been assigned load the data
  auto env_wrapper = EnvironmentManager::get(data_->component_info);
  if (env_wrapper != nullptr && env_wrapper->getEnvironment()->isInitialized())
    setSceneGraph(*env_wrapper->getEnvironment()->getSceneGraph());

  // Install event filter
  qGuiApp->installEventFilter(this);
}

SceneGraphModel::~SceneGraphModel() = default;

SceneGraphModel::SceneGraphModel(const SceneGraphModel& other)
  : SceneGraphModel(other.data_->component_info, other.d_ptr->parent)
{
  // Install event filter
  qGuiApp->installEventFilter(this);
}

SceneGraphModel& SceneGraphModel::operator=(const SceneGraphModel& other) { return *this; }

std::shared_ptr<const ComponentInfo> SceneGraphModel::getComponentInfo() const { return data_->component_info; }

void SceneGraphModel::setSceneGraph(const tesseract_scene_graph::SceneGraph& scene_graph)
{
  clear();

  data_->scene_graph_item->setSceneGraph(scene_graph);
}

void SceneGraphModel::setName(const std::string& name) { data_->scene_graph_item->setName(name); }

void SceneGraphModel::addLink(const tesseract_scene_graph::Link& link) { data_->scene_graph_item->addLink(link); }

void SceneGraphModel::addJoint(const tesseract_scene_graph::Joint& joint) { data_->scene_graph_item->addJoint(joint); }

void SceneGraphModel::removeLink(const std::string& link_name) { data_->scene_graph_item->removeLink(link_name); }

void SceneGraphModel::removeJoint(const std::string& joint_name) { data_->scene_graph_item->removeJoint(joint_name); }

bool SceneGraphModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // Need emit application event to change visible
  if (role == Qt::CheckStateRole)
  {
    QStandardItem* item = itemFromIndex(index);
    if (item->type() == static_cast<int>(StandardItemType::SG_LINK))
    {
      assert(dynamic_cast<LinkStandardItem*>(item) != nullptr);
      auto* derived_item = static_cast<LinkStandardItem*>(item);
      events::SceneGraphModifyLinkVisibility event(data_->component_info,
                                                   { derived_item->link->getName() },
                                                   LinkVisibilityFlags::LINK,
                                                   value.value<Qt::CheckState>() == Qt::Checked);
      QApplication::sendEvent(qApp, &event);
    }
    else if (item->type() == static_cast<int>(StandardItemType::SG_VISUALS))
    {
      assert(dynamic_cast<LinkStandardItem*>(item->parent()) != nullptr);
      auto* derived_item = static_cast<LinkStandardItem*>(item->parent());
      events::SceneGraphModifyLinkVisibility event(data_->component_info,
                                                   { derived_item->link->getName() },
                                                   LinkVisibilityFlags::VISUAL,
                                                   value.value<Qt::CheckState>() == Qt::Checked);
      QApplication::sendEvent(qApp, &event);
    }
    else if (item->type() == static_cast<int>(StandardItemType::SG_COLLISIONS))
    {
      assert(dynamic_cast<LinkStandardItem*>(item->parent()) != nullptr);
      auto* derived_item = static_cast<LinkStandardItem*>(item->parent());
      events::SceneGraphModifyLinkVisibility event(data_->component_info,
                                                   { derived_item->link->getName() },
                                                   LinkVisibilityFlags::COLLISION,
                                                   value.value<Qt::CheckState>() == Qt::Checked);
      QApplication::sendEvent(qApp, &event);
    }
  }
  return QStandardItemModel::setData(index, value, role);
}

void SceneGraphModel::clear() { data_->clear(); }

bool SceneGraphModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::SceneGraphClear::kType)
  {
    assert(dynamic_cast<events::SceneGraphClear*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphClear*>(event);
    if (e->getComponentInfo() == data_->component_info)
      clear();
  }
  else if (event->type() == events::SceneGraphSet::kType)
  {
    assert(dynamic_cast<events::SceneGraphSet*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphSet*>(event);
    if (e->getComponentInfo() == data_->component_info)
      setSceneGraph(*e->getSceneGraph());
  }
  else if (event->type() == events::SceneGraphAddLink::kType)
  {
    assert(dynamic_cast<events::SceneGraphAddLink*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphAddLink*>(event);
    if (e->getComponentInfo() == data_->component_info)
      addLink(*e->getLink());
  }
  else if (event->type() == events::SceneGraphAddJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphAddJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphAddJoint*>(event);
    if (e->getComponentInfo() == data_->component_info)
      addJoint(*e->getJoint());
  }
  else if (event->type() == events::SceneGraphMoveLink::kType)
  {
    assert(dynamic_cast<events::SceneGraphMoveLink*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphMoveLink*>(event);
    if (e->getComponentInfo() == data_->component_info)
    {
      auto joint = e->getJoint();
      std::string child_link_name = joint->child_link_name;
      for (const auto& joint : data_->scene_graph_item->getJoints())
      {
        if (joint.second->joint->child_link_name == child_link_name)
        {
          removeJoint(joint.first);
          break;
        }
      }
      addJoint(*joint);
    }
  }
  else if (event->type() == events::SceneGraphMoveJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphMoveJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphMoveJoint*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->scene_graph_item->getJoints().at(e->getJointName())->setParentLink(e->getParentLink().c_str());
  }
  else if (event->type() == events::SceneGraphRemoveLink::kType)
  {
    assert(dynamic_cast<events::SceneGraphRemoveLink*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphRemoveLink*>(event);
    if (e->getComponentInfo() == data_->component_info)
      removeLink(e->getLinkName());
  }
  else if (event->type() == events::SceneGraphRemoveJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphRemoveJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphRemoveJoint*>(event);
    if (e->getComponentInfo() == data_->component_info)
      removeJoint(e->getJointName());
  }
  else if (event->type() == events::SceneGraphReplaceJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphReplaceJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphReplaceJoint*>(event);
    if (e->getComponentInfo() == data_->component_info)
    {
      removeJoint(e->getJoint()->getName());
      addJoint(*e->getJoint());
    }
  }
  else if (event->type() == events::SceneGraphModifyLinkVisibility::kType)
  {
    assert(dynamic_cast<events::SceneGraphModifyLinkVisibility*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphModifyLinkVisibility*>(event);
    auto flags = e->getVisibilityFlags();
    if (e->getComponentInfo() == data_->component_info)
    {
      const auto& link_items = data_->scene_graph_item->getLinks();
      for (const auto& link_name : e->getLinkNames())
      {
        auto it = link_items.find(link_name);
        if (it != link_items.end())
        {
          Qt::CheckState checked_state = (e->visible()) ? Qt::Checked : Qt::Unchecked;
          if (flags & LinkVisibilityFlags::LINK)
            it->second->setCheckState(checked_state);

          if (flags & LinkVisibilityFlags::COLLISION)
            it->second->getCollisionsItem()->setCheckState(checked_state);

          if (flags & LinkVisibilityFlags::VISUAL)
            it->second->getVisualsItem()->setCheckState(checked_state);
        }
      }
    }
  }
  else if (event->type() == events::SceneGraphModifyLinkVisibilityALL::kType)
  {
    assert(dynamic_cast<events::SceneGraphModifyLinkVisibilityALL*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphModifyLinkVisibilityALL*>(event);
    auto flags = e->getVisibilityFlags();
    if (e->getComponentInfo() == data_->component_info)
    {
      const auto& link_items = data_->scene_graph_item->getLinks();
      Qt::CheckState checked_state = (e->visible()) ? Qt::Checked : Qt::Unchecked;
      for (const auto& link_item : link_items)
      {
        if (flags & LinkVisibilityFlags::LINK)
          link_item.second->setCheckState(checked_state);

        if (flags & LinkVisibilityFlags::COLLISION && link_item.second->getCollisionsItem() != nullptr)
          link_item.second->getCollisionsItem()->setCheckState(checked_state);

        if (flags & LinkVisibilityFlags::VISUAL && link_item.second->getVisualsItem() != nullptr)
          link_item.second->getVisualsItem()->setCheckState(checked_state);
      }
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

}  // namespace tesseract_gui
