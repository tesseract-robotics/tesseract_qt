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
#include <tesseract_qt/common/entity_container.h>
#include <tesseract_qt/common/entity_manager.h>

namespace tesseract_gui
{
const std::string EntityContainer::VISUAL_NS = "visual";
const std::string EntityContainer::SENSOR_NS = "sensor";
const std::string EntityContainer::RESOURCE_NS = "resource";

EntityContainer::EntityContainer(std::shared_ptr<EntityManager> manager, std::string name)
  : manager_(std::move(manager)), name_(std::move(name))
{
}

std::string EntityContainer::getName() const { return name_; }

Entity EntityContainer::addTrackedEntity(const std::string& ns, const std::string& name)
{
  std::unique_lock<std::shared_mutex> lock(mutex_);
  Entity entity = manager_->createEntity();
  tracked_entity_map_[ns][name] = entity;
  return entity;
}

Entity EntityContainer::getTrackedEntity(const std::string& ns, const std::string& name) const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto ns_it = tracked_entity_map_.find(ns);
  if (ns_it == tracked_entity_map_.end())
    throw std::runtime_error("Tracked entity namespace does not exist for name '" + ns + "'.");

  auto it = ns_it->second.find(name);
  if (it == ns_it->second.end())
    throw std::runtime_error("Tracked entity '" + name + "' does not exist under namespace '" + ns + "'.");

  return it->second;
}

bool EntityContainer::hasTrackedEntity(const std::string& ns, const std::string& name) const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto ns_it = tracked_entity_map_.find(ns);
  if (ns_it == tracked_entity_map_.end())
    return false;

  return (ns_it->second.find(name) != ns_it->second.end());
}

EntityMap EntityContainer::getTrackedEntities(const std::string& ns) const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto ns_it = tracked_entity_map_.find(ns);
  if (ns_it == tracked_entity_map_.end())
    return {};

  return ns_it->second;
}

std::unordered_map<std::string, EntityMap> EntityContainer::getTrackedEntities() const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  return tracked_entity_map_;
}

Entity EntityContainer::addUntrackedEntity(const std::string& ns)
{
  std::unique_lock<std::shared_mutex> lock(mutex_);
  Entity entity = manager_->createEntity();
  untracked_entity_map_[ns].push_back(entity);
  return entity;
}

EntityVector EntityContainer::getUntrackedEntities(const std::string& ns) const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto ns_it = untracked_entity_map_.find(ns);
  if (ns_it == untracked_entity_map_.end())
    return {};

  return ns_it->second;
}

std::unordered_map<std::string, EntityVector> EntityContainer::getUntrackedEntities() const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  return untracked_entity_map_;
}

void EntityContainer::addTrackedUnmanagedObject(const std::string& ns, const std::string& name, UnmanagedObject object)
{
  std::unique_lock<std::shared_mutex> lock(mutex_);
  tracked_unmanaged_objects_map_[ns][name] = object;
}

UnmanagedObject EntityContainer::getTrackedUnmanagedObject(const std::string& ns, const std::string& name)
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto ns_it = tracked_unmanaged_objects_map_.find(ns);
  if (ns_it == tracked_unmanaged_objects_map_.end())
    throw std::runtime_error("Tracked entity namespace does not exist for name '" + ns + "'.");

  auto it = ns_it->second.find(name);
  if (it == ns_it->second.end())
    throw std::runtime_error("Tracked entity '" + name + "' does not exist under namespace '" + ns + "'.");

  return it->second;
}

bool EntityContainer::hasTrackedUnmanagedObject(const std::string& ns, const std::string& name)
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto ns_it = tracked_unmanaged_objects_map_.find(ns);
  if (ns_it == tracked_unmanaged_objects_map_.end())
    return false;

  return (ns_it->second.find(name) != ns_it->second.end());
}

UnmanagedObjectMap EntityContainer::getTrackedUnmanagedObjects(const std::string& ns) const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto ns_it = tracked_unmanaged_objects_map_.find(ns);
  if (ns_it == tracked_unmanaged_objects_map_.end())
    return {};

  return ns_it->second;
}

std::unordered_map<std::string, UnmanagedObjectMap> EntityContainer::getTrackedUnmanagedObjects() const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  return tracked_unmanaged_objects_map_;
}

void EntityContainer::addUntrackedUnmanagedObject(const std::string& ns, UnmanagedObject object)
{
  std::unique_lock<std::shared_mutex> lock(mutex_);
  untracked_unmanaged_objects_map_[ns].push_back(object);
}

UnmanagedObjectVector EntityContainer::getUntrackedUnmanagedObjects(const std::string& ns) const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto ns_it = untracked_unmanaged_objects_map_.find(ns);
  if (ns_it == untracked_unmanaged_objects_map_.end())
    return {};

  return ns_it->second;
}

std::unordered_map<std::string, UnmanagedObjectVector> EntityContainer::getUntrackedUnmanagedObjects() const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  return untracked_unmanaged_objects_map_;
}

bool EntityContainer::empty() const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  if (tracked_entity_map_.empty() && untracked_entity_map_.empty())
    return true;

  for (const auto& ns : tracked_entity_map_)
  {
    if (!ns.second.empty())
      return false;
  }

  for (const auto& ns : untracked_entity_map_)
  {
    if (!ns.second.empty())
      return false;
  }

  for (const auto& ns : tracked_unmanaged_objects_map_)
  {
    if (!ns.second.empty())
      return false;
  }

  for (const auto& ns : untracked_unmanaged_objects_map_)
  {
    if (!ns.second.empty())
      return false;
  }

  return true;
}

void EntityContainer::clear()
{
  std::unique_lock<std::shared_mutex> lock(mutex_);
  tracked_entity_map_.clear();
  untracked_entity_map_.clear();
  tracked_unmanaged_objects_map_.clear();
  untracked_unmanaged_objects_map_.clear();
}

EntityManager& EntityContainer::getEntityManager() { return *manager_; }
const EntityManager& EntityContainer::getEntityManager() const { return *manager_; }

}  // namespace tesseract_gui
