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
#ifndef TESSERACT_QT_COMMON_PLUGIN_INFOS_H
#define TESSERACT_QT_COMMON_PLUGIN_INFOS_H

#include <set>
#include <string>
#include <tesseract_common/types.h>
#include <tesseract_qt/common/component_info.h>

namespace tesseract_gui
{
/** @brief The studio plugin information structure */
struct StudioPluginInfo
{
  /** @brief A list of paths to search for plugins */
  std::set<std::string> search_paths;

  /** @brief A list of library names without the prefix or suffix that contain plugins*/
  std::set<std::string> search_libraries;

  /** @brief A map of name to component infos */
  std::map<std::string, ComponentInfo> component_infos;

  /** @brief A map of name to studio plugin information */
  tesseract_common::PluginInfoContainer plugin_infos;

  /** @brief Insert the content of an other StudioPluginInfo */
  void insert(const StudioPluginInfo& other);

  /** @brief Clear the contents */
  void clear();

  /** @brief Check if structure is empty */
  bool empty() const;

  // Yaml Config key
  static inline const std::string CONFIG_KEY{ "studio_plugins" };

  bool operator==(const StudioPluginInfo& rhs) const;
  bool operator!=(const StudioPluginInfo& rhs) const;

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version);  // NOLINT
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_COMMON_PLUGIN_INFOS_H
