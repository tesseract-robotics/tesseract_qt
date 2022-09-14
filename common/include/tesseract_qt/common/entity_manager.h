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

#ifndef TESSERACT_GUI_COMMON_ENTITY_MANAGER_H
#define TESSERACT_GUI_COMMON_ENTITY_MANAGER_H

#include <tesseract_qt/common/entity.h>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace tesseract_gui
{
class EntityContainer;

class EntityManager : public std::enable_shared_from_this<EntityManager>
{
public:
  using Ptr = std::shared_ptr<EntityManager>;
  using ConstPtr = std::shared_ptr<const EntityManager>;

  EntityManager() = default;
  virtual ~EntityManager() = default;
  /**
   * @brief Get entity container. If it does not exist one is created.
   * @param name The name of the container
   * @return The entity container
   */
  std::shared_ptr<EntityContainer> getEntityContainer(const std::string& name);
  std::shared_ptr<const EntityContainer> getEntityContainer(const std::string& name) const;
  bool hasEntityContainer(const std::string& name) const;
  void removeEntityContainer(const std::string& name);

  /**
   * @brief Get entity containers. If it does not exist one is created.
   * @param name The name of the container
   * @return The entity container
   */
  std::unordered_map<std::string, std::shared_ptr<EntityContainer>> getEntityContainers();
  std::unordered_map<std::string, std::shared_ptr<const EntityContainer>> getEntityContainers() const;

  /**
   * @brief Create new entity ID
   * @return A new entity ID
   */
  Entity createEntity();

  /** @brief Check if empty */
  bool empty() const;

  /** @brief Clear internal data */
  void clear();

private:
  mutable std::shared_mutex mutex_;
  std::unordered_map<std::string, std::shared_ptr<EntityContainer>> containers_;
  std::unordered_map<std::string, std::shared_ptr<const EntityContainer>> containers_const_;
  static int entity_counter_; /**< Start entity counter to avoid clashing with gazebo */
};
}  // namespace tesseract_gui
#endif  // TESSERACT_GUI_COMMON_ENTITY_MANAGER_H
