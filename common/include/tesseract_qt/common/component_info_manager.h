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
#ifndef TESSERACT_GUI_COMMON_COMPONENT_INFO_MANAGER_H
#define TESSERACT_GUI_COMMON_COMPONENT_INFO_MANAGER_H

#include <memory>
#include <vector>
#include <mutex>
#include <boost/uuid/uuid.hpp>
#include <yaml-cpp/yaml.h>

namespace tesseract_gui
{
struct ComponentInfo;

/** @brief This managers all component infos which can be accessed by anything within the application */
class ComponentInfoManager
{
public:
  ComponentInfoManager();
  ~ComponentInfoManager();
  ComponentInfoManager(const ComponentInfoManager&) = delete;
  ComponentInfoManager& operator=(const ComponentInfoManager&) = delete;
  ComponentInfoManager(ComponentInfoManager&&) = delete;
  ComponentInfoManager& operator=(ComponentInfoManager&&) = delete;

  /**
   * @brief Construct manager from yaml config
   * @details This will clear the manager then load the config
   * @param config The yaml config
   */
  static void loadConfig(const YAML::Node& config);

  /**
   * @brief Generate a config based on the contents of the manager
   * @return A yaml node representing the contents of the manager
   */
  static YAML::Node getConfig();

  /**
   * @brief Create a unique component info
   * @return A unique component info
   */
  static std::shared_ptr<ComponentInfo> create(const std::string& scene_name);

  /**
   * @brief Create a unique component info with the provided name
   * @return A unique component info, if name provided is not unique then a nullptr is returned
   */
  static std::shared_ptr<ComponentInfo> create(const std::string& scene_name, const std::string& name);

  /**
   * @brief Create a child component info
   * @return A child component info
   */
  static std::shared_ptr<ComponentInfo> createChild(std::shared_ptr<const ComponentInfo> parent);

  /**
   * @brief Create a child component info with the provided name
   * @return A child component info, if name provided is not unique then a nullptr is returned
   */
  static std::shared_ptr<ComponentInfo> createChild(std::shared_ptr<const ComponentInfo> parent,
                                                    const std::string& name);

  /**
   * @brief Create a child component info
   * @return A child component info
   */
  static std::shared_ptr<ComponentInfo> createChild(const std::string& parent_name);

  /**
   * @brief Create a child component info with the provided name
   * @return A child component info, if name provided is not unique then a nullptr is returned
   */
  static std::shared_ptr<ComponentInfo> createChild(const std::string& parent_name, const std::string& name);

  /**
   * @brief Create a child component info
   * @return A child component info
   */
  static std::shared_ptr<ComponentInfo> createChild(const boost::uuids::uuid& parent_ns);

  /**
   * @brief Create a child component info with the provided name
   * @return A child component info, if name provided is not unique then a nullptr is returned
   */
  static std::shared_ptr<ComponentInfo> createChild(const boost::uuids::uuid& parent_ns, const std::string& name);

  /**
   * @brief Get a component info by uuuid
   * @param ns The namespace
   * @return The component info associated with the provided uuid, if not found nullptr is returned
   */
  static std::shared_ptr<ComponentInfo> get(const boost::uuids::uuid& ns);

  /**
   * @brief Get a component info by name
   * @param name The name associated with the component info
   * @return The component info associated with the provided name, if not found nullptr is returned
   */
  static std::shared_ptr<ComponentInfo> get(const std::string& name);

  /**
   * @brief Get all component infos
   * @return A vector of all component infos
   */
  static std::vector<std::shared_ptr<ComponentInfo>> get();

  /**
   * @brief Remove unused component infos
   * @details This leverages the reference counter to determine if anything is using a component info.
   */
  static void removeUnused();

  /** @brief Check if empty */
  static bool empty();

  /**
   * @brief Clear all environments
   * @warning This should rarely be used, recommend using removeUnused
   */
  static void clear();

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;

  static std::shared_ptr<ComponentInfoManager> singleton;
  static std::once_flag init_instance_flag;
  static void initSingleton();
  static std::shared_ptr<ComponentInfoManager> instance();

  void loadConfigHelper(const YAML::Node& config);
  YAML::Node getConfigHelper() const;

  std::shared_ptr<ComponentInfo> createHelper(const std::string& scene_name);
  std::shared_ptr<ComponentInfo> createHelper(const std::string& scene_name, const std::string& name);
  std::shared_ptr<ComponentInfo> createChildHelper(std::shared_ptr<const ComponentInfo> parent);
  std::shared_ptr<ComponentInfo> createChildHelper(std::shared_ptr<const ComponentInfo> parent,
                                                   const std::string& name);
  std::shared_ptr<ComponentInfo> createChildHelper(const std::string& parent_name);
  std::shared_ptr<ComponentInfo> createChildHelper(const std::string& parent_name, const std::string& name);
  std::shared_ptr<ComponentInfo> createChildHelper(const boost::uuids::uuid& parent_ns);
  std::shared_ptr<ComponentInfo> createChildHelper(const boost::uuids::uuid& parent_ns, const std::string& name);

  std::string getUniqueName();
  void removeUnusedHelper();

  std::shared_ptr<ComponentInfo> getHelper(const boost::uuids::uuid& ns) const;
  std::shared_ptr<ComponentInfo> getHelper(const std::string& name) const;
  std::vector<std::shared_ptr<ComponentInfo>> getHelper() const;

  bool emptyHelper() const;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_GUI_COMMON_COMPONENT_INFO_MANAGER_H
