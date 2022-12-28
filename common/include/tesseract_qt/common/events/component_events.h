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
#ifndef TESSERACT_GUI_COMMON_COMPONENT_EVENTS_H
#define TESSERACT_GUI_COMMON_COMPONENT_EVENTS_H

#include <tesseract_qt/common/component_info.h>

#include <QEvent>
#include <memory>

namespace boost::uuids
{
struct uuid;
}

namespace tesseract_gui::events
{
/** @brief Event called  */
class ComponentEvent : public QEvent
{
public:
  ComponentEvent(ComponentInfo component_info, QEvent::Type type);
  ~ComponentEvent() override;

  /** @brief Get the scene name the event is associated with */
  const ComponentInfo& getComponentInfo() const;

private:
  ComponentInfo component_info_;
};

class ComponentEventUUID : public ComponentEvent
{
public:
  ComponentEventUUID(ComponentInfo component_info, boost::uuids::uuid uuid, QEvent::Type type);
  ComponentEventUUID(ComponentInfo component_info,
                     boost::uuids::uuid uuid,
                     boost::uuids::uuid child_uuid,
                     QEvent::Type type);
  ~ComponentEventUUID() override;

  /** @brief Get the uuid the event is associated with */
  const boost::uuids::uuid& getUUID() const;

  /** @brief Get the child uuid the event is associated with and allowed to be null */
  const boost::uuids::uuid& getChildUUID() const;

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

class ComponentEventVisibility : public ComponentEventUUID
{
public:
  ComponentEventVisibility(ComponentInfo component_info, boost::uuids::uuid uuid, bool visible, QEvent::Type type);
  ComponentEventVisibility(ComponentInfo component_info,
                           boost::uuids::uuid uuid,
                           boost::uuids::uuid child_uuid,
                           bool visible,
                           QEvent::Type type);
  ~ComponentEventVisibility() override;

  /** @brief Get the visibility */
  bool getVisibility() const;

private:
  bool visible_;
};

class ComponentEventVisibilityAll : public ComponentEvent
{
public:
  ComponentEventVisibilityAll(ComponentInfo component_info, bool visible, QEvent::Type type);
  ~ComponentEventVisibilityAll() override;

  /** @brief Get the visibility */
  bool getVisibility() const;

private:
  bool visible_;
};
}  // namespace tesseract_gui::events
#endif  // TESSERACT_GUI_COMMON_COMPONENT_EVENTS_H
