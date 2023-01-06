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

#include <map>
#include <unordered_map>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>
#include <tesseract_qt/common/component_info.h>

namespace tesseract_gui
{
struct EnvironmentManager::Implementation
{
  std::unordered_map<ComponentInfo, std::shared_ptr<EnvironmentWrapper>> environments;
  ComponentInfo default_component_info;
};

EnvironmentManager::EnvironmentManager() : data_(std::make_unique<Implementation>()) {}

EnvironmentManager::~EnvironmentManager() = default;

void EnvironmentManager::set(std::shared_ptr<EnvironmentWrapper> env, bool set_default)
{
  instance()->setHelper(env, set_default);
}

std::shared_ptr<EnvironmentWrapper> EnvironmentManager::get(const ComponentInfo& component_info)
{
  return instance()->getHelper(component_info);
}

std::unordered_map<ComponentInfo, std::shared_ptr<EnvironmentWrapper>> EnvironmentManager::getAll()
{
  return instance()->data_->environments;
}

std::shared_ptr<EnvironmentWrapper> EnvironmentManager::find(const ComponentInfo& component_info)
{
  return instance()->findHelper(component_info);
}

std::shared_ptr<EnvironmentWrapper> EnvironmentManager::getDefault() { return instance()->getDefaultHelper(); }

void EnvironmentManager::setDefault(const ComponentInfo& component_info)
{
  instance()->setDefaultHelper(component_info);
}

std::shared_ptr<EnvironmentManager> EnvironmentManager::instance()
{
  static std::shared_ptr<EnvironmentManager> singleton = nullptr;
  if (singleton == nullptr)
    singleton = std::make_shared<EnvironmentManager>();

  return singleton;
}

void EnvironmentManager::setHelper(std::shared_ptr<EnvironmentWrapper> env, bool set_default)
{
  ComponentInfo component_info = env->getComponentInfo();
  if (data_->environments.empty() || set_default)
    data_->default_component_info = component_info;

  data_->environments[component_info] = env;
}

std::shared_ptr<EnvironmentWrapper> EnvironmentManager::getHelper(const ComponentInfo& component_info) const
{
  auto it = data_->environments.find(component_info);
  if (it == data_->environments.end())
    return nullptr;

  return it->second;
}

std::shared_ptr<EnvironmentWrapper> EnvironmentManager::findHelper(const ComponentInfo& component_info) const
{
  auto env_wrapper = getHelper(component_info);
  if (env_wrapper != nullptr)
    return env_wrapper;

  if (component_info.hasParent())
    return findHelper(*component_info.getParent());

  return nullptr;
}

void EnvironmentManager::setDefaultHelper(const ComponentInfo& component_info)
{
  auto it = data_->environments.find(component_info);
  if (it == data_->environments.end())
    return;

  data_->default_component_info = component_info;
}

std::shared_ptr<EnvironmentWrapper> EnvironmentManager::getDefaultHelper() const
{
  return getHelper(data_->default_component_info);
}

}  // namespace tesseract_gui
