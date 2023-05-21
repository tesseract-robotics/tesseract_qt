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
#include <tesseract_qt/environment/models/environment_commands_model.h>
#include <tesseract_qt/environment/models/environment_commands_standard_item.h>
#include <tesseract_qt/common/events/environment_events.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_environment/environment.h>

#include <QApplication>

namespace tesseract_gui
{
EnvironmentCommandsModel::EnvironmentCommandsModel(QObject* parent) : EnvironmentCommandsModel(nullptr, parent) {}

EnvironmentCommandsModel::EnvironmentCommandsModel(std::shared_ptr<const ComponentInfo> component_info, QObject* parent)
  : QStandardItemModel(parent), component_info_(std::move(component_info))
{
  clear();

  // If an environment has already been assigned load the data
  auto env_wrapper = EnvironmentManager::get(component_info_);
  if (env_wrapper != nullptr && env_wrapper->getEnvironment()->isInitialized())
    set(env_wrapper->getEnvironment()->getCommandHistory());

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

EnvironmentCommandsModel::EnvironmentCommandsModel(const EnvironmentCommandsModel& other)
  : QStandardItemModel(other.d_ptr->parent)
{
}

EnvironmentCommandsModel& EnvironmentCommandsModel::operator=(const EnvironmentCommandsModel& /*other*/)
{
  return *this;
}

EnvironmentCommandsModel::~EnvironmentCommandsModel() = default;

std::shared_ptr<const ComponentInfo> EnvironmentCommandsModel::getComponentInfo() const { return component_info_; }

void EnvironmentCommandsModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  appendRow(new EnvironmentCommandsStandardItem());
}

void EnvironmentCommandsModel::set(const tesseract_environment::Commands& commands)
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  appendRow(new EnvironmentCommandsStandardItem(commands));
}

void EnvironmentCommandsModel::appendCommand(const tesseract_environment::Command::ConstPtr& command)
{
  getRoot()->appendCommand(command);
}

const tesseract_environment::Commands& EnvironmentCommandsModel::getCommands() const
{
  return getRoot()->getCommands();
}

EnvironmentCommandsStandardItem* EnvironmentCommandsModel::getRoot()
{
  return dynamic_cast<EnvironmentCommandsStandardItem*>(item(0));
}

const EnvironmentCommandsStandardItem* EnvironmentCommandsModel::getRoot() const
{
  return dynamic_cast<const EnvironmentCommandsStandardItem*>(item(0));
}

bool EnvironmentCommandsModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::EnvironmentCommandsSet::kType)
  {
    assert(dynamic_cast<events::EnvironmentCommandsSet*>(event) != nullptr);
    auto* e = static_cast<events::EnvironmentCommandsSet*>(event);
    if (e->getComponentInfo() == component_info_)
      set(e->getCommands());
  }
  else if (event->type() == events::EnvironmentCommandsAppend::kType)
  {
    assert(dynamic_cast<events::EnvironmentCommandsAppend*>(event) != nullptr);
    auto* e = static_cast<events::EnvironmentCommandsAppend*>(event);
    if (e->getComponentInfo() == component_info_)
    {
      for (const auto& command : e->getCommands())
        appendCommand(command);
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

}  // namespace tesseract_gui
