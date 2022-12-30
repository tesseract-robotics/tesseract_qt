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

#include <map>
#include <unordered_map>
#include <tesseract_qt/common/model_manager.h>
#include <tesseract_qt/common/component_info.h>

namespace tesseract_gui
{
ModelManager::ModelManager() {}
ModelManager::~ModelManager() = default;

std::shared_ptr<ModelManager> ModelManager::instance()
{
  static std::shared_ptr<ModelManager> singleton = nullptr;
  if (singleton == nullptr)
    singleton = std::make_shared<ModelManager>();

  return singleton;
}

}  // namespace tesseract_gui
