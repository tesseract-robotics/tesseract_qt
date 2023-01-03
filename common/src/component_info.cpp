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

#include <tesseract_qt/common/component_info.h>
#include <boost/uuid/random_generator.hpp>

namespace tesseract_gui
{
ComponentInfo::ComponentInfo(std::string scene_name)
  : scene_name(std::move(scene_name)), ns(boost::uuids::random_generator()())
{
}

bool ComponentInfo::hasParent() const { return parent_ != nullptr; }

std::shared_ptr<const ComponentInfo> ComponentInfo::getParent() const { return parent_; }

ComponentInfo ComponentInfo::createChild() const
{
  ComponentInfo child{ scene_name };
  child.parent_ = std::make_shared<ComponentInfo>(*this);
  return child;
}

bool ComponentInfo::isParent(const ComponentInfo& other) const
{
  if (parent_ == nullptr)
    return false;

  return (scene_name == other.scene_name && *parent_ == other);
}

bool ComponentInfo::isChild(const ComponentInfo& other) const
{
  if (other.parent_ == nullptr)
    return false;

  return (scene_name == other.scene_name && *this == *other.parent_);
}

bool ComponentInfo::operator==(const ComponentInfo& rhs) const
{
  return (scene_name == rhs.scene_name && ns == rhs.ns && parent_ == rhs.parent_);
}

bool ComponentInfo::operator!=(const ComponentInfo& rhs) const { return !operator==(rhs); }
}  // namespace tesseract_gui
