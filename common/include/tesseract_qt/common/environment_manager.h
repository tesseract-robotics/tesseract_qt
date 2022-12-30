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

namespace tesseract_gui
{
struct ComponentInfo;
class EnvironmentWrapper;
class EnvironmentManager
{
public:
  EnvironmentManager();
  ~EnvironmentManager();
  EnvironmentManager(const EnvironmentManager&) = delete;
  EnvironmentManager& operator=(const EnvironmentManager&) = delete;
  EnvironmentManager(EnvironmentManager&&) = delete;
  EnvironmentManager& operator=(EnvironmentManager&&) = delete;

  static std::shared_ptr<const EnvironmentWrapper> get(const ComponentInfo& component_info);
  static void set(std::shared_ptr<const EnvironmentWrapper> env);

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;

  static std::shared_ptr<EnvironmentManager> instance();
  void setHelper(std::shared_ptr<const EnvironmentWrapper> env);
  std::shared_ptr<const EnvironmentWrapper> getHelper(const ComponentInfo& component_info) const;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_GUI_COMMON_ENVIRONMENT_MANAGER_H
