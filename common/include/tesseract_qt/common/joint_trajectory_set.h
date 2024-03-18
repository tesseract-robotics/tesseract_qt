/**
 * @file joint_trajectory_set.h
 * @brief Tesseract Joint Trajectory Trajectory Set
 *
 * @author Levi Armstrong
 * @date December 5, 2021
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2021, Southwest Research Institute
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
#ifndef TESSERACT_COMMON_JOINT_TRAJECTORY_SET_H
#define TESSERACT_COMMON_JOINT_TRAJECTORY_SET_H

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <boost/serialization/access.hpp>
#include <boost/uuid/uuid.hpp>
#include <tesseract_common/fwd.h>
#include <tesseract_environment/fwd.h>
#include <tesseract_common/joint_state.h>

namespace tesseract_common
{
struct JointTrajectoryInfo
{
  JointState joint_state;
  JointTrajectory joint_trajectory;
  std::string description;

  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version);  // NOLINT
};

class JointTrajectorySet
{
public:
  /** @brief This is only provided for serialization, DO NOT USE */
  JointTrajectorySet() = default;

  /**
   * @brief Create a trajectory set with initial state
   * @param initial_state The initial state of the environment
   * @param description (Optional) A description of the trajectory set
   */
  JointTrajectorySet(const std::unordered_map<std::string, double>& initial_state, std::string description = "");

  /**
   * @brief Create a trajectory set with initial state
   * @param initial_state The initial state of the environment
   * @param environment The environment to use for planning
   */
  JointTrajectorySet(std::unique_ptr<tesseract_environment::Environment> environment, std::string description = "");

  /**
   * @brief Create a trajectory set with initial state and environment commands
   * @param initial_state The initial state of the environment
   * @param commands Additional Commands to be applied to environment prior to planning
   */
  JointTrajectorySet(const std::unordered_map<std::string, double>& initial_state,
                     std::vector<std::shared_ptr<const tesseract_environment::Command>> commands,
                     std::string description = "");

  /** @brief Get the uuid */
  boost::uuids::uuid getUUID() const;

  /** @brief Regenerate uuid */
  void regenerateUUID();

  /**
   * @brief Append a Joint Trajectory
   * @brief This will adjust the time because each trajectory most likely starts from zero
   * @param key The key to store the trajectory under
   * @param joint_trajectory The joint trajectory to append
   */
  void appendJointTrajectory(const JointTrajectory& joint_trajectory);

  /**
   * @brief This is to be used if the trajectory only contains commands but no environment.
   * @details This apply the stored commands to the provided and store the environment and clear the commands
   * @throws If the trajectory set contains an environment it will throw an exception
   * @throws If unable to apply stored commands to provided environment
   * @param env The environment to apply internal command to
   */
  void applyEnvironment(std::unique_ptr<tesseract_environment::Environment> env);

  /**
   * @brief Get the environment for the joint trajectory set
   * @details This can be a nullptr. If nullptr then check for commands which should be applied to the existing
   * environment.
   * @return The environment
   */
  std::shared_ptr<tesseract_environment::Environment> getEnvironment() const;

  /**
   * @brief Get the environment commands that were applied to existing environment for the joint trajectory set
   * @return The environment commands
   */
  const std::vector<std::shared_ptr<const tesseract_environment::Command>>& getEnvironmentCommands() const;

  /**
   * @brief Get the initial state
   * @return The initial state
   */
  const JointState& getInitialState() const;

  /**
   * @brief Get the scene trajectory
   * @return The trajectory
   */
  const std::vector<JointTrajectoryInfo>& getJointTrajectories() const;

  /**
   * @brief Set a description for the trajectory set
   * @param description A description of the trajectory set
   */
  void setDescription(std::string description);

  /**
   * @brief Get the description for the trajectory set
   * @return The description
   */
  const std::string& getDescription() const;

  /**
   * @brief Set a namespace for the trajectory set
   * @details This can be multiple namespace by separating by '::' for example 'main::left::place'
   * which will have it show up in the widget as a teared item in the tree view.
   * @param ns A namespace the trajectory is associated with
   */
  void setNamespace(std::string ns);

  /**
   * @brief Get the namespace for the trajectory set
   * @return The namespace
   */
  const std::string& getNamespace() const;

  /**
   * @brief Get the size of the trajectory set
   * @return The size
   */
  std::size_t size() const;

  /** @brief access specified element */
  std::vector<JointTrajectoryInfo>::reference operator[](std::size_t pos);
  std::vector<JointTrajectoryInfo>::const_reference operator[](std::size_t pos) const;

private:
  /** @brief The initial state of the environment */
  JointState initial_state_;

  /** @brief A vector of joint trajectory in the set */
  std::vector<JointTrajectoryInfo> joint_trajectory_;

  /** @brief (Optional) Override the existing Tesseract Environment */
  std::shared_ptr<tesseract_environment::Environment> environment_{ nullptr };

  /** @brief (Optional) Additional Commands to be applied to environment prior to trajectory visualization */
  std::vector<std::shared_ptr<const tesseract_environment::Command>> commands_;

  /** @brief A description of the trajectory set */
  std::string description_;

  /** @brief The namespace associated with the trajectory set */
  std::string ns_{ "general" };

  /** @brief The trajectory set uuid */
  boost::uuids::uuid uuid_{};

  /**
   * @brief Append a joint state to the end
   * @details It assumes the state time is a delta time which gets add to the current trajectory time.
   * @param joint_state The joint state to append
   */
  void appendJointState(JointTrajectoryInfo& traj_info, const JointState& joint_state);

  JointState getNewTrajectoryInitialState() const;

  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version);  // NOLINT
};
}  // namespace tesseract_common
#endif  // TESSERACT_COMMON_JOINT_TRAJECTORY_SET_H
