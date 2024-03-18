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

#include <tesseract_qt/common/events/component_events.h>
#include <tesseract_qt/common/component_info.h>

#include <boost/uuid/uuid.hpp>

namespace tesseract_gui::events
{
ComponentEvent::ComponentEvent(std::shared_ptr<const ComponentInfo> component_info, QEvent::Type type)
  : QEvent(type), component_info_(std::move(component_info))
{
}
ComponentEvent::~ComponentEvent() = default;
std::shared_ptr<const ComponentInfo> ComponentEvent::getComponentInfo() const { return component_info_; }

//////////////////////////////////////////

class ComponentEventUUID::Implementation
{
public:
  boost::uuids::uuid uuid{};
  boost::uuids::uuid child_uuid{};
};

ComponentEventUUID::ComponentEventUUID(std::shared_ptr<const ComponentInfo> component_info,
                                       boost::uuids::uuid uuid,
                                       QEvent::Type type)
  : ComponentEvent(std::move(component_info), type), data_(std::make_unique<Implementation>())
{
  data_->uuid = uuid;
}

ComponentEventUUID::ComponentEventUUID(std::shared_ptr<const ComponentInfo> component_info,
                                       boost::uuids::uuid uuid,
                                       boost::uuids::uuid child_uuid,
                                       QEvent::Type type)
  : ComponentEvent(std::move(component_info), type), data_(std::make_unique<Implementation>())
{
  data_->uuid = uuid;
  data_->child_uuid = child_uuid;
}

ComponentEventUUID::ComponentEventUUID(const ComponentEventUUID& other)
  : ComponentEventUUID(other.getComponentInfo(), other.getUUID(), other.getChildUUID(), other.type())
{
}

ComponentEventUUID::~ComponentEventUUID() = default;

const boost::uuids::uuid& ComponentEventUUID::getUUID() const { return data_->uuid; }

const boost::uuids::uuid& ComponentEventUUID::getChildUUID() const { return data_->child_uuid; }

//////////////////////////////////////////

ComponentEventVisibility::ComponentEventVisibility(std::shared_ptr<const ComponentInfo> component_info,
                                                   boost::uuids::uuid uuid,
                                                   bool visible,
                                                   QEvent::Type type)
  : ComponentEventUUID(std::move(component_info), uuid, type), visible_(visible)
{
}

ComponentEventVisibility::ComponentEventVisibility(std::shared_ptr<const ComponentInfo> component_info,
                                                   boost::uuids::uuid uuid,
                                                   boost::uuids::uuid child_uuid,
                                                   bool visible,
                                                   QEvent::Type type)
  : ComponentEventUUID(std::move(component_info), uuid, child_uuid, type), visible_(visible)
{
}
ComponentEventVisibility::~ComponentEventVisibility() = default;

bool ComponentEventVisibility::getVisibility() const { return visible_; }

//////////////////////////////////////////

ComponentEventVisibilityAll::ComponentEventVisibilityAll(std::shared_ptr<const ComponentInfo> component_info,
                                                         bool visible,
                                                         QEvent::Type type)
  : ComponentEvent(std::move(component_info), type), visible_(visible)
{
}

ComponentEventVisibilityAll::~ComponentEventVisibilityAll() = default;

bool ComponentEventVisibilityAll::getVisibility() const { return visible_; }
}  // namespace tesseract_gui::events
