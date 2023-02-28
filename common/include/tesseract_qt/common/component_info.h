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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_hash.hpp>

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

  /** @brief The associated render scene */
  std::string scene_name{ "tesseract_default" };

  /** @brief The associated namespace */
  boost::uuids::uuid ns{};

  /**
   * @brief A description
   * @details This is not used for comparision
   */
  std::string description;

  /** @brief Check if it has a parent component */
  bool hasParent() const;

  /** @brief Get the parent hash */
  std::shared_ptr<const ComponentInfo> getParent() const;

  /** @brief Create child component info object */
  ComponentInfo createChild() const;

  /**
   * @brief Check if the provided ComponentInfo is the objects parent
   * @param other The object to check if parent
   * @return True if the scene_name is equal and the provided objects ns equals this objects parent_ns, otherwise false
   */
  bool isParent(const ComponentInfo& other) const;

  /**
   * @brief Check if the provided ComponentInfo a child of this object
   * @param other The object to check if child
   * @return True if the scene_name is equal and the provided objects parent_ns equals this objects ns, otherwise false
   */
  bool isChild(const ComponentInfo& other) const;

  bool operator==(const ComponentInfo& rhs) const;
  bool operator!=(const ComponentInfo& rhs) const;

private:
  /** @brief The associated parent ns */
  std::shared_ptr<const ComponentInfo> parent_;
};

}  // namespace tesseract_gui

namespace std
{
template <>
struct hash<tesseract_gui::ComponentInfo>
{
  auto operator()(const tesseract_gui::ComponentInfo& obj) const -> size_t
  {
    return (hash<std::string>{}(obj.scene_name) ^ hash<boost::uuids::uuid>{}(obj.ns) ^
            hash<std::shared_ptr<const tesseract_gui::ComponentInfo>>{}(obj.getParent()));
  }
};

}  // namespace std
#endif  // TESSERACT_GUI_COMMON_COMPONENT_INFO_H
