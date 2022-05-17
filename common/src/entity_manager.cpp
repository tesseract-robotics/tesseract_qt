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
#include <tesseract_qt/common/entity_manager.h>
#include <tesseract_qt/common/entity_container.h>

namespace tesseract_gui
{
int EntityManager::entity_counter_ = 1000;  // NOLINT

std::shared_ptr<EntityContainer> EntityManager::getEntityContainer(const std::string& name)
{
  std::unique_lock<std::shared_mutex> lock(mutex_);
  auto c_it = containers_.find(name);
  if (c_it == containers_.end())
  {
    auto container = std::make_shared<EntityContainer>(shared_from_this(), name);
    containers_[name] = container;
    containers_const_[name] = container;
    return container;
  }

  return c_it->second;
}

std::shared_ptr<const EntityContainer> EntityManager::getEntityContainer(const std::string& name) const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto c_it = containers_.find(name);
  if (c_it == containers_.end())
    return nullptr;

  return c_it->second;
}

bool EntityManager::hasEntityContainer(const std::string& name) const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  return (containers_.find(name) != containers_.end());
}

void EntityManager::removeEntityContainer(const std::string& name)
{
  std::unique_lock<std::shared_mutex> lock(mutex_);
  containers_.erase(name);
  containers_const_.erase(name);
}

std::unordered_map<std::string, std::shared_ptr<EntityContainer>> EntityManager::getEntityContainers()
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  return containers_;
}

std::unordered_map<std::string, std::shared_ptr<const EntityContainer>> EntityManager::getEntityContainers() const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  return containers_const_;
}

Entity EntityManager::createEntity()
{
  std::unique_lock<std::shared_mutex> lock(mutex_);
  Entity entity;
  entity.id = ++entity_counter_;
  entity.unique_name = "TesseractEntity::" + std::to_string(entity.id);
  return entity;
}

bool EntityManager::empty() const
{
  std::shared_lock<std::shared_mutex> lock(mutex_);
  for (const auto& c : containers_)
  {
    if (!c.second->empty())
      return false;
  }

  return true;
}

void EntityManager::clear()
{
  std::unique_lock<std::shared_mutex> lock(mutex_);
  for (auto& c : containers_)
    c.second->clear();

  containers_.clear();
}
}  // namespace tesseract_gui
