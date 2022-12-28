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

ToolPathAdd::ToolPathAdd(ComponentInfo component_info, const ToolPath& tool_path)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->tool_path = tool_path;
}
ToolPathAdd::ToolPathAdd(const ToolPathAdd& other) : ToolPathAdd(other.getComponentInfo(), other.getToolPath()) {}
ToolPathAdd::~ToolPathAdd() = default;

const ToolPath& ToolPathAdd::getToolPath() const { return data_->tool_path; }

//////////////////////////////////////////

class ToolPathRemove::Implementation
{
public:
  boost::uuids::uuid uuid;
};

ToolPathRemove::ToolPathRemove(ComponentInfo component_info, const boost::uuids::uuid& uuid)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->uuid = uuid;
}
ToolPathRemove::ToolPathRemove(const ToolPathRemove& other) : ToolPathRemove(other.getComponentInfo(), other.getUUID())
{
}
ToolPathRemove::~ToolPathRemove() = default;

const boost::uuids::uuid& ToolPathRemove::getUUID() const { return data_->uuid; }

//////////////////////////////////////////

ToolPathRemoveAll::ToolPathRemoveAll(ComponentInfo component_info) : ComponentEvent(std::move(component_info), kType) {}
ToolPathRemoveAll::ToolPathRemoveAll(const ToolPathRemoveAll& other) : ToolPathRemoveAll(other.getComponentInfo()) {}
ToolPathRemoveAll::~ToolPathRemoveAll() = default;

ToolPathRemoveSelected::ToolPathRemoveSelected(ComponentInfo component_info)
  : ComponentEvent(std::move(component_info), kType)
{
}
ToolPathRemoveSelected::~ToolPathRemoveSelected() = default;

//////////////////////////////////////////

class ToolPathHide::Implementation
{
public:
  boost::uuids::uuid uuid;
  boost::uuids::uuid child_uuid{};
};

ToolPathHide::ToolPathHide(ComponentInfo component_info, const boost::uuids::uuid& uuid)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->uuid = uuid;
}

ToolPathHide::ToolPathHide(ComponentInfo component_info,
                           const boost::uuids::uuid& uuid,
                           const boost::uuids::uuid& child_uuid)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->uuid = uuid;
  data_->child_uuid = child_uuid;
}
ToolPathHide::ToolPathHide(const ToolPathHide& other)
  : ToolPathHide(other.getComponentInfo(), other.getUUID(), other.getChildUUID())
{
}
ToolPathHide::~ToolPathHide() = default;

const boost::uuids::uuid& ToolPathHide::getUUID() const { return data_->uuid; }
const boost::uuids::uuid& ToolPathHide::getChildUUID() const { return data_->child_uuid; }

//////////////////////////////////////////

ToolPathHideAll::ToolPathHideAll(ComponentInfo component_info) : ComponentEvent(std::move(component_info), kType) {}
ToolPathHideAll::ToolPathHideAll(const ToolPathHideAll& other) : ToolPathHideAll(other.getComponentInfo()) {}
ToolPathHideAll::~ToolPathHideAll() = default;

//////////////////////////////////////////

class ToolPathShow::Implementation
{
public:
  boost::uuids::uuid uuid;
  boost::uuids::uuid child_uuid{};
};

ToolPathShow::ToolPathShow(ComponentInfo component_info, const boost::uuids::uuid& uuid)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->uuid = uuid;
}

ToolPathShow::ToolPathShow(ComponentInfo component_info,
                           const boost::uuids::uuid& uuid,
                           const boost::uuids::uuid& child_uuid)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->uuid = uuid;
  data_->child_uuid = child_uuid;
}
ToolPathShow::ToolPathShow(const ToolPathShow& other)
  : ToolPathShow(other.getComponentInfo(), other.getUUID(), other.getChildUUID())
{
}
ToolPathShow::~ToolPathShow() = default;

const boost::uuids::uuid& ToolPathShow::getUUID() const { return data_->uuid; }
const boost::uuids::uuid& ToolPathShow::getChildUUID() const { return data_->child_uuid; }

//////////////////////////////////////////

ToolPathShowAll::ToolPathShowAll(ComponentInfo component_info) : ComponentEvent(std::move(component_info), kType) {}
ToolPathShowAll::ToolPathShowAll(const ToolPathShowAll& other) : ToolPathShowAll(other.getComponentInfo()) {}
ToolPathShowAll::~ToolPathShowAll() = default;

}  // namespace tesseract_gui::events
