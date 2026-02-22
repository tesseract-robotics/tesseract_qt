#ifndef TESSERACT_GUI_COMMON_CEREAL_SERIALIZATION_H
#define TESSERACT_GUI_COMMON_CEREAL_SERIALIZATION_H

#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/joint_trajectory_set.h>
#include <tesseract_qt/common/tool_path.h>
#include <tesseract_qt/common/tool_path_pose.h>
#include <tesseract_qt/common/tool_path_segment.h>

#include <tesseract_common/cereal_serialization.h>

#include <cereal/cereal.hpp>

namespace tesseract::gui
{
template <class Archive>
void serialize(Archive& ar, ComponentInfo& obj)
{
  ar(cereal::make_nvp("scene_name", obj.scene_name_));
  ar(cereal::make_nvp("name", obj.name_));
  ar(cereal::make_nvp("ns", obj.ns_));
  ar(cereal::make_nvp("parent", obj.parent_));
  ar(cereal::make_nvp("description", obj.description_));
}

template <class Archive>
void serialize(Archive& ar, ToolPath& obj)
{
  ar(cereal::make_nvp("uuid", obj.uuid_));
  ar(cereal::make_nvp("parent_uuid", obj.parent_uuid_));
  ar(cereal::make_nvp("description", obj.description_));
  ar(cereal::make_nvp("container", obj.container_));
  ar(cereal::make_nvp("working_frame", obj.working_frame_));
  ar(cereal::make_nvp("ns", obj.ns_));
}

template <class Archive>
void serialize(Archive& ar, ToolPathPose& obj)
{
  ar(cereal::make_nvp("uuid", obj.uuid_));
  ar(cereal::make_nvp("parent_uuid", obj.parent_uuid_));
  ar(cereal::make_nvp("description", obj.description_));
  ar(cereal::make_nvp("transform", obj.transform_));
}

template <class Archive>
void serialize(Archive& ar, ToolPathSegment& obj)
{
  ar(cereal::make_nvp("uuid", obj.uuid_));
  ar(cereal::make_nvp("parent_uuid", obj.parent_uuid_));
  ar(cereal::make_nvp("description", obj.description_));
  ar(cereal::make_nvp("container", obj.container_));
}

}  // namespace tesseract::gui

namespace tesseract::common
{
template <class Archive>
void serialize(Archive& ar, JointTrajectoryInfo& obj)
{
  ar(cereal::make_nvp("joint_state", obj.joint_state));
  ar(cereal::make_nvp("joint_trajectory", obj.joint_trajectory));
}

template <class Archive>
void serialize(Archive& ar, JointTrajectorySet& obj)
{
  ar(cereal::make_nvp("initial_state", obj.initial_state_));
  ar(cereal::make_nvp("joint_trajectory", obj.joint_trajectory_));
  ar(cereal::make_nvp("environment", obj.environment_));
  ar(cereal::make_nvp("commands", obj.commands_));
  ar(cereal::make_nvp("description", obj.description_));
  ar(cereal::make_nvp("ns", obj.ns_));
  ar(cereal::make_nvp("uuid", obj.uuid_));
}
}  // namespace tesseract::common

#endif  // TESSERACT_GUI_COMMON_CEREAL_SERIALIZATION_H
