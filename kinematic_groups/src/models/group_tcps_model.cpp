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
#include <tesseract_qt/kinematic_groups/models/group_tcps_model.h>
#include <tesseract_qt/kinematic_groups/models/group_tcps_standard_item.h>
#include <tesseract_qt/common/events/group_tcps_events.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_environment/environment.h>

#include <QApplication>

namespace tesseract_gui
{
GroupTCPsModel::GroupTCPsModel(QObject* parent) : GroupTCPsModel(ComponentInfo(), parent) {}

GroupTCPsModel::GroupTCPsModel(ComponentInfo component_info, QObject* parent)
  : QStandardItemModel(parent), component_info_(std::make_unique<ComponentInfo>(std::move(component_info)))
{
  clear();

  // If an environment has already been assigned load the data
  auto env_wrapper = EnvironmentManager::get(*component_info_);
  if (env_wrapper != nullptr && env_wrapper->getEnvironment()->isInitialized())
    set(env_wrapper->getEnvironment()->getKinematicsInformation().group_tcps);

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}
GroupTCPsModel::GroupTCPsModel(const GroupTCPsModel& other) : QStandardItemModel(other.d_ptr->parent) {}

GroupTCPsModel& GroupTCPsModel::operator=(const GroupTCPsModel& /*other*/) { return *this; }

GroupTCPsModel::~GroupTCPsModel() = default;

const ComponentInfo& GroupTCPsModel::getComponentInfo() const { return *component_info_; }

void GroupTCPsModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  appendRow(new GroupTCPsStandardItem());
}

void GroupTCPsModel::set(const tesseract_srdf::GroupTCPs& group_tcps)
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  appendRow(new GroupTCPsStandardItem(group_tcps));
}

void GroupTCPsModel::add(const std::string& group_name, const std::string& tcp_name, const Eigen::Isometry3d& tcp)
{
  getRoot()->addGroupTCP(QString::fromStdString(group_name), QString::fromStdString(tcp_name), tcp);
}

void GroupTCPsModel::remove(const std::string& group_name, const std::string& tcp_name)
{
  getRoot()->removeGroupTCP(QString::fromStdString(group_name), QString::fromStdString(tcp_name));
}

void GroupTCPsModel::remove(const std::string& group_name)
{
  getRoot()->removeGroup(QString::fromStdString(group_name));
}

const tesseract_srdf::GroupTCPs& GroupTCPsModel::getGroupTCPs() const { return getRoot()->getGroupTCPs(); }

GroupTCPsStandardItem* GroupTCPsModel::getRoot() { return dynamic_cast<GroupTCPsStandardItem*>(item(0)); }

const GroupTCPsStandardItem* GroupTCPsModel::getRoot() const
{
  return dynamic_cast<const GroupTCPsStandardItem*>(item(0));
}

bool GroupTCPsModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::GroupTCPsSet::kType)
  {
    assert(dynamic_cast<events::GroupTCPsSet*>(event) != nullptr);
    auto* e = static_cast<events::GroupTCPsSet*>(event);
    if (e->getComponentInfo() == *component_info_)
      set(e->getGroupTCPs());
  }
  else if (event->type() == events::GroupTCPsAdd::kType)
  {
    assert(dynamic_cast<events::GroupTCPsAdd*>(event) != nullptr);
    auto* e = static_cast<events::GroupTCPsAdd*>(event);
    if (e->getComponentInfo() == *component_info_)
      add(e->getGroupName(), e->getTCPName(), e->getTCP());
  }
  else if (event->type() == events::GroupTCPsClear::kType)
  {
    assert(dynamic_cast<events::GroupTCPsClear*>(event) != nullptr);
    auto* e = static_cast<events::GroupTCPsClear*>(event);
    if (e->getComponentInfo() == *component_info_)
      clear();
  }
  else if (event->type() == events::GroupTCPsRemove::kType)
  {
    assert(dynamic_cast<events::GroupTCPsRemove*>(event) != nullptr);
    auto* e = static_cast<events::GroupTCPsRemove*>(event);
    if (e->getComponentInfo() == *component_info_)
    {
      for (const auto& entry : e->getEntries())
        remove(entry[0], entry[1]);
    }
  }
  else if (event->type() == events::GroupTCPsRemoveGroup::kType)
  {
    assert(dynamic_cast<events::GroupTCPsRemoveGroup*>(event) != nullptr);
    auto* e = static_cast<events::GroupTCPsRemoveGroup*>(event);
    if (e->getComponentInfo() == *component_info_)
    {
      for (const auto& link_name : e->getGroupNames())
        remove(link_name);
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

}  // namespace tesseract_gui
