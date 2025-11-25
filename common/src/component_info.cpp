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

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>

namespace tesseract_gui
{
ComponentInfo::ComponentInfo() {}

ComponentInfo::ComponentInfo(std::string scene_name, std::string name, std::string description)
  : ComponentInfo(std::move(scene_name),
                  std::move(name),
                  boost::uuids::random_generator()(),
                  nullptr,
                  std::move(description))
{
}

ComponentInfo::ComponentInfo(std::string scene_name,
                             std::string name,
                             std::shared_ptr<const ComponentInfo> parent,
                             std::string description)
  : ComponentInfo(std::move(scene_name),
                  std::move(name),
                  boost::uuids::random_generator()(),
                  std::move(parent),
                  std::move(description))
{
}

ComponentInfo::ComponentInfo(std::string scene_name,
                             std::string name,
                             const boost::uuids::uuid& ns,
                             std::shared_ptr<const ComponentInfo> parent,
                             std::string description)
  : scene_name_(std::move(scene_name))
  , name_(std::move(name))
  , description_(std::move(description))
  , ns_(std::move(ns))
  , parent_({ std::move(parent) })
{
}

const std::string& ComponentInfo::getName() const { return name_; }

const std::string& ComponentInfo::getSceneName() const { return scene_name_; }

const std::string& ComponentInfo::getDescription() const { return description_; }
void ComponentInfo::setDescription(const std::string& description) { description_ = description; }

const boost::uuids::uuid& ComponentInfo::getNamespace() const { return ns_; }

void getLineageRecursive(std::list<boost::uuids::uuid>& lineage, const tesseract_gui::ComponentInfo& component_info)
{
  lineage.push_back(component_info.getNamespace());

  if (!component_info.hasParent())
    return;

  getLineageRecursive(lineage, *component_info.getParentComponentInfo());
}
std::list<boost::uuids::uuid> ComponentInfo::getLineage() const
{
  if (!hasParent())
    return {};

  std::list<boost::uuids::uuid> lineage;
  getLineageRecursive(lineage, *parent_);
  return lineage;
}

bool ComponentInfo::hasParent() const { return (parent_ != nullptr); }

std::shared_ptr<const ComponentInfo> ComponentInfo::getParentComponentInfo() const { return parent_; }

std::shared_ptr<ComponentInfo> ComponentInfo::createChild() const { return ComponentInfoManager::createChild(name_); }

std::shared_ptr<ComponentInfo> ComponentInfo::createChild(const std::string& name) const
{
  return ComponentInfoManager::createChild(name_, name);
}

bool isParentRecursive(const ComponentInfo* check, const ComponentInfo* provided)
{
  if (check == provided)
    return true;

  if (!check->hasParent())
    return false;

  return isParentRecursive(check->getParentComponentInfo().get(), provided);
}

bool ComponentInfo::isParent(const std::shared_ptr<const ComponentInfo>& other) const
{
  if (!hasParent() || other == nullptr)
    return false;

  return isParentRecursive(parent_.get(), other.get());
}

bool ComponentInfo::isChild(const std::shared_ptr<const ComponentInfo>& other) const
{
  if (!other->hasParent() || other == nullptr)
    return false;

  return isParentRecursive(other.get(), this);
}

std::string ComponentInfo::toString() const
{
  std::string str;
  str += "Name: " + name_ + "\n";
  str += "Scene: " + scene_name_ + "\n";
  str += "Namespace: " + boost::uuids::to_string(ns_) + "\n";
  str += "Description: " + description_ + "\n";
  str += "Child: " + std::string((parent_ == nullptr) ? "False" : "True");

  return str;
}

bool ComponentInfo::operator==(const ComponentInfo& rhs) const
{
  return (scene_name_ == rhs.scene_name_ && ns_ == rhs.ns_);
}

bool ComponentInfo::operator!=(const ComponentInfo& rhs) const { return !operator==(rhs); }

}  // namespace tesseract_gui
