/**
 * @file tool_path.cpp
 * @brief Common Tesseract Tool Path
 *
 * @author Levi Armstrong
 * @date Nov 16, 2022
 *
 * @copyright Copyright (c) 2022, Levi Armstrong
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <boost/uuid/uuid_generators.hpp>

#include <tesseract_qt/common/tool_path_pose.h>

namespace tesseract::gui
{
ToolPathPose::ToolPathPose(std::string description)
  : uuid_(boost::uuids::random_generator()()), description_(std::move(description))
{
}

ToolPathPose::ToolPathPose(boost::uuids::uuid uuid, std::string description)
  : uuid_(uuid), description_(std::move(description))
{
}

ToolPathPose::ToolPathPose(const Eigen::Isometry3d& pose, std::string description)
  : uuid_(boost::uuids::random_generator()()), description_(std::move(description)), transform_(pose)
{
}

ToolPathPose::ToolPathPose(const Eigen::Isometry3d& pose, boost::uuids::uuid uuid, std::string description)
  : uuid_(uuid), description_(std::move(description)), transform_(pose)
{
}

boost::uuids::uuid ToolPathPose::getUUID() const { return uuid_; }

void ToolPathPose::regenerateUUID() { uuid_ = boost::uuids::random_generator()(); }

const boost::uuids::uuid& ToolPathPose::getParentUUID() const { return parent_uuid_; }

void ToolPathPose::setParentUUID(const boost::uuids::uuid& uuid) { parent_uuid_ = uuid; }

void ToolPathPose::setDescription(const std::string& desc) { description_ = desc; }
const std::string& ToolPathPose::getDescription() const { return description_; }

const Eigen::Isometry3d& ToolPathPose::getTransform() const { return transform_; }

void ToolPathPose::setTransform(const Eigen::Isometry3d& transform) { transform_ = transform; }

bool ToolPathPose::operator==(const ToolPathPose& rhs) const
{
  bool equal = true;
  equal &= (uuid_ == rhs.uuid_);                // NOLINT
  equal &= (parent_uuid_ == rhs.parent_uuid_);  // NOLINT
  equal &= (description_ == rhs.description_);  // NOLINT
  equal &= (transform_.isApprox(rhs.transform_, std::numeric_limits<float>::epsilon()));
  return equal;
}

bool ToolPathPose::operator!=(const ToolPathPose& rhs) const { return !operator==(rhs); }

}  // namespace tesseract::gui
