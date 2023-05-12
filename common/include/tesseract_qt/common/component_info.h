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
#include <functional>
#include <memory>
#include <boost/serialization/access.hpp>

namespace tesseract_gui
{
struct ComponentInfo;

/** @brief All modesl will have this object to allow associating disconnected object together. */
struct ComponentInfo
{
  /** @breif Default Construction */
  ComponentInfo() = default;

  /** @brief Assigns provided scene name and generates random namespace */
  explicit ComponentInfo(std::string scene_name, std::string description = "");

  /** @brief Assigns provided scene name and use namespace string generates namespace uuid */
  explicit ComponentInfo(std::string scene_name, std::string ns, std::string description = "");

  /** @brief The associated render scene */
  std::string scene_name{ "tesseract_default" };

  /**
   * @brief The associated namespace
   * @details Depending on the constructor this is generated using boost uuid converted to string
   */
  std::string ns;

  /**
   * @brief A description
   * @details This is not used for comparision
   */
  std::string description;

  /**
   * @brief The associated parent ns
   * @details Every time a child is created the parent namespace is prepended separated by ::
   */
  std::pair<std::string, std::string> parent_info;

  /** @brief Check if it has a parent component */
  bool hasParent() const;

  /** @brief Create parent component info using parent info */
  ComponentInfo getParentComponentInfo() const;

  /** @brief Create child component info object */
  ComponentInfo createChild() const;

  /**
   * @brief Check if the provided ComponentInfo is the objects parent
   * @note A parent and child should have different namespaces
   * @param other The object to check if parent
   * @return True if the scene_name is equal and internally stored parent is the provided object, otherwise false
   */
  bool isParent(const ComponentInfo& other) const;

  /**
   * @brief Check if the provided ComponentInfo a child of this object
   * @note A parent and child should have different namespaces
   * @param other The object to check if child
   * @return True if the scene_name is equal and internally stored parent is the provided object, otherwise false
   */
  bool isChild(const ComponentInfo& other) const;

  bool operator==(const ComponentInfo& rhs) const;
  bool operator!=(const ComponentInfo& rhs) const;

  // These are helper functions
  static std::pair<std::string, std::string> createParentInfo(const ComponentInfo& component_info);

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version);  // NOLINT
};

}  // namespace tesseract_gui

namespace std
{
template <>
struct hash<tesseract_gui::ComponentInfo>
{
  auto operator()(const tesseract_gui::ComponentInfo& obj) const -> size_t
  {
    return (hash<std::string>{}(obj.scene_name) ^ hash<std::string>{}(obj.ns) ^
            hash<std::string>{}(obj.parent_info.first) ^ hash<std::string>{}(obj.parent_info.second));
  }
};

}  // namespace std
#endif  // TESSERACT_GUI_COMMON_COMPONENT_INFO_H
