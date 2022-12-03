/**
 * @file tool_path_pose.h
 * @brief Common Tesseract Tool Path Pose
 *
 * @author Levi Armstrong
 * @date Nov 16, 2022
 * @version TODO
 * @bug No known bugs
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
#ifndef TESSERACT_QT_COMMON_TOOL_PATH_POSE_H
#define TESSERACT_QT_COMMON_TOOL_PATH_POSE_H

#include <string>
#include <boost/uuid/uuid.hpp>
#include <tesseract_common/serialization.h>

namespace tesseract_gui
{
class ToolPathPose
{
public:
  // LCOV_EXCL_START
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  // LCOV_EXCL_STOP

  ToolPathPose(std::string description = "");
  ToolPathPose(boost::uuids::uuid uuid, std::string description = "");
  ToolPathPose(const Eigen::Isometry3d& pose, std::string description = "");
  ToolPathPose(const Eigen::Isometry3d& pose, boost::uuids::uuid uuid, std::string description = "");
  virtual ~ToolPathPose() = default;

  /** @brief Get the uuid */
  boost::uuids::uuid getUUID() const;

  /** @brief Regenerate uuid */
  void regenerateUUID();

  /**
   * @brief Get the parent uuid
   * @details This can be null
   */
  const boost::uuids::uuid& getParentUUID() const;

  /**
   * @brief Set the parent uuid
   * @details This can be used in cases were a segment is split during a filter process and want tracability.
   * @param uuid
   */
  void setParentUUID(const boost::uuids::uuid& uuid);

  /** @brief Set the segment description */
  void setDescription(const std::string& desc);

  /** @brief Get the segment description */
  const std::string& getDescription() const;

  /** @brief Get the transform */
  const Eigen::Isometry3d& getTransform() const;

  /** @brief Set the transform */
  void setTransform(const Eigen::Isometry3d& transform);

  bool operator==(const ToolPathPose& rhs) const;
  bool operator!=(const ToolPathPose& rhs) const;

protected:
  /** @brief The uuid */
  boost::uuids::uuid uuid_{};

  /** @brief The parent uuid */
  boost::uuids::uuid parent_uuid_{};

  /** @brief The description */
  std::string description_;

  /** @brief The transform */
  Eigen::Isometry3d transform_{ Eigen::Isometry3d::Identity() };

  friend struct tesseract_common::Serialization;
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version);  // NOLINT
};

}  // namespace tesseract_gui

#include <boost/serialization/export.hpp>
#include <boost/serialization/tracking.hpp>
BOOST_CLASS_EXPORT_KEY2(tesseract_gui::ToolPathPose, "ToolPathPose")

#endif  // TESSERACT_QT_COMMON_TOOL_PATH_POSE_H
