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

#ifndef TESSERACT_GUI_COMMON_ENTITY_CONTAINER_H
#define TESSERACT_GUI_COMMON_ENTITY_CONTAINER_H

#include <tesseract_qt/common/entity.h>
#include <memory>
#include <vector>
#include <shared_mutex>

namespace tesseract_gui
{
class EntityManager;

class EntityContainer
{
public:
  using Ptr = std::shared_ptr<EntityContainer>;
  using ConstPtr = std::shared_ptr<const EntityContainer>;

  static const std::string VISUAL_NS;
  static const std::string SENSOR_NS;
  static const std::string RESOURCE_NS;

  EntityContainer(std::shared_ptr<EntityManager> manager, std::string name);

  /**
   * @brief Get the name of the container
   * @brief This should be unique
   * @return The container name
   */
  std::string getName() const;

  /**
   * @brief Add a tracked entity
   * @param ns The namespace to store it under
   * @param name The tracked name
   * @return The entity
   */
  Entity addTrackedEntity(const std::string& ns, const std::string& name);

  /**
   * @brief Get a tracked entity
   * @throws If either the namespace or name do not exist
   * @param ns The namespace to search under
   * @param name The name to locate
   * @return The entity
   */
  Entity getTrackedEntity(const std::string& ns, const std::string& name);

  /**
   * @brief Check if a tracked entity exists
   * @param ns The namespace to search under
   * @param name The name to locate
   * @return True if it exists, otherwise false
   */
  bool hasTrackedEntity(const std::string& ns, const std::string& name);

  /**
   * @brief Get the tracked entities under the provided namespace
   * @param ns The namespace to return
   * @return Tracked entities for the provided namespace
   */
  EntityMap getTrackedEntities(const std::string& ns) const;

  /**
   * @brief Get all tracked entities
   * @return All tracked entities
   */
  std::unordered_map<std::string, EntityMap> getTrackedEntities() const;

  /**
   * @brief Add untracked entity under the provided namespace
   * @param ns The namespace to store it under
   * @return The entity
   */
  Entity addUntrackedEntity(const std::string& ns);

  /**
   * @brief Get the untracked entities under the provided namespace
   * @param ns The namespace to return
   * @return Untracked entities for the provided namespace
   */
  EntityVector getUntrackedEntities(const std::string& ns) const;

  /**
   * @brief Get all untracked entities
   * @return All untracked entities
   */
  std::unordered_map<std::string, EntityVector> getUntrackedEntities() const;

  /**
   * @brief Add unmanaged object under the provided namespace
   * @details This allows for storing object that must be manually deleted and not owned by the render engine
   * @param ns The namespace to store it under
   */
  void addUnmanagedObject(const std::string& ns, std::shared_ptr<const void> object);

  /**
   * @brief Get the unmanaged objects under the provided namespace
   * @param ns The namespace to return
   * @return Unmanaged objects for the provided namespace
   */
  std::vector<std::shared_ptr<const void>> getUnmanagedObjects(const std::string& ns) const;

  /**
   * @brief Get all unmanaged objects
   * @return All unmanaged objects
   */
  std::unordered_map<std::string, std::vector<std::shared_ptr<const void>>> getUnmanagedObjects() const;

  /** @brief Check if empty */
  bool empty() const;

  /** @brief Clear internal data */
  void clear();

  /** @brief Get the entity manager that the container is associated with */
  EntityManager& getEntityManager();
  const EntityManager& getEntityManager() const;

private:
  std::shared_ptr<EntityManager> manager_;
  std::string name_;
  std::unordered_map<std::string, EntityMap> tracked_entity_map_;
  std::unordered_map<std::string, EntityVector> untracked_entity_map_;
  std::unordered_map<std::string, std::vector<std::shared_ptr<const void>>> unmanaged_objects_map_;

  mutable std::shared_mutex mutex_;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_GUI_COMMON_ENTITY_CONTAINER_H
