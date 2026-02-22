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
#ifndef TESSERACT_QT_COMMON_CONTACT_RESULTS_EVENTS_H
#define TESSERACT_QT_COMMON_CONTACT_RESULTS_EVENTS_H

#include <memory>
#include <variant>
#include <tesseract_scene_graph/fwd.h>

#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>
#include <tesseract_qt/common/contact_results_types.h>

namespace tesseract::gui::events
{
class ContactResultsClear : public ComponentEvent
{
public:
  ContactResultsClear(std::shared_ptr<const ComponentInfo> component_info, const std::string& ns = "");
  ~ContactResultsClear() override;

  const std::string& getNamespace() const;

private:
  std::string ns_;
};

class ContactResultsSet : public ComponentEvent
{
public:
  ContactResultsSet(std::shared_ptr<const ComponentInfo> component_info,
                    const std::variant<ContactResultVector, ContactResultMap>& contact_results,
                    const std::string& ns = "");
  ~ContactResultsSet() override;

  const std::string& getNamespace() const;
  const std::variant<ContactResultVector, ContactResultMap>& getContactResults() const;

private:
  std::string ns_;
  std::variant<ContactResultVector, ContactResultMap> contact_results_;
};

class ContactResultsRemove : public ComponentEventUUID
{
public:
  ContactResultsRemove(std::shared_ptr<const ComponentInfo> component_info, boost::uuids::uuid uuid);
  ContactResultsRemove(std::shared_ptr<const ComponentInfo> component_info,
                       boost::uuids::uuid uuid,
                       boost::uuids::uuid child_uuid);
  ~ContactResultsRemove() override;
};

class ContactResultsVisbility : public ComponentEventVisibility
{
public:
  ContactResultsVisbility(std::shared_ptr<const ComponentInfo> component_info, boost::uuids::uuid uuid, bool visible);
  ContactResultsVisbility(std::shared_ptr<const ComponentInfo> component_info,
                          boost::uuids::uuid uuid,
                          boost::uuids::uuid child_uuid,
                          bool visible);
  ~ContactResultsVisbility() override;
};

class ContactResultsVisbilityAll : public ComponentEventVisibilityAll
{
public:
  ContactResultsVisbilityAll(std::shared_ptr<const ComponentInfo> component_info, bool visible);
  ~ContactResultsVisbilityAll() override;
};

class ContactResultsCompute : public ComponentEvent
{
public:
  enum StateType
  {
    CURRENT_STATE,
    NAMED_STATE,
    USER_DEFINED_STATE
  };

  ContactResultsCompute(std::shared_ptr<const ComponentInfo> component_info,
                        tesseract::collision::ContactManagerConfig contact_manager_config,
                        tesseract::collision::CollisionCheckConfig collision_check_config,
                        StateType state_type = CURRENT_STATE,
                        std::string ns = "");
  ~ContactResultsCompute() override;

  const std::string& getNamespace() const;
  const tesseract::collision::ContactManagerConfig& getContactManagerConfig() const;
  const tesseract::collision::CollisionCheckConfig& getCollisionCheckConfig() const;
  StateType getStateType() const;

private:
  std::string ns_;
  tesseract::collision::ContactManagerConfig contact_manager_config_;
  tesseract::collision::CollisionCheckConfig collision_check_config_;
  StateType state_type_;
};

}  // namespace tesseract::gui::events

#endif  // TESSERACT_QT_COMMON_CONTACT_RESULTS_EVENTS_H
