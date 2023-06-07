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
#ifndef TESSERACT_GUI_COMMON_ENVIRONMENT_MANAGER_H
#define TESSERACT_GUI_COMMON_ENVIRONMENT_MANAGER_H

#include <memory>
#include <unordered_map>
#include <mutex>

namespace tesseract_gui
{
struct ComponentInfo;
class EnvironmentWrapper;

/**
 * @brief This managers all environments which can be accessed by anything within the application
 * @details The first environment added is automatically set as the default
 */
class EnvironmentManager
{
public:
  EnvironmentManager();
  ~EnvironmentManager();
  EnvironmentManager(const EnvironmentManager&) = delete;
  EnvironmentManager& operator=(const EnvironmentManager&) = delete;
  EnvironmentManager(EnvironmentManager&&) = delete;
  EnvironmentManager& operator=(EnvironmentManager&&) = delete;

  /**
   * @brief Store the environment wrapper
   * @param env The environment wrapper to store
   * @param set_default Indicate if this should be set as the default. The first environment wrapper added is
   * automatically assigned as the default
   */
  static void set(std::shared_ptr<EnvironmentWrapper> env, bool set_default = false);

  /**
   * @brief Get the environment wrapper associated with the provided component info
   * @param component_info The component info used to retrieve environment wrapper
   * @return The environment wrapper, if not found a nullptr is retuned
   */
  static std::shared_ptr<EnvironmentWrapper> get(const std::shared_ptr<const ComponentInfo>& component_info);

  /**
   * @brief Get all environment wrappers
   * @return A map of all environment wrappers
   */
  static std::map<std::shared_ptr<const ComponentInfo>, std::shared_ptr<EnvironmentWrapper> > getAll();

  /**
   * @brief This will search for the first environment
   * @details It starts with the provided component info then searches the parents until it finds an available
   * environment wrapper.
   * @param component_info The component info used begin the search
   * @return The first available environment wrapper, if not found a nullptr is retuned
   */
  static std::shared_ptr<EnvironmentWrapper> find(const std::shared_ptr<const ComponentInfo>& component_info);

  /**
   * @brief Get the default environment wrapper.
   * @return The default environment wrapper
   */
  static std::shared_ptr<EnvironmentWrapper> getDefault();

  /**
   * @brief Set the default environment wrapper
   * @param component_info The component info associated with the environment wrapper to set as default
   */
  static void setDefault(std::shared_ptr<const ComponentInfo> component_info);

  /** @brief Remove environment */
  static void remove(const std::shared_ptr<const ComponentInfo>& component_info);

  /**
   * @brief Clear all environments
   * @warning This should rarely be used
   */
  static void clear();

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;

  static std::shared_ptr<EnvironmentManager> singleton;
  static std::once_flag init_instance_flag;
  static void initSingleton();
  static std::shared_ptr<EnvironmentManager> instance();

  void setHelper(std::shared_ptr<EnvironmentWrapper> env, bool set_default);
  std::shared_ptr<EnvironmentWrapper> getHelper(const std::shared_ptr<const ComponentInfo>& component_info) const;
  std::shared_ptr<EnvironmentWrapper> findHelper(const std::shared_ptr<const ComponentInfo>& component_info) const;
  void setDefaultHelper(const std::shared_ptr<const ComponentInfo>& component_info);
  std::shared_ptr<EnvironmentWrapper> getDefaultHelper() const;
  void removeHelper(const std::shared_ptr<const ComponentInfo>& component_info);
};
}  // namespace tesseract_gui

#endif  // TESSERACT_GUI_COMMON_ENVIRONMENT_MANAGER_H
