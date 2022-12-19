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

namespace tesseract_gui::events
{
ContactResultsClear::ContactResultsClear(const std::string& scene_name, const std::string& ns)
  : SceneEvent(scene_name, kType), ns_(ns)
{
}

ContactResultsClear::~ContactResultsClear() = default;

const std::string& ContactResultsClear::getNamespace() const { return ns_; }

//////////////////////////////////////////

ContactResultsSet::ContactResultsSet(const std::string& scene_name,
                                     const std::variant<ContactResultVector, ContactResultMap>& contact_results,
                                     const std::string& ns)
  : SceneEvent(scene_name, kType), ns_(ns), contact_results_(contact_results)
{
}

ContactResultsSet::~ContactResultsSet() = default;

const std::string& ContactResultsSet::getNamespace() const { return ns_; }
const std::variant<ContactResultVector, ContactResultMap>& ContactResultsSet::getContactResults() const
{
  return contact_results_;
}

//////////////////////////////////////////

ContactResultsRemove::ContactResultsRemove(const std::string& scene_name, boost::uuids::uuid uuid)
  : SceneEventUUID(scene_name, uuid, kType)
{
}
ContactResultsRemove::ContactResultsRemove(const std::string& scene_name,
                                           boost::uuids::uuid uuid,
                                           boost::uuids::uuid child_uuid)
  : SceneEventUUID(scene_name, uuid, child_uuid, kType)
{
}
ContactResultsRemove::~ContactResultsRemove() = default;

//////////////////////////////////////////

ContactResultsVisbility::ContactResultsVisbility(const std::string& scene_name, boost::uuids::uuid uuid, bool visible)
  : SceneEventVisibility(scene_name, uuid, visible, kType)
{
}
ContactResultsVisbility::ContactResultsVisbility(const std::string& scene_name,
                                                 boost::uuids::uuid uuid,
                                                 boost::uuids::uuid child_uuid,
                                                 bool visible)
  : SceneEventVisibility(scene_name, uuid, child_uuid, visible, kType)
{
}
ContactResultsVisbility::~ContactResultsVisbility() = default;

//////////////////////////////////////////

ContactResultsVisbilityAll::ContactResultsVisbilityAll(const std::string& scene_name, bool visible)
  : SceneEventVisibilityAll(scene_name, visible, kType)
{
}

ContactResultsVisbilityAll::~ContactResultsVisbilityAll() = default;

}  // namespace tesseract_gui::events
