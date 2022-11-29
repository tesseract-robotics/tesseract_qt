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
#include <tesseract_qt/common/tool_path.h>
#include <string>
#include <boost/uuid/uuid.hpp>

namespace tesseract_gui::events
{
class ToolPathAdd::Implementation
{
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  tesseract_gui::ToolPath tool_path;
};

ToolPathAdd::ToolPathAdd(const std::string& scene_name, const ToolPath& tool_path)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->tool_path = tool_path;
}
ToolPathAdd::~ToolPathAdd() = default;

const ToolPath& ToolPathAdd::getToolPath() const { return data_->tool_path; }

//////////////////////////////////////////

class ToolPathRemove::Implementation
{
public:
  boost::uuids::uuid uuid;
};

ToolPathRemove::ToolPathRemove(const std::string& scene_name, const boost::uuids::uuid& uuid)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->uuid = uuid;
}
ToolPathRemove::~ToolPathRemove() = default;

const boost::uuids::uuid& ToolPathRemove::getUUID() const { return data_->uuid; }

ToolPathRemoveAll::ToolPathRemoveAll(const std::string& scene_name) : SceneEvent(scene_name, kType) {}
ToolPathRemoveAll::~ToolPathRemoveAll() = default;

ToolPathRemoveSelected::ToolPathRemoveSelected(const std::string& scene_name) : SceneEvent(scene_name, kType) {}
ToolPathRemoveSelected::~ToolPathRemoveSelected() = default;

class ToolPathHide::Implementation
{
public:
  boost::uuids::uuid uuid;
};

ToolPathHide::ToolPathHide(const std::string& scene_name, const boost::uuids::uuid& uuid)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->uuid = uuid;
}
ToolPathHide::~ToolPathHide() = default;

const boost::uuids::uuid& ToolPathHide::getUUID() const { return data_->uuid; }

ToolPathHideAll::ToolPathHideAll(const std::string& scene_name) : SceneEvent(scene_name, kType) {}
ToolPathHideAll::~ToolPathHideAll() = default;

class ToolPathShow::Implementation
{
public:
  boost::uuids::uuid uuid;
};

ToolPathShow::ToolPathShow(const std::string& scene_name, const boost::uuids::uuid& uuid)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->uuid = uuid;
}
ToolPathShow::~ToolPathShow() = default;

const boost::uuids::uuid& ToolPathShow::getUUID() const { return data_->uuid; }

ToolPathShowAll::ToolPathShowAll(const std::string& scene_name) : SceneEvent(scene_name, kType) {}
ToolPathShowAll::~ToolPathShowAll() = default;

}  // namespace tesseract_gui::events
