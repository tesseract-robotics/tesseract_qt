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
#include <shared_mutex>

namespace tesseract_gui
{
struct EnvironmentManager::Implementation
{
  std::map<std::shared_ptr<const ComponentInfo>, std::shared_ptr<EnvironmentWrapper>> environments;
  std::shared_ptr<const ComponentInfo> default_component_info;
  std::shared_mutex mutex;
};

EnvironmentManager::EnvironmentManager() : data_(std::make_unique<Implementation>()) {}

EnvironmentManager::~EnvironmentManager() = default;

std::shared_ptr<EnvironmentManager> EnvironmentManager::singleton = nullptr;
std::once_flag EnvironmentManager::init_instance_flag;
void EnvironmentManager::initSingleton() { singleton = std::make_shared<EnvironmentManager>(); }

void EnvironmentManager::set(std::shared_ptr<EnvironmentWrapper> env, bool set_default)
{
  std::shared_ptr<EnvironmentManager> obj = instance();
  std::unique_lock lock(obj->data_->mutex);
  obj->setHelper(env, set_default);
}

std::shared_ptr<EnvironmentWrapper> EnvironmentManager::get(const std::shared_ptr<const ComponentInfo>& component_info)
{
  std::shared_ptr<EnvironmentManager> obj = instance();
  std::shared_lock lock(obj->data_->mutex);
  return obj->getHelper(component_info);
}

std::map<std::shared_ptr<const ComponentInfo>, std::shared_ptr<EnvironmentWrapper>> EnvironmentManager::getAll()
{
  std::shared_ptr<EnvironmentManager> obj = instance();
  std::shared_lock lock(obj->data_->mutex);
  return obj->data_->environments;
}

std::shared_ptr<EnvironmentWrapper> EnvironmentManager::find(const std::shared_ptr<const ComponentInfo>& component_info)
{
  std::shared_ptr<EnvironmentManager> obj = instance();
  std::shared_lock lock(obj->data_->mutex);
  return obj->findHelper(component_info);
}

std::shared_ptr<EnvironmentWrapper> EnvironmentManager::getDefault()
{
  std::shared_ptr<EnvironmentManager> obj = instance();
  std::shared_lock lock(obj->data_->mutex);
  return obj->getDefaultHelper();
}

void EnvironmentManager::setDefault(std::shared_ptr<const ComponentInfo> component_info)
{
  std::shared_ptr<EnvironmentManager> obj = instance();
  std::unique_lock lock(obj->data_->mutex);
  obj->setDefaultHelper(component_info);
}

void EnvironmentManager::remove(const std::shared_ptr<const ComponentInfo>& component_info)
{
  std::shared_ptr<EnvironmentManager> obj = instance();
  std::unique_lock lock(obj->data_->mutex);
  obj->removeHelper(component_info);
}

std::shared_ptr<EnvironmentManager> EnvironmentManager::instance()
{
  std::call_once(init_instance_flag, &EnvironmentManager::initSingleton);
  return singleton;
}

void EnvironmentManager::setHelper(std::shared_ptr<EnvironmentWrapper> env, bool set_default)
{
  std::shared_ptr<const ComponentInfo> component_info = env->getComponentInfo();

  // Remove any existing wrappers associated with component info
  removeHelper(component_info);

  // Initialize environment wrapper
  env->init();

  // Set default if not already set
  if (data_->environments.empty() || set_default)
    data_->default_component_info = component_info;

  // Store the wrapper
  data_->environments[component_info] = env;
}

std::shared_ptr<EnvironmentWrapper>
EnvironmentManager::getHelper(const std::shared_ptr<const ComponentInfo>& component_info) const
{
  auto it = data_->environments.find(component_info);
  if (it == data_->environments.end())
    return nullptr;

  return it->second;
}

std::shared_ptr<EnvironmentWrapper>
EnvironmentManager::findHelper(const std::shared_ptr<const ComponentInfo>& component_info) const
{
  auto env_wrapper = getHelper(component_info);
  if (env_wrapper != nullptr)
    return env_wrapper;

  if (component_info->hasParent())
    return findHelper(component_info->getParentComponentInfo());

  return nullptr;
}

void EnvironmentManager::setDefaultHelper(const std::shared_ptr<const ComponentInfo>& component_info)
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

void EnvironmentManager::removeHelper(const std::shared_ptr<const ComponentInfo>& component_info)
{
  bool update_default{ false };
  std::vector<std::shared_ptr<const ComponentInfo>> remove_component_infos;
  for (auto& env : data_->environments)
  {
    if (component_info == env.first || env.first->isParent(component_info))
    {
      remove_component_infos.push_back(env.first);

      if (env.first == data_->default_component_info)
        update_default = true;
    }
  }

  for (const auto& rci : remove_component_infos)
    data_->environments.erase(rci);

  if (update_default)
  {
    data_->default_component_info = nullptr;
    if (!data_->environments.empty())
      data_->default_component_info = data_->environments.begin()->first;
  }
}

}  // namespace tesseract_gui
