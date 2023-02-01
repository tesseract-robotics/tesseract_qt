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
#include <tesseract_qt/common/events/manipulation_events.h>

namespace tesseract_gui::events
{
class ManipulationChanged::Implementation
{
public:
  std::string state_name;
  std::unordered_map<std::string, tesseract_gui::ComponentInfo> state_component_infos;
};

ManipulationChanged::ManipulationChanged(
    ComponentInfo component_info,
    std::string state_name,
    std::unordered_map<std::string, tesseract_gui::ComponentInfo> state_component_infos)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->state_name = std::move(state_name);
  data_->state_component_infos = std::move(state_component_infos);
}
ManipulationChanged::ManipulationChanged(const ManipulationChanged& other)
  : ManipulationChanged(other.getComponentInfo(), other.getStateName(), other.getStateComponentInfos())
{
}

ManipulationChanged::~ManipulationChanged() = default;

const std::string& ManipulationChanged::getStateName() const { return data_->state_name; }
const std::unordered_map<std::string, tesseract_gui::ComponentInfo>& ManipulationChanged::getStateComponentInfos() const
{
  return data_->state_component_infos;
}

}  // namespace tesseract_gui::events
