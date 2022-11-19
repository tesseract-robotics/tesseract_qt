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
#include <tesseract_qt/tool_path/tool_path_events.h>
#include <tesseract_common/tool_path.h>
#include <string>
#include <boost/uuid/uuid.hpp>

namespace tesseract_gui::events
{
class AddToolPath::Implementation
{
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  /// The scene the event is associated with
  std::string scene_name;

  tesseract_common::ToolPath tool_path;
};

AddToolPath::AddToolPath(const std::string& scene_name, const tesseract_common::ToolPath& tool_path)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->scene_name = scene_name;
  data_->tool_path = tool_path;
}
AddToolPath::~AddToolPath() = default;

const std::string& AddToolPath::getSceneName() const { return data_->scene_name; }
const tesseract_common::ToolPath& AddToolPath::getToolPath() const { return data_->tool_path; }

//////////////////////////////////////////

class RemoveToolPath::Implementation
{
public:
  /// The scene the event is associated with
  std::string scene_name;

  boost::uuids::uuid uuid;
};

RemoveToolPath::RemoveToolPath(const std::string& scene_name, const boost::uuids::uuid& uuid)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->scene_name = scene_name;
  data_->uuid = uuid;
}
RemoveToolPath::~RemoveToolPath() = default;

const std::string& RemoveToolPath::getSceneName() const { return data_->scene_name; }
const boost::uuids::uuid& RemoveToolPath::getUUID() const { return data_->uuid; }
}  // namespace tesseract_gui::events
