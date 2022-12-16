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

#include <tesseract_qt/common/scene_events.h>
#include <boost/uuid/uuid.hpp>

namespace tesseract_gui::events
{
SceneEvent::SceneEvent(std::string scene_name, QEvent::Type type) : QEvent(type), scene_name_(std::move(scene_name)) {}
SceneEvent::~SceneEvent() = default;
const std::string& SceneEvent::getSceneName() const { return scene_name_; }

//////////////////////////////////////////

class SceneEventUUID::Implementation
{
public:
  boost::uuids::uuid uuid{};
  boost::uuids::uuid child_uuid{};
};

SceneEventUUID::SceneEventUUID(std::string scene_name, boost::uuids::uuid uuid, QEvent::Type type)
  : SceneEvent(std::move(scene_name), type), data_(std::make_unique<Implementation>())
{
  data_->uuid = uuid;
}

SceneEventUUID::SceneEventUUID(std::string scene_name,
                               boost::uuids::uuid uuid,
                               boost::uuids::uuid child_uuid,
                               QEvent::Type type)
  : SceneEvent(std::move(scene_name), type), data_(std::make_unique<Implementation>())
{
  data_->uuid = uuid;
  data_->child_uuid = child_uuid;
}
SceneEventUUID::~SceneEventUUID() = default;

const boost::uuids::uuid& SceneEventUUID::getUUID() const { return data_->uuid; }

const boost::uuids::uuid& SceneEventUUID::getChildUUID() const { return data_->child_uuid; }

//////////////////////////////////////////

SceneEventVisibility::SceneEventVisibility(std::string scene_name,
                                           boost::uuids::uuid uuid,
                                           bool visible,
                                           QEvent::Type type)
  : SceneEventUUID(std::move(scene_name), uuid, type), visible_(visible)
{
}

SceneEventVisibility::SceneEventVisibility(std::string scene_name,
                                           boost::uuids::uuid uuid,
                                           boost::uuids::uuid child_uuid,
                                           bool visible,
                                           QEvent::Type type)
  : SceneEventUUID(std::move(scene_name), uuid, child_uuid, type), visible_(visible)
{
}
SceneEventVisibility::~SceneEventVisibility() = default;

bool SceneEventVisibility::getVisibility() const { return visible_; }

//////////////////////////////////////////

SceneEventVisibilityAll::SceneEventVisibilityAll(std::string scene_name, bool visible, QEvent::Type type)
  : SceneEvent(std::move(scene_name), type), visible_(visible)
{
}

SceneEventVisibilityAll::~SceneEventVisibilityAll() = default;

bool SceneEventVisibilityAll::getVisibility() const { return visible_; }
}  // namespace tesseract_gui::events
