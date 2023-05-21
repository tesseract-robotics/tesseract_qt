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
#ifndef TESSERACT_GUI_COMMON_COMPONENT_INFO_H
#define TESSERACT_GUI_COMMON_COMPONENT_INFO_H

#include <string>
#include <list>
#include <functional>
#include <memory>
#include <boost/serialization/access.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_hash.hpp>

namespace tesseract_gui
{
struct ComponentInfo;

/** @brief All modesl will have this object to allow associating disconnected object together. */
struct ComponentInfo
{
  /** @brief Get the unique name */
  const std::string& getName() const;

  /** @brief Get the scene name */
  const std::string& getSceneName() const;

  /**
   * @brief Get this component infos namespace
   * @return The namespace
   */
  const boost::uuids::uuid& getNamespace() const;

  /**
   * @brief Get the component infos lineage
   * @return The lineage
   */
  std::list<boost::uuids::uuid> getLineage() const;

  /**
   * @brief A description
   * @details This is not used for comparision
   */
  const std::string& getDescription() const;
  void setDescription(const std::string& description);

  /** @brief Check if it has a parent component */
  bool hasParent() const;

  /** @brief Create parent component info using parent info */
  std::shared_ptr<const ComponentInfo> getParentComponentInfo() const;

  /** @brief Create child component info object */
  std::shared_ptr<ComponentInfo> createChild() const;

  /**
   * @brief Create child component info object
   * @param name The name assigned to the child; if name is not unique then nullptr is returned
   */
  std::shared_ptr<ComponentInfo> createChild(const std::string& name) const;

  /**
   * @brief Check if the provided ComponentInfo is the objects parent
   * @details It check if this component info ever derived from the provided info
   * @param other The object to check if parent
   * @return True if the scene_name is equal and this object is derived from the provided object, otherwise false
   */
  bool isParent(const std::shared_ptr<const ComponentInfo>& other) const;

  /**
   * @brief Check if the provided ComponentInfo is a child of this object
   * @details It check if the provided component info ever derived from this object
   * @param other The object to check if child
   * @return True if the scene_name is equal and the provided object derived from this object, otherwise false
   */
  bool isChild(const std::shared_ptr<const ComponentInfo>& other) const;

  bool operator==(const ComponentInfo& rhs) const;
  bool operator!=(const ComponentInfo& rhs) const;

private:
  friend class boost::serialization::access;
  friend class ComponentInfoManager;

  template <class Archive>
  void serialize(Archive& ar, const unsigned int version);  // NOLINT

  /** @brief The associated render scene */
  std::string scene_name_{ "tesseract_default" };

  /** @brief A unique name associated with the namespace */
  std::string name_;

  /** @brief The namespace */
  boost::uuids::uuid ns_{};

  /** @brief The parent if a child otherwise nullptr */
  std::shared_ptr<const ComponentInfo> parent_{ nullptr };

  /**
   * @brief A description
   * @details This is not used for comparision
   */
  std::string description_;

  /** @breif Default Construction */
  ComponentInfo();

  /** @brief Assigns provided scene name and generates random namespace */
  explicit ComponentInfo(std::string scene_name, std::string name, std::string description = "");

  /** @brief Constructor used when creating a child */
  explicit ComponentInfo(std::string scene_name,
                         std::string name,
                         std::shared_ptr<const ComponentInfo> parent,
                         std::string description = "");

  /** @brief Assigns provided scene name and use namespace string generates namespace uuid */
  explicit ComponentInfo(std::string scene_name,
                         std::string name,
                         const boost::uuids::uuid& ns,
                         std::shared_ptr<const ComponentInfo> parent = nullptr,
                         std::string description = "");
};

}  // namespace tesseract_gui

namespace std
{
template <>
struct hash<tesseract_gui::ComponentInfo>
{
  auto operator()(const tesseract_gui::ComponentInfo& obj) const -> size_t
  {
    return hash<std::string>{}(obj.getSceneName()) ^ hash<boost::uuids::uuid>{}(obj.getNamespace());
  }
};

}  // namespace std
#endif  // TESSERACT_GUI_COMMON_COMPONENT_INFO_H
