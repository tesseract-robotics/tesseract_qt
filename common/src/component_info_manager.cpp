/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2023 Levi Armstrong <levi.armstrong@gmail.com>
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
#include <tesseract_qt/common/component_info_manager.h>
#include <tesseract_qt/common/component_info.h>
#include <boost/uuid/uuid_hash.hpp>
#include <shared_mutex>

namespace tesseract_gui
{
struct ComponentInfoManager::Implementation
{
  std::unordered_map<boost::uuids::uuid, std::shared_ptr<ComponentInfo>> component_infos_by_ns;
  std::unordered_map<std::string, std::shared_ptr<ComponentInfo>> component_infos_by_name;
  long cnt{ 0 };
  std::shared_mutex mutex;
};

ComponentInfoManager::ComponentInfoManager() : data_(std::make_unique<Implementation>()) {}

ComponentInfoManager::~ComponentInfoManager() = default;

std::shared_ptr<ComponentInfo> ComponentInfoManager::create(const std::string& scene_name)
{
  std::shared_ptr<ComponentInfoManager> obj = instance();
  std::unique_lock lock(obj->data_->mutex);
  return obj->createHelper(scene_name);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::create(const std::string& scene_name, const std::string& name)
{
  std::shared_ptr<ComponentInfoManager> obj = instance();
  std::unique_lock lock(obj->data_->mutex);
  return obj->createHelper(scene_name, name);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createChild(std::shared_ptr<const ComponentInfo> parent)
{
  std::shared_ptr<ComponentInfoManager> obj = instance();
  std::unique_lock lock(obj->data_->mutex);
  return obj->createChildHelper(parent);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createChild(std::shared_ptr<const ComponentInfo> parent,
                                                                 const std::string& name)
{
  std::shared_ptr<ComponentInfoManager> obj = instance();
  std::unique_lock lock(obj->data_->mutex);
  return obj->createChildHelper(parent, name);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createChild(const std::string& parent_name)
{
  std::shared_ptr<ComponentInfoManager> obj = instance();
  std::unique_lock lock(obj->data_->mutex);
  return obj->createChildHelper(parent_name);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createChild(const std::string& parent_name,
                                                                 const std::string& name)
{
  std::shared_ptr<ComponentInfoManager> obj = instance();
  std::unique_lock lock(obj->data_->mutex);
  return obj->createChildHelper(parent_name, name);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createChild(const boost::uuids::uuid& parent_ns)
{
  std::shared_ptr<ComponentInfoManager> obj = instance();
  std::unique_lock lock(obj->data_->mutex);
  return obj->createChildHelper(parent_ns);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createChild(const boost::uuids::uuid& parent_ns,
                                                                 const std::string& name)
{
  std::shared_ptr<ComponentInfoManager> obj = instance();
  std::unique_lock lock(obj->data_->mutex);
  return obj->createChildHelper(parent_ns, name);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::get(const boost::uuids::uuid& ns)
{
  std::shared_ptr<ComponentInfoManager> obj = instance();
  std::shared_lock lock(obj->data_->mutex);
  return obj->getHelper(ns);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::get(const std::string& name)
{
  std::shared_ptr<ComponentInfoManager> obj = instance();
  std::shared_lock lock(obj->data_->mutex);
  return obj->getHelper(name);
}

void ComponentInfoManager::removeUnused()
{
  std::shared_ptr<ComponentInfoManager> obj = instance();
  std::unique_lock lock(obj->data_->mutex);
  obj->removeUnusedHelper();
}

std::shared_ptr<ComponentInfoManager> ComponentInfoManager::instance()
{
  static std::shared_ptr<ComponentInfoManager> singleton = nullptr;
  if (singleton == nullptr)
    singleton = std::make_shared<ComponentInfoManager>();

  return singleton;
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createHelper(const std::string& scene_name)
{
  std::string name = getUniqueName();
  return createHelper(scene_name, name);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createHelper(const std::string& scene_name,
                                                                  const std::string& name)
{
  auto it = data_->component_infos_by_name.find(name);
  if (it != data_->component_infos_by_name.end())
    return nullptr;

  auto ci = std::shared_ptr<ComponentInfo>(new ComponentInfo(scene_name, name));
  data_->component_infos_by_ns[ci->getNamespace()] = ci;
  data_->component_infos_by_name[name] = ci;
  return ci;
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createChildHelper(std::shared_ptr<const ComponentInfo> parent)
{
  std::string name = getUniqueName();
  return createChildHelper(std::move(parent), name);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createChildHelper(std::shared_ptr<const ComponentInfo> parent,
                                                                       const std::string& name)
{
  auto it = data_->component_infos_by_name.find(name);
  if (it != data_->component_infos_by_name.end())
    return nullptr;

  auto ci = std::shared_ptr<ComponentInfo>(new ComponentInfo(parent->getSceneName(), name, parent));
  data_->component_infos_by_ns[ci->getNamespace()] = ci;
  data_->component_infos_by_name[name] = ci;
  return ci;
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createChildHelper(const std::string& parent_name)
{
  std::string name = getUniqueName();
  return createChildHelper(parent_name, name);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createChildHelper(const std::string& parent_name,
                                                                       const std::string& name)
{
  auto it = data_->component_infos_by_name.find(name);
  if (it != data_->component_infos_by_name.end())
    return nullptr;

  auto parent_it = data_->component_infos_by_name.find(parent_name);
  if (parent_it == data_->component_infos_by_name.end())
    return nullptr;

  auto ci =
      std::shared_ptr<ComponentInfo>(new ComponentInfo(parent_it->second->getSceneName(), name, parent_it->second));
  data_->component_infos_by_ns[ci->getNamespace()] = ci;
  data_->component_infos_by_name[name] = ci;
  return ci;
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createChildHelper(const boost::uuids::uuid& parent_ns)
{
  std::string name = getUniqueName();
  return createChildHelper(parent_ns, name);
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::createChildHelper(const boost::uuids::uuid& parent_ns,
                                                                       const std::string& name)
{
  auto it = data_->component_infos_by_name.find(name);
  if (it != data_->component_infos_by_name.end())
    return nullptr;

  auto parent_it = data_->component_infos_by_ns.find(parent_ns);
  if (parent_it == data_->component_infos_by_ns.end())
    return nullptr;

  auto ci =
      std::shared_ptr<ComponentInfo>(new ComponentInfo(parent_it->second->getSceneName(), name, parent_it->second));
  data_->component_infos_by_ns[ci->getNamespace()] = ci;
  data_->component_infos_by_name[name] = ci;
  return ci;
}

std::string ComponentInfoManager::getUniqueName()
{
  std::string name = "Component" + std::to_string(data_->cnt++);
  auto it = data_->component_infos_by_name.find(name);
  while (it != data_->component_infos_by_name.end())
  {
    name = "Component" + std::to_string(data_->cnt++);
    it = data_->component_infos_by_name.find(name);
  }

  return name;
}

void ComponentInfoManager::removeUnusedHelper() {}

std::shared_ptr<ComponentInfo> ComponentInfoManager::getHelper(const boost::uuids::uuid& ns) const
{
  auto it = data_->component_infos_by_ns.find(ns);
  if (it == data_->component_infos_by_ns.end())
    return nullptr;

  return it->second;
}

std::shared_ptr<ComponentInfo> ComponentInfoManager::getHelper(const std::string& name) const
{
  auto it = data_->component_infos_by_name.find(name);
  if (it == data_->component_infos_by_name.end())
    return nullptr;

  return it->second;
}

}  // namespace tesseract_gui
