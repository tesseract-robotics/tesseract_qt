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
#include <tesseract_qt/collision/contact_results_events.h>
#include <tesseract_scene_graph/scene_state.h>

namespace tesseract_gui::events
{
ContactResultsClear::ContactResultsClear(ComponentInfo component_info, const std::string& ns)
  : ComponentEvent(std::move(component_info), kType), ns_(ns)
{
}

ContactResultsClear::~ContactResultsClear() = default;

const std::string& ContactResultsClear::getNamespace() const { return ns_; }

//////////////////////////////////////////

ContactResultsSet::ContactResultsSet(ComponentInfo component_info,
                                     const std::variant<ContactResultVector, ContactResultMap>& contact_results,
                                     const std::string& ns)
  : ComponentEvent(std::move(component_info), kType), ns_(ns), contact_results_(contact_results)
{
}

ContactResultsSet::~ContactResultsSet() = default;

const std::string& ContactResultsSet::getNamespace() const { return ns_; }
const std::variant<ContactResultVector, ContactResultMap>& ContactResultsSet::getContactResults() const
{
  return contact_results_;
}

//////////////////////////////////////////

ContactResultsRemove::ContactResultsRemove(ComponentInfo component_info, boost::uuids::uuid uuid)
  : ComponentEventUUID(std::move(component_info), uuid, kType)
{
}
ContactResultsRemove::ContactResultsRemove(ComponentInfo component_info,
                                           boost::uuids::uuid uuid,
                                           boost::uuids::uuid child_uuid)
  : ComponentEventUUID(std::move(component_info), uuid, child_uuid, kType)
{
}
ContactResultsRemove::~ContactResultsRemove() = default;

//////////////////////////////////////////

ContactResultsVisbility::ContactResultsVisbility(ComponentInfo component_info, boost::uuids::uuid uuid, bool visible)
  : ComponentEventVisibility(std::move(component_info), uuid, visible, kType)
{
}
ContactResultsVisbility::ContactResultsVisbility(ComponentInfo component_info,
                                                 boost::uuids::uuid uuid,
                                                 boost::uuids::uuid child_uuid,
                                                 bool visible)
  : ComponentEventVisibility(std::move(component_info), uuid, child_uuid, visible, kType)
{
}
ContactResultsVisbility::~ContactResultsVisbility() = default;

//////////////////////////////////////////

ContactResultsVisbilityAll::ContactResultsVisbilityAll(ComponentInfo component_info, bool visible)
  : ComponentEventVisibilityAll(std::move(component_info), visible, kType)
{
}

ContactResultsVisbilityAll::~ContactResultsVisbilityAll() = default;

//////////////////////////////////////////

ContactResultsCompute::ContactResultsCompute(ComponentInfo component_info,
                                             const tesseract_collision::CollisionCheckConfig& config,
                                             const std::string& ns)
  : ComponentEvent(std::move(component_info), kType), ns_(ns), config_(config)
{
}

ContactResultsCompute::~ContactResultsCompute() = default;

const std::string& ContactResultsCompute::getNamespace() const { return ns_; }
const tesseract_collision::CollisionCheckConfig& ContactResultsCompute::getConfig() const { return config_; }

//////////////////////////////////////////

ContactResultsComputeState::ContactResultsComputeState(ComponentInfo component_info,
                                                       const tesseract_collision::CollisionCheckConfig& config,
                                                       const tesseract_scene_graph::SceneState& state,
                                                       const std::string& ns)
  : ComponentEvent(std::move(component_info), kType)
  , ns_(ns)
  , config_(config)
  , state_(std::make_unique<tesseract_scene_graph::SceneState>(state))
{
}

ContactResultsComputeState::~ContactResultsComputeState() = default;

const std::string& ContactResultsComputeState::getNamespace() const { return ns_; }
const tesseract_collision::CollisionCheckConfig& ContactResultsComputeState::getConfig() const { return config_; }
const tesseract_scene_graph::SceneState& ContactResultsComputeState::getState() const { return *state_; }

//////////////////////////////////////////

ContactResultsComputeNamedState::ContactResultsComputeNamedState(
    ComponentInfo component_info,
    const tesseract_collision::CollisionCheckConfig& config,
    const std::string& group_name,
    const std::string& state_name,
    const std::string& ns)
  : ComponentEvent(std::move(component_info), kType)
  , ns_(ns)
  , config_(config)
  , group_name_(group_name)
  , state_name_(state_name)
{
}

ContactResultsComputeNamedState::~ContactResultsComputeNamedState() = default;

const std::string& ContactResultsComputeNamedState::getNamespace() const { return ns_; }
const tesseract_collision::CollisionCheckConfig& ContactResultsComputeNamedState::getConfig() const { return config_; }
const std::string& ContactResultsComputeNamedState::getGroupName() const { return group_name_; }
const std::string& ContactResultsComputeNamedState::getStateName() const { return state_name_; }
}  // namespace tesseract_gui::events
