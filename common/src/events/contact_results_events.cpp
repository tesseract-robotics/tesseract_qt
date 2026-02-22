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
#include <tesseract_qt/common/events/contact_results_events.h>

#include <tesseract_scene_graph/scene_state.h>

namespace tesseract::gui::events
{
ContactResultsClear::ContactResultsClear(std::shared_ptr<const ComponentInfo> component_info, const std::string& ns)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::CONTACT_RESULTS_CLEAR)), ns_(ns)
{
}

ContactResultsClear::~ContactResultsClear() = default;

const std::string& ContactResultsClear::getNamespace() const { return ns_; }

//////////////////////////////////////////

ContactResultsSet::ContactResultsSet(std::shared_ptr<const ComponentInfo> component_info,
                                     const std::variant<ContactResultVector, ContactResultMap>& contact_results,
                                     const std::string& ns)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::CONTACT_RESULTS_SET))
  , ns_(ns)
  , contact_results_(contact_results)
{
}

ContactResultsSet::~ContactResultsSet() = default;

const std::string& ContactResultsSet::getNamespace() const { return ns_; }
const std::variant<ContactResultVector, ContactResultMap>& ContactResultsSet::getContactResults() const
{
  return contact_results_;
}

//////////////////////////////////////////

ContactResultsRemove::ContactResultsRemove(std::shared_ptr<const ComponentInfo> component_info, boost::uuids::uuid uuid)
  : ComponentEventUUID(std::move(component_info), uuid, QEvent::Type(EventType::CONTACT_RESULTS_REMOVE))
{
}
ContactResultsRemove::ContactResultsRemove(std::shared_ptr<const ComponentInfo> component_info,
                                           boost::uuids::uuid uuid,
                                           boost::uuids::uuid child_uuid)
  : ComponentEventUUID(std::move(component_info), uuid, child_uuid, QEvent::Type(EventType::CONTACT_RESULTS_REMOVE))
{
}
ContactResultsRemove::~ContactResultsRemove() = default;

//////////////////////////////////////////

ContactResultsVisbility::ContactResultsVisbility(std::shared_ptr<const ComponentInfo> component_info,
                                                 boost::uuids::uuid uuid,
                                                 bool visible)
  : ComponentEventVisibility(std::move(component_info),
                             uuid,
                             visible,
                             QEvent::Type(EventType::CONTACT_RESULTS_VISIBILITY))
{
}
ContactResultsVisbility::ContactResultsVisbility(std::shared_ptr<const ComponentInfo> component_info,
                                                 boost::uuids::uuid uuid,
                                                 boost::uuids::uuid child_uuid,
                                                 bool visible)
  : ComponentEventVisibility(std::move(component_info),
                             uuid,
                             child_uuid,
                             visible,
                             QEvent::Type(EventType::CONTACT_RESULTS_VISIBILITY))
{
}
ContactResultsVisbility::~ContactResultsVisbility() = default;

//////////////////////////////////////////

ContactResultsVisbilityAll::ContactResultsVisbilityAll(std::shared_ptr<const ComponentInfo> component_info,
                                                       bool visible)
  : ComponentEventVisibilityAll(std::move(component_info),
                                visible,
                                QEvent::Type(EventType::CONTACT_RESULTS_VISIBILITY_ALL))
{
}

ContactResultsVisbilityAll::~ContactResultsVisbilityAll() = default;

//////////////////////////////////////////

ContactResultsCompute::ContactResultsCompute(std::shared_ptr<const ComponentInfo> component_info,
                                             tesseract::collision::ContactManagerConfig contact_manager_config,
                                             tesseract::collision::CollisionCheckConfig collision_check_config,
                                             StateType state_type,
                                             std::string ns)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::CONTACT_RESULTS_COMPUTE))
  , ns_(std::move(ns))
  , contact_manager_config_(std::move(contact_manager_config))
  , collision_check_config_(std::move(collision_check_config))
  , state_type_(std::move(state_type))
{
}

ContactResultsCompute::~ContactResultsCompute() = default;

const std::string& ContactResultsCompute::getNamespace() const { return ns_; }
const tesseract::collision::ContactManagerConfig& ContactResultsCompute::getContactManagerConfig() const
{
  return contact_manager_config_;
}
const tesseract::collision::CollisionCheckConfig& ContactResultsCompute::getCollisionCheckConfig() const
{
  return collision_check_config_;
}
ContactResultsCompute::StateType ContactResultsCompute::getStateType() const { return state_type_; }

//////////////////////////////////////////

}  // namespace tesseract::gui::events
