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
#ifndef TESSERACT_QT_COLLISION_CONTACT_RESULTS_EVENTS_H
#define TESSERACT_QT_COLLISION_CONTACT_RESULTS_EVENTS_H

#include <memory>
#include <tesseract_qt/common/event_type.h>
#include <tesseract_qt/common/scene_events.h>
#include <tesseract_qt/collision/contact_results_types.h>

namespace tesseract_gui::events
{
class ContactResultsClear : public SceneEvent
{
public:
  ContactResultsClear(const std::string& scene_name, const std::string& ns = "");
  ~ContactResultsClear() override;

  const std::string& getNamespace() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::CONTACT_RESULTS_CLEAR);

private:
  std::string ns_;
};

class ContactResultsAdd : public SceneEvent
{
public:
  ContactResultsAdd(const std::string& scene_name,
                    const std::variant<ContactResultVector, ContactResultMap>& contact_results,
                    const std::string& ns = "");
  ~ContactResultsAdd() override;

  const std::string& getNamespace() const;
  const std::variant<ContactResultVector, ContactResultMap>& getContactResults() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::CONTACT_RESULTS_ADD);

private:
  std::string ns_;
  std::variant<ContactResultVector, ContactResultMap> contact_results_;
};

class ContactResultsRemove : public SceneEventUUID
{
public:
  ContactResultsRemove(const std::string& scene_name, boost::uuids::uuid uuid);
  ContactResultsRemove(const std::string& scene_name, boost::uuids::uuid uuid, boost::uuids::uuid child_uuid);
  ~ContactResultsRemove() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::CONTACT_RESULTS_REMOVE);
};

class ContactResultsVisbility : public SceneEventVisibility
{
public:
  ContactResultsVisbility(const std::string& scene_name, boost::uuids::uuid uuid, bool visible);
  ContactResultsVisbility(const std::string& scene_name,
                          boost::uuids::uuid uuid,
                          boost::uuids::uuid child_uuid,
                          bool visible);
  ~ContactResultsVisbility() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::CONTACT_RESULTS_VISIBILITY);
};

class ContactResultsVisbilityAll : public SceneEventVisibilityAll
{
public:
  ContactResultsVisbilityAll(const std::string& scene_name, bool visible);
  ~ContactResultsVisbilityAll() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::CONTACT_RESULTS_VISIBILITY_ALL);
};

}  // namespace tesseract_gui::events

#endif  // TESSERACT_QT_COLLISION_CONTACT_RESULTS_EVENTS_H
