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
#ifndef TESSERACT_GUI_COMMON_MODEL_MANAGER_H
#define TESSERACT_GUI_COMMON_MODEL_MANAGER_H

#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <tesseract_qt/common/component_info.h>

namespace tesseract_gui
{
// clang-format off
enum class ModelType : int
{
  SCENE_GRAPH = 1,
  SCENE_STATE = 2,
  KINEMATIC_GROUPS = 3,
  GROUP_STATES = 4,
  GROUP_TCPS = 5,
  ALLOWED_COLLISION_MATRIX = 6,
  LINK_NAMES = 7,
  JOINT_NAMES = 8,
  COMMAND_HISTORY = 9,
  CONTACT_RESUTLS = 10,
  COMPOSITE_INSTRUCTION = 11
};
// clang-format on

struct ComponentInfo;
/** @brief A centralized location to store data models */
class ModelManager
{
public:
  ModelManager();
  ~ModelManager();
  ModelManager(const ModelManager&) = delete;
  ModelManager& operator=(const ModelManager&) = delete;
  ModelManager(ModelManager&&) = delete;
  ModelManager& operator=(ModelManager&&) = delete;

  template <typename T>
  static std::shared_ptr<T> get(const ComponentInfo& component_info, ModelType type)
  {
    return instance()->getHelper<T>(component_info, type);
  }

private:
  std::unordered_map<ComponentInfo, std::map<ModelType, std::shared_ptr<void>>> data_;

  static std::shared_ptr<ModelManager> instance();

  template <typename T>
  std::shared_ptr<T> getHelper(const ComponentInfo& component_info, ModelType type)
  {
    auto it = data_.find(component_info);
    if (it == data_.end())
    {
      auto model = std::make_shared<T>(component_info);
      data_[component_info][type] = model;
      return model;
    }

    auto it2 = it->second.find(type);
    if (it2 == it->second.end())
    {
      auto model = std::make_shared<T>(component_info);
      data_[component_info][type] = model;
      return model;
    }

    return std::dynamic_pointer_cast<T>(it2->second);
  }
};
}  // namespace tesseract_gui

#endif  // TESSERACT_GUI_COMMON_MODEL_MANAGER_H
