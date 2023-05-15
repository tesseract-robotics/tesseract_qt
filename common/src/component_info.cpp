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
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/common/component_info.h>

namespace tesseract_gui
{
ComponentInfo::ComponentInfo() : ComponentInfo("tesseract_default") {}

ComponentInfo::ComponentInfo(std::string scene_name, std::string description)
  : ComponentInfo(std::move(scene_name),
                  { boost::uuids::to_string(boost::uuids::random_generator()()) },
                  std::move(description))
{
}

ComponentInfo::ComponentInfo(std::string scene_name, std::list<std::string> ns, std::string description)
  : scene_name_(std::move(scene_name)), description_(std::move(description)), ns_({ std::move(ns) })
{
}

const std::string& ComponentInfo::getSceneName() const { return scene_name_; }

const std::string& ComponentInfo::getDescription() const { return description_; }
void ComponentInfo::setDescription(const std::string& description) { description_ = description; }

const std::string& ComponentInfo::getNamespace() const { return ns_.front(); }

std::list<std::string> ComponentInfo::getLineage() const
{
  if (!hasParent())
    return {};

  return std::list<std::string>{ std::next(ns_.begin(), 1), ns_.end() };
}

bool ComponentInfo::hasParent() const { return (ns_.size() > 1); }

ComponentInfo ComponentInfo::getParentComponentInfo() const
{
  if (!hasParent())
    throw std::runtime_error("Tried to get parent component info when not a child");

  ComponentInfo component_info;
  component_info.scene_name_ = scene_name_;
  component_info.ns_ = std::list<std::string>{ std::next(ns_.begin(), 1), ns_.end() };

  return component_info;
}

ComponentInfo ComponentInfo::createChild() const
{
  ComponentInfo child{ *this };
  child.ns_.push_front(boost::uuids::to_string(boost::uuids::random_generator()()));
  return child;
}

bool ComponentInfo::isParent(const ComponentInfo& other) const
{
  if (!hasParent())
    return false;

  if (other.ns_.size() >= ns_.size())
    return false;

  std::list<std::string> parent_ns(std::prev(ns_.end(), other.ns_.size()), ns_.end());
  return (scene_name_ == other.scene_name_ && parent_ns == other.ns_);
}

bool ComponentInfo::isChild(const ComponentInfo& other) const
{
  if (!other.hasParent())
    return false;

  if (ns_.size() >= other.ns_.size())
    return false;

  std::list<std::string> parent_ns(std::prev(other.ns_.end(), ns_.size()), other.ns_.end());
  return (scene_name_ == other.scene_name_ && parent_ns == ns_);
}

bool ComponentInfo::operator==(const ComponentInfo& rhs) const
{
  return (scene_name_ == rhs.scene_name_ && ns_ == rhs.ns_);
}

bool ComponentInfo::operator!=(const ComponentInfo& rhs) const { return !operator==(rhs); }

template <class Archive>
void ComponentInfo::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& BOOST_SERIALIZATION_NVP(scene_name_);
  ar& BOOST_SERIALIZATION_NVP(ns_);
  ar& BOOST_SERIALIZATION_NVP(description_);
}

}  // namespace tesseract_gui

#include <tesseract_common/serialization.h>
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_gui::ComponentInfo)
BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_gui::ComponentInfo)
