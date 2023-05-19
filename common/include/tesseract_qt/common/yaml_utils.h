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
#ifndef TESSERACT_QT_COMMON_YAML_UTILS_H
#define TESSERACT_QT_COMMON_YAML_UTILS

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <yaml-cpp/yaml.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/common/plugin_infos.h>
#include <tesseract_qt/common/component_info.h>

namespace YAML
{
template <>
struct convert<tesseract_gui::ComponentInfo>
{
  static Node encode(const tesseract_gui::ComponentInfo& rhs)
  {
    YAML::Node component_info;
    component_info["scene_name"] = rhs.getSceneName();
    component_info["description"] = rhs.getDescription();
    component_info["ns"] = boost::uuids::to_string(rhs.getNamespace());
    if (rhs.hasParent())
    {
      for (const auto& n : rhs.getLineage())
        component_info["lineage"]["ns"].push_back(boost::uuids::to_string(n));
    }

    return component_info;
  }

  static bool decode(const Node& node, tesseract_gui::ComponentInfo& rhs)
  {
    std::string scene_name{ "tesseract_default" };
    if (node["scene_name"])
      scene_name = node["scene_name"].as<std::string>();

    std::string description;
    if (node["description"])
      description = node["description"].as<std::string>();

    std::list<boost::uuids::uuid> ns;
    if (node["ns"])
      ns.push_back(boost::lexical_cast<boost::uuids::uuid>(node["ns"].as<std::string>()));

    if (node["lineage"])
    {
      auto lineage = node["lineage"].as<std::list<std::string>>();
      for (const auto& n : lineage)
        ns.push_back(boost::lexical_cast<boost::uuids::uuid>(n));
    }

    rhs = tesseract_gui::ComponentInfo{ scene_name, ns, description };
    return true;
  }
};

template <>
struct convert<tesseract_gui::StudioPluginInfo>
{
  static Node encode(const tesseract_gui::StudioPluginInfo& rhs)
  {
    const std::string SEARCH_PATHS_KEY{ "search_paths" };
    const std::string SEARCH_LIBRARIES_KEY{ "search_libraries" };
    const std::string COMPONENT_INFOS_KEY{ "component_infos" };
    const std::string PLUGINS_KEY{ "components" };

    YAML::Node studio_plugins;
    if (!rhs.search_paths.empty())
      studio_plugins[SEARCH_PATHS_KEY] = rhs.search_paths;

    if (!rhs.search_libraries.empty())
      studio_plugins[SEARCH_LIBRARIES_KEY] = rhs.search_libraries;

    //    if (!rhs.component_infos.empty())
    //      studio_plugins[COMPONENT_INFOS_KEY] = rhs.component_infos;

    if (!rhs.plugin_infos.plugins.empty())
      studio_plugins[PLUGINS_KEY] = rhs.plugin_infos;

    return studio_plugins;
  }

  static bool decode(const Node& node, tesseract_gui::StudioPluginInfo& rhs)
  {
    const std::string SEARCH_PATHS_KEY{ "search_paths" };
    const std::string SEARCH_LIBRARIES_KEY{ "search_libraries" };
    const std::string COMPONENT_INFOS_KEY{ "component_infos" };
    const std::string PLUGINS_KEY{ "components" };

    if (const YAML::Node& search_paths = node[SEARCH_PATHS_KEY])
    {
      std::set<std::string> sp;
      try
      {
        sp = search_paths.as<std::set<std::string>>();
      }
      catch (const std::exception& e)
      {
        throw std::runtime_error("StudioPluginInfo: Constructor failed to cast '" + SEARCH_PATHS_KEY +
                                 "' to std::set<std::string>! "
                                 "Details: " +
                                 e.what());
      }
      rhs.search_paths.insert(sp.begin(), sp.end());
    }

    if (const YAML::Node& search_libraries = node[SEARCH_LIBRARIES_KEY])
    {
      std::set<std::string> sl;
      try
      {
        sl = search_libraries.as<std::set<std::string>>();
      }
      catch (const std::exception& e)
      {
        throw std::runtime_error("StudioPluginInfo: Constructor failed to cast '" + SEARCH_LIBRARIES_KEY +
                                 "' to std::set<std::string>! "
                                 "Details: " +
                                 e.what());
      }
      rhs.search_libraries.insert(sl.begin(), sl.end());
    }

    if (const YAML::Node& component_infos = node[COMPONENT_INFOS_KEY])
    {
      if (!component_infos.IsMap())
        throw std::runtime_error(COMPONENT_INFOS_KEY + ", should contain a map of component info names to component "
                                                       "info!");

      try
      {
        //        rhs.component_infos = component_infos.as<std::map<std::string, tesseract_gui::ComponentInfo>>();
      }
      catch (const std::exception& e)
      {
        throw std::runtime_error(
            "StudioPluginInfo: Constructor failed to cast '" + COMPONENT_INFOS_KEY +
            "' to std::unordered_map<std::string, tesseract_gui::ComponentInfo>! Details: " + e.what());
      }
    }

    if (const YAML::Node& plugins = node[PLUGINS_KEY])
    {
      if (!plugins.IsMap())
        throw std::runtime_error(PLUGINS_KEY + ", should contain a map of studio plugin names to "
                                               "plugins!");

      try
      {
        rhs.plugin_infos = plugins.as<tesseract_common::PluginInfoContainer>();
      }
      catch (const std::exception& e)
      {
        throw std::runtime_error("StudioPluginInfo: Constructor failed to cast '" + PLUGINS_KEY +
                                 "' to tesseract_common::PluginInfoContainer! Details: " + e.what());
      }
    }

    return true;
  }
};
}  // namespace YAML
#endif  // TESSERACT_QT_COMMON_YAML_UTILS_H
