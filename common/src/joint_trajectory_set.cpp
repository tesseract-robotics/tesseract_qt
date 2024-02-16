/**
 * @file joint_trajectory_set.cpp
 * @brief Tesseract Joint Trajectory Set
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

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_common/utils.h>
#include <tesseract_common/serialization.h>
//#include <tesseract_common/joint_trajectory_set.h>
#include <tesseract_qt/common/joint_trajectory_set.h>
#include <tesseract_environment/commands.h>

namespace tesseract_common
{
template <class Archive>
void JointTrajectoryInfo::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& BOOST_SERIALIZATION_NVP(joint_state);
  ar& BOOST_SERIALIZATION_NVP(joint_trajectory);
  ar& BOOST_SERIALIZATION_NVP(description);
}

JointTrajectorySet::JointTrajectorySet(const std::unordered_map<std::string, double>& initial_state,
                                       std::string description)
  : description_(std::move(description)), uuid_(boost::uuids::random_generator()())
{
  initial_state_.joint_names.reserve(initial_state.size());
  initial_state_.position.resize(static_cast<Eigen::Index>(initial_state.size()));
  initial_state_.velocity.resize(static_cast<Eigen::Index>(initial_state.size()));
  initial_state_.acceleration.resize(static_cast<Eigen::Index>(initial_state.size()));
  initial_state_.effort.resize(static_cast<Eigen::Index>(initial_state.size()));
  for (const auto& joint : initial_state)
  {
    initial_state_.joint_names.push_back(joint.first);

    Eigen::Index r = static_cast<Eigen::Index>(initial_state_.joint_names.size()) - 1;
    initial_state_.position(r) = joint.second;
    initial_state_.velocity(r) = 0;
    initial_state_.acceleration(r) = 0;
    initial_state_.effort(r) = 0;
  }
}

JointTrajectorySet::JointTrajectorySet(const std::unordered_map<std::string, double>& initial_state,
                                       tesseract_environment::Commands commands,
                                       std::string description)
  : JointTrajectorySet(initial_state, description)
{
  commands_ = std::move(commands);
}

JointTrajectorySet::JointTrajectorySet(tesseract_environment::Environment::UPtr environment, std::string description)
  : JointTrajectorySet(environment->getState().joints, std::move(description))
{
  environment_ = std::move(environment);
}

boost::uuids::uuid JointTrajectorySet::getUUID() const { return uuid_; }

void JointTrajectorySet::regenerateUUID() { uuid_ = boost::uuids::random_generator()(); }

void JointTrajectorySet::applyEnvironment(tesseract_environment::Environment::UPtr env)
{
  if (environment_ != nullptr)
    throw std::runtime_error("JointTrajectorySet: Cannot apply environment to trajectory set which already contains an "
                             "environment");

  environment_ = std::move(env);
  if (!environment_->applyCommands(commands_))
    throw std::runtime_error("JointTrajectorySet: Failed to apply commands to provided environment");

  commands_.clear();
}

tesseract_environment::Environment::Ptr JointTrajectorySet::getEnvironment() const { return environment_; }

const tesseract_environment::Commands& JointTrajectorySet::getEnvironmentCommands() const { return commands_; }

JointState JointTrajectorySet::getNewTrajectoryInitialState() const
{
  JointState prev_state = initial_state_;
  if (!joint_trajectory_.empty() && !joint_trajectory_.back().joint_trajectory.empty())
  {
    const JointTrajectoryInfo& info = joint_trajectory_.back();
    const JointState& last_state = info.joint_trajectory.back();
    prev_state = info.joint_state;
    prev_state.time = last_state.time;
    for (Eigen::Index i = 0; i < last_state.joint_names.size(); ++i)
    {
      auto it = std::find(prev_state.joint_names.begin(), prev_state.joint_names.end(), last_state.joint_names[i]);
      Eigen::Index idx = std::distance(prev_state.joint_names.begin(), it);
      prev_state.position(idx) = last_state.position(i);
      prev_state.velocity(idx) = last_state.velocity(i);
      prev_state.acceleration(idx) = last_state.acceleration(i);
      prev_state.effort(idx) = last_state.effort(i);
    }
  }
  return prev_state;
}

void JointTrajectorySet::appendJointTrajectory(const JointTrajectory& joint_trajectory)
{
  JointTrajectoryInfo traj_info;
  traj_info.joint_state = getNewTrajectoryInitialState();
  traj_info.joint_trajectory.reserve(joint_trajectory.size());
  traj_info.description = joint_trajectory.description;

  std::set<std::string> prune_joint_names;
  double last_time = traj_info.joint_state.time;
  double current_time = 0;
  for (const auto& joint_state : joint_trajectory)
  {
    prune_joint_names.insert(joint_state.joint_names.begin(), joint_state.joint_names.end());

    tesseract_common::JointState adj_state(joint_state);
    current_time = adj_state.time;

    // It is possible for sub composites to start back from zero, this accounts for it
    if (current_time < last_time)
      last_time = 0;

    double dt = current_time - last_time;
    adj_state.time = dt;
    last_time = current_time;
    appendJointState(traj_info, adj_state);
  }

  // Next, for this trajectory prune only joints that changed because the initial state captures those that did not.
  std::vector<Eigen::Index> prune_indices;
  std::vector<std::string> prune_names;
  prune_indices.reserve(prune_joint_names.size());
  prune_names.reserve(prune_joint_names.size());
  for (const auto& joint_name : prune_joint_names)
  {
    prune_names.push_back(joint_name);

    auto it = std::find(traj_info.joint_state.joint_names.begin(), traj_info.joint_state.joint_names.end(), joint_name);
    prune_indices.push_back(std::distance(traj_info.joint_state.joint_names.begin(), it));
  }

  for (JointState& js : traj_info.joint_trajectory)
  {
    JointState pruned_state;
    pruned_state.joint_names = prune_names;
    pruned_state.position = Eigen::VectorXd::Zero(static_cast<Eigen::Index>(prune_names.size()));
    pruned_state.velocity = Eigen::VectorXd::Zero(static_cast<Eigen::Index>(prune_names.size()));
    pruned_state.acceleration = Eigen::VectorXd::Zero(static_cast<Eigen::Index>(prune_names.size()));
    pruned_state.effort = Eigen::VectorXd::Zero(static_cast<Eigen::Index>(prune_names.size()));

    pruned_state.time = js.time;
    for (std::size_t i = 0; i < prune_indices.size(); ++i)
    {
      pruned_state.position(static_cast<Eigen::Index>(i)) = js.position(prune_indices[i]);
      pruned_state.velocity(static_cast<Eigen::Index>(i)) = js.velocity(prune_indices[i]);
      pruned_state.acceleration(static_cast<Eigen::Index>(i)) = js.acceleration(prune_indices[i]);
      pruned_state.effort(static_cast<Eigen::Index>(i)) = js.effort(prune_indices[i]);
    }

    js = pruned_state;
  }
  joint_trajectory_.push_back(traj_info);
}

void JointTrajectorySet::appendJointState(JointTrajectoryInfo& traj_info, const JointState& joint_state)
{
  JointState& prev_state = traj_info.joint_state;
  if (!traj_info.joint_trajectory.empty())
    prev_state = traj_info.joint_trajectory.back();

  JointState full_state(prev_state);
  // Only position should be copied over, zero everything else
  full_state.velocity = Eigen::VectorXd::Zero(full_state.position.rows());
  full_state.acceleration = Eigen::VectorXd::Zero(full_state.position.rows());
  full_state.effort = Eigen::VectorXd::Zero(full_state.position.rows());

  for (std::size_t i = 0; i < joint_state.joint_names.size(); ++i)
  {
    auto it =
        std::find(initial_state_.joint_names.begin(), initial_state_.joint_names.end(), joint_state.joint_names[i]);
    long index = std::distance(initial_state_.joint_names.begin(), it);

    // update position
    full_state.position(index) = joint_state.position(static_cast<Eigen::Index>(i));

    // update velocity
    if (joint_state.velocity.rows() == joint_state.position.rows())
      full_state.velocity(index) = joint_state.velocity(static_cast<Eigen::Index>(i));

    // update acceleration
    if (joint_state.acceleration.rows() == joint_state.position.rows())
      full_state.acceleration(index) = joint_state.acceleration(static_cast<Eigen::Index>(i));

    // update effort
    if (joint_state.effort.rows() == joint_state.position.rows())
      full_state.effort(index) = joint_state.effort(static_cast<Eigen::Index>(i));
  }
  full_state.time = prev_state.time + joint_state.time;

  traj_info.joint_trajectory.push_back(full_state);
}

const std::vector<JointTrajectoryInfo>& JointTrajectorySet::getJointTrajectories() const { return joint_trajectory_; }

void JointTrajectorySet::setDescription(std::string description) { description_ = std::move(description); }

const std::string& JointTrajectorySet::getDescription() const { return description_; }

void JointTrajectorySet::setNamespace(std::string ns) { ns_ = std::move(ns); }

const std::string& JointTrajectorySet::getNamespace() const { return ns_; }

std::size_t JointTrajectorySet::size() const { return joint_trajectory_.size(); }

std::vector<JointTrajectoryInfo>::reference JointTrajectorySet::operator[](std::size_t pos)
{
  return joint_trajectory_[pos];
}
std::vector<JointTrajectoryInfo>::const_reference JointTrajectorySet::operator[](std::size_t pos) const
{
  return joint_trajectory_[pos];
}

const JointState& JointTrajectorySet::getInitialState() const { return initial_state_; }

template <class Archive>
void JointTrajectorySet::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& BOOST_SERIALIZATION_NVP(initial_state_);
  ar& BOOST_SERIALIZATION_NVP(joint_trajectory_);
  ar& BOOST_SERIALIZATION_NVP(environment_);
  ar& BOOST_SERIALIZATION_NVP(commands_);
  ar& BOOST_SERIALIZATION_NVP(description_);
  ar& BOOST_SERIALIZATION_NVP(ns_);
  ar& BOOST_SERIALIZATION_NVP(uuid_);
}

}  // namespace tesseract_common

#include <tesseract_common/serialization.h>
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_common::JointTrajectoryInfo)
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_common::JointTrajectorySet)
