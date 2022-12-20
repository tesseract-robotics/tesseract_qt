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
#include <boost/uuid/uuid.hpp>

namespace tesseract_gui
{
/** @brief All modesl will have this object to allow associating disconnected object together. */
struct ComponentInfo
{
  /** @breif Default Construction */
  ComponentInfo() = default;

  /** @brief Assigns provided scene name and generates random namespace */
  explicit ComponentInfo(std::string scene_name);

  /** @brief The associated render scene */
  std::string scene_name{ "tesseract_default" };

  /** @brief The associated namespace */
  boost::uuids::uuid ns{};

  bool operator==(const ComponentInfo& other) const;
  bool operator!=(const ComponentInfo& rhs) const;
};

}  // namespace tesseract_gui
#endif  // TESSERACT_GUI_COMMON_COMPONENT_INFO_H
