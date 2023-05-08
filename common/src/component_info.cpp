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

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/common/component_info.h>

namespace tesseract_gui
{
ComponentInfo::ComponentInfo(std::string scene_name, std::string description)
  : scene_name(std::move(scene_name))
  , ns(boost::uuids::to_string(boost::uuids::random_generator()()))
  , description(std::move(description))
{
}

ComponentInfo::ComponentInfo(std::string scene_name, std::string ns, std::string description)
  : scene_name(std::move(scene_name)), ns(std::move(ns)), description(std::move(description))
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

template <class Archive>
void ComponentInfo::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& BOOST_SERIALIZATION_NVP(scene_name);
  ar& BOOST_SERIALIZATION_NVP(ns);
  ar& BOOST_SERIALIZATION_NVP(description);
  ar& BOOST_SERIALIZATION_NVP(parent_);
}

}  // namespace tesseract_gui

#include <tesseract_common/serialization.h>
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_gui::ComponentInfo)
BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_gui::ComponentInfo)
