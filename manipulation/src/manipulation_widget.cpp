#include <tesseract_qt/manipulation/manipulation_widget.h>
#include "ui_manipulation_widget.h"

#include <tesseract_qt/scene_graph/models/scene_state_model.h>

#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/events/manipulation_events.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_environment/environment.h>
#include <tesseract_kinematics/core/kinematic_group.h>

#include <QStringList>
#include <QStringListModel>
#include <QApplication>

namespace tesseract_gui
{
struct ManipulationWidget::Implementation
{
  std::shared_ptr<const ComponentInfo> parent_component_info;

  /** @brief If true adding and removing states is disabled */
  bool use_parent_component_info{ false };

  tesseract_kinematics::KinematicGroup::UPtr kin_group;
  QStringList state_names;
  tesseract_environment::Environment::Ptr environment;
  std::unordered_map<std::string, std::unordered_map<std::string, double>> states;
  std::unordered_map<std::string, std::shared_ptr<SceneStateModel>> state_models;

  QStringListModel group_names_model;
  QStringListModel working_frames_model;
  QStringListModel tcp_names_model;
  QStringListModel tcp_offset_names_model;
  QStringListModel state_names_model;
  tesseract_common::TransformMap tcp_offsets;
};

ManipulationWidget::ManipulationWidget(QWidget* parent) : ManipulationWidget(nullptr, false, parent) {}
ManipulationWidget::ManipulationWidget(bool use_parent_component_info, QWidget* parent)
  : ManipulationWidget(nullptr, use_parent_component_info, parent)
{
}

ManipulationWidget::ManipulationWidget(std::shared_ptr<const ComponentInfo> parent_component_info,
                                       bool use_parent_component_info,
                                       QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::ManipulationWidget>()), data_(std::make_unique<Implementation>())
{
  ui->setupUi(this);

  data_->parent_component_info = std::move(parent_component_info);
  data_->use_parent_component_info = use_parent_component_info;

  addStateHelper("default");
  ui->state_widget->setModel(data_->state_models["default"]);

  ui->group_combo_box->setModel(&data_->group_names_model);
  ui->working_frame_combo_box->setModel(&data_->working_frames_model);
  ui->tcp_combo_box->setModel(&data_->tcp_names_model);
  ui->tcp_offset_combo_box->setModel(&data_->tcp_offset_names_model);
  ui->state_combo_box->setModel(&data_->state_names_model);
  ui->state_label->setEnabled(!data_->use_parent_component_info);
  ui->state_combo_box->setEnabled(!data_->use_parent_component_info);

  connect(ui->group_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onGroupNameChanged()));
  connect(ui->working_frame_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onWorkingFrameChanged()));
  connect(ui->tcp_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onTCPNameChanged()));
  connect(ui->tcp_offset_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onTCPOffsetNameChanged()));
  connect(ui->state_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onStateNameChanged()));
  connect(ui->mode_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onModeChanged()));
  connect(ui->reload_push_button, SIGNAL(clicked()), this, SLOT(onReset()));

  connect(ui->joint_state_slider,
          &tesseract_gui::JointStateSliderWidget::jointStateChanged,
          this,
          &tesseract_gui::ManipulationWidget::onJointStateSliderChanged);

  connect(ui->cartesian_widget,
          &tesseract_gui::CartesianEditorWidget::transformChanged,
          this,
          &tesseract_gui::ManipulationWidget::onCartesianTransformChanged);

  ui->tabWidget->setCurrentIndex(0);

  onModeChanged();
}

ManipulationWidget::~ManipulationWidget() = default;

void ManipulationWidget::addState(const std::string& state_name)
{
  if (data_->use_parent_component_info)
    throw std::runtime_error("ManipulationWidget, cannot add states when use_parent_component_info is enabled");

  addStateHelper(state_name);
}

void ManipulationWidget::removeState(const std::string& state_name)
{
  if (data_->use_parent_component_info)
    throw std::runtime_error("ManipulationWidget, cannot remove states when use_parent_component_info is enabled");

  removeStateHelper(state_name);
}

void ManipulationWidget::addStateHelper(const std::string& state_name)
{
  data_->state_names.push_back(state_name.c_str());
  data_->state_names.removeDuplicates();
  data_->states[state_name] = std::unordered_map<std::string, double>{};
  if (data_->use_parent_component_info)
    data_->state_models[state_name] = std::make_shared<SceneStateModel>(data_->parent_component_info);
  else
    data_->state_models[state_name] = std::make_shared<SceneStateModel>(data_->parent_component_info->createChild());

  const bool is_empty = data_->state_names_model.stringList().isEmpty();
  const QString current_state_name =
      (is_empty) ? QString::fromStdString(state_name) : ui->state_combo_box->currentText();
  data_->state_names_model.setStringList(data_->state_names);
  ui->state_combo_box->setCurrentText(current_state_name);

  {  // Update toolbar if one exists
    std::unordered_map<std::string, std::shared_ptr<const ComponentInfo>> state_component_infos;
    for (const auto& it : data_->state_models)
      state_component_infos[it.first] = it.second->getComponentInfo();
    QApplication::sendEvent(qApp,
                            new events::ManipulationChanged(
                                data_->parent_component_info, current_state_name.toStdString(), state_component_infos));
  }

  if (!data_->use_parent_component_info)
  {
    std::shared_ptr<EnvironmentWrapper> env_wrapper = EnvironmentManager::find(data_->parent_component_info);
    if (env_wrapper == nullptr)
      env_wrapper = EnvironmentManager::getDefault();

    if (env_wrapper == nullptr)
      return;

    auto env = env_wrapper->getEnvironment();
    if (env == nullptr || !env->isInitialized())
      return;

    auto it = data_->state_models.find(state_name);
    if (it != data_->state_models.end())
    {
      auto child_env_wrapper =
          std::make_shared<DefaultEnvironmentWrapper>(it->second->getComponentInfo(), env->clone());
      EnvironmentManager::set(child_env_wrapper);
      data_->environment = child_env_wrapper->getEnvironment();
    }
  }
}

void ManipulationWidget::removeStateHelper(const std::string& state_name)
{
  if (!data_->use_parent_component_info)
  {
    auto it = data_->state_models.find(state_name);
    if (it != data_->state_models.end())
      EnvironmentManager::remove(it->second->getComponentInfo());
  }

  const QString current_state_name = ui->state_combo_box->currentText();

  data_->state_names.removeAll(state_name.c_str());
  data_->states.erase(state_name);
  data_->state_models.erase(state_name);

  data_->state_names_model.setStringList(data_->state_names);

  const bool is_empty = data_->state_names_model.stringList().isEmpty();
  if (!is_empty && (current_state_name.toStdString() != state_name))
    ui->state_combo_box->setCurrentText(current_state_name);

  // Update toolbar if one exists
  std::unordered_map<std::string, std::shared_ptr<const ComponentInfo>> state_component_infos;
  for (const auto& it : data_->state_models)
    state_component_infos[it.first] = it.second->getComponentInfo();
  QApplication::sendEvent(qApp,
                          new events::ManipulationChanged(data_->parent_component_info,
                                                          ui->state_combo_box->currentText().toStdString(),
                                                          state_component_infos));
}

void ManipulationWidget::setComponentInfo(std::shared_ptr<const ComponentInfo> component_info)
{
  data_->parent_component_info = std::move(component_info);

  const std::string current_state = ui->state_combo_box->currentText().toStdString();
  const QStringList state_names = data_->state_names;
  for (const auto& state_name : state_names)
  {
    removeStateHelper(state_name.toStdString());
    addStateHelper(state_name.toStdString());
  }

  auto it = data_->state_models.find(current_state);
  if (it != data_->state_models.end())
    ui->state_widget->setModel(it->second);

  onReset();
}

std::shared_ptr<const ComponentInfo> ManipulationWidget::getComponentInfo() const
{
  return data_->parent_component_info;
}

bool ManipulationWidget::isValid() const
{
  std::shared_ptr<EnvironmentWrapper> env_wrapper = EnvironmentManager::find(data_->parent_component_info);
  if (env_wrapper == nullptr)
  {
    env_wrapper = EnvironmentManager::getDefault();
    if (env_wrapper == nullptr)
      return false;
  }

  auto env = env_wrapper->getEnvironment();
  if (env == nullptr || !env->isInitialized())
    return false;

  for (const auto& state : data_->states)
  {
    if (state.second.empty())
      return false;
  }

  return true;
}

int ManipulationWidget::getStateCount() const { return data_->states.size(); }

const tesseract_kinematics::KinematicGroup& ManipulationWidget::kinematicGroup() const { return *data_->kin_group; }

int ManipulationWidget::getMode() const { return ui->mode_combo_box->currentIndex(); }

QString ManipulationWidget::getGroupName() const { return ui->group_combo_box->currentText(); }

QString ManipulationWidget::getWorkingFrame() const { return ui->working_frame_combo_box->currentText(); }

QString ManipulationWidget::getTCPName() const { return ui->tcp_combo_box->currentText(); }

QString ManipulationWidget::getTCPOffsetName() const { return ui->tcp_offset_combo_box->currentText(); }

Eigen::Isometry3d ManipulationWidget::getTCPOffset() const
{
  auto it = data_->tcp_offsets.find(getTCPOffsetName().toStdString());
  if (it == data_->tcp_offsets.end())
    return Eigen::Isometry3d::Identity();

  return it->second;
}

std::string ManipulationWidget::getActiveStateName() const { return ui->state_combo_box->currentText().toStdString(); }

tesseract_scene_graph::SceneState ManipulationWidget::getActiveState() const { return getState(getActiveStateName()); }

void ManipulationWidget::setActiveState(const std::unordered_map<std::string, double>& state)
{
  ui->joint_state_slider->setJointState(state);
}

void ManipulationWidget::setActiveCartesianTransform(const Eigen::Isometry3d& transform)
{
  onCartesianTransformChanged(transform);
  // Update the cartesian transform details
  if (ui->mode_combo_box->currentIndex() == 1)
  {
    ui->cartesian_widget->blockSignals(true);
    ui->cartesian_widget->setTransform(getActiveCartesianTransform());
    ui->cartesian_widget->blockSignals(false);
  }
}

tesseract_scene_graph::SceneState ManipulationWidget::getState(const std::string& state_name) const
{
  return data_->environment->getState(data_->states.at(state_name));
}

void ManipulationWidget::onGroupNameChanged()
{
  std::shared_ptr<EnvironmentWrapper> env_wrapper = EnvironmentManager::find(data_->parent_component_info);
  if (env_wrapper == nullptr)
  {
    env_wrapper = EnvironmentManager::getDefault();
    if (env_wrapper == nullptr)
      return;
  }

  auto env = env_wrapper->getEnvironment();
  if (env == nullptr || !env->isInitialized())
    return;

  std::vector<tesseract_scene_graph::Joint::ConstPtr> joints;
  {  // Scope for lock read
    std::shared_lock<std::shared_mutex> lock = env->lockRead();
    auto group_names = env->getGroupNames();
    auto group_name = ui->group_combo_box->currentText().toStdString();
    auto it = group_names.find(group_name);
    if (it != group_names.end())
    {
      ui->group_combo_box->blockSignals(true);
      ui->working_frame_combo_box->blockSignals(true);
      ui->tcp_combo_box->blockSignals(true);
      ui->tcp_offset_combo_box->blockSignals(true);

      data_->kin_group = env->getKinematicGroup(group_name);

      for (const auto& state_name : data_->state_names)
      {
        QApplication::sendEvent(
            qApp,
            new events::SceneStateChanged(data_->state_models.at(state_name.toStdString())->getComponentInfo(),
                                          env->getState()));
        data_->states[state_name.toStdString()].clear();
      }

      // Update working frames
      std::vector<std::string> working_frames = data_->kin_group->getAllValidWorkingFrames();
      QStringList wf_sl;
      for (const auto& working_frame : working_frames)
        wf_sl.append(working_frame.c_str());
      data_->working_frames_model.setStringList(wf_sl);

      if (!working_frames.empty())
        ui->working_frame_combo_box->setCurrentIndex(0);

      // Update TCP Names
      std::vector<std::string> tcp_names = data_->kin_group->getAllPossibleTipLinkNames();
      QStringList tcp_sl;
      for (const auto& tcp_name : tcp_names)
        tcp_sl.append(tcp_name.c_str());
      data_->tcp_names_model.setStringList(tcp_sl);

      if (!tcp_names.empty())
        ui->tcp_combo_box->setCurrentIndex(0);

      // Update TCP Offset Names
      QStringList tcp_offset_sl;
      tcp_offset_sl.append("None");
      data_->tcp_offsets.clear();
      auto group_tcp_offsets = env->getKinematicsInformation().group_tcps;
      auto it = group_tcp_offsets.find(group_name);
      if (it != group_tcp_offsets.end())
      {
        data_->tcp_offsets = it->second;
        for (const auto& tcp_offset : data_->tcp_offsets)
          tcp_offset_sl.append(tcp_offset.first.c_str());
      }
      data_->tcp_offset_names_model.setStringList(tcp_offset_sl);
      ui->tcp_offset_combo_box->setCurrentIndex(0);

      // Update joint sliders
      std::vector<std::string> joint_names = data_->kin_group->getJointNames();

      joints.reserve(joint_names.size());
      for (const auto& joint_name : joint_names)
        joints.push_back(env->getJoint(joint_name));

      ui->group_combo_box->blockSignals(false);
      ui->working_frame_combo_box->blockSignals(false);
      ui->tcp_combo_box->blockSignals(false);
      ui->tcp_offset_combo_box->blockSignals(false);
    }
  }

  ui->joint_state_slider->setJoints(joints);

  for (const auto& state_name : data_->state_names)
    data_->states[state_name.toStdString()] = ui->joint_state_slider->getJointState();

  if (!joints.empty())
    ui->cartesian_widget->setTransform(getActiveCartesianTransform());

  onJointStateSliderChanged(ui->joint_state_slider->getJointState());

  Q_EMIT groupNameChanged(ui->group_combo_box->currentText());
}

void ManipulationWidget::onModeChanged()
{
  if (ui->mode_combo_box->currentIndex() == 0)
  {
    ui->joint_tab->setEnabled(true);
    ui->cartesian_tab->setEnabled(false);
  }
  else
  {
    ui->joint_tab->setEnabled(false);
    ui->cartesian_tab->setEnabled(true);
  }

  Q_EMIT modeChanged(ui->mode_combo_box->currentIndex());
}

void ManipulationWidget::onWorkingFrameChanged()
{
  ui->cartesian_widget->setTransform(getActiveCartesianTransform());
  Q_EMIT workingFrameChanged(ui->working_frame_combo_box->currentText());
}

void ManipulationWidget::onTCPNameChanged()
{
  ui->cartesian_widget->setTransform(getActiveCartesianTransform());
  Q_EMIT tcpNameChanged(ui->tcp_combo_box->currentText());
}

void ManipulationWidget::onTCPOffsetNameChanged()
{
  ui->cartesian_widget->setTransform(getActiveCartesianTransform());
  Q_EMIT tcpOffsetNameChanged(ui->tcp_offset_combo_box->currentText());
}

void ManipulationWidget::onStateNameChanged()
{
  const std::string current_state_name = ui->state_combo_box->currentText().toStdString();

  auto it = data_->states.find(current_state_name);
  if (it != data_->states.end())
    ui->joint_state_slider->setJointState(it->second);

  auto it2 = data_->state_models.find(current_state_name);
  if (it2 != data_->state_models.end())
    ui->state_widget->setModel(it2->second);
}

void ManipulationWidget::onJointStateSliderChanged(std::unordered_map<std::string, double> state)
{
  if (isValid())
  {
    const std::string state_name = ui->state_combo_box->currentText().toStdString();
    data_->states[state_name] = state;
    data_->environment->setState(state);
    tesseract_scene_graph::SceneState scene_state = data_->environment->getState();
    tesseract_scene_graph::SceneState reduced_scene_state = getReducedSceneState(scene_state);

    // Update the cartesian transform details
    if (ui->mode_combo_box->currentIndex() == 0)
      ui->cartesian_widget->setTransform(getActiveCartesianTransform());

    Q_EMIT manipulationStateChanged(reduced_scene_state, state_name);
  }
}

tesseract_scene_graph::SceneState
ManipulationWidget::getReducedSceneState(const tesseract_scene_graph::SceneState& scene_state)
{
  tesseract_scene_graph::SceneState reduced_scene_state;
  for (const auto& link_name : data_->kin_group->getActiveLinkNames())
    reduced_scene_state.link_transforms[link_name] = scene_state.link_transforms.at(link_name);

  for (const auto& joint_name : data_->kin_group->getJointNames())
  {
    reduced_scene_state.joints[joint_name] = scene_state.joints.at(joint_name);
    reduced_scene_state.joint_transforms[joint_name] = scene_state.joint_transforms.at(joint_name);
  }
  return reduced_scene_state;
}

Eigen::Isometry3d ManipulationWidget::getActiveCartesianTransform(bool in_world) const
{
  std::string working_frame = getWorkingFrame().toStdString();
  std::string tcp_name = getTCPName().toStdString();
  Eigen::Isometry3d tcp_offset = getTCPOffset();
  Eigen::VectorXd jv = getActiveJointValues();
  tesseract_common::TransformMap tf = data_->kin_group->calcFwdKin(jv);
  if (in_world)
    return tf.at(tcp_name) * tcp_offset;

  return tf.at(working_frame).inverse() * tf.at(tcp_name) * tcp_offset;
}

std::vector<std::string> ManipulationWidget::getActiveJointNames() const { return data_->kin_group->getJointNames(); }

Eigen::VectorXd ManipulationWidget::getActiveJointValues() const
{
  Eigen::VectorXd jv{ Eigen::VectorXd::Zero(data_->kin_group->numJoints()) };
  std::vector<std::string> joint_names = data_->kin_group->getJointNames();
  Eigen::Index idx{ 0 };
  const std::string active_state_name = getActiveStateName();
  for (const auto& jn : joint_names)
  {
    auto j_it = data_->states[active_state_name].find(jn);
    if (j_it != data_->states[active_state_name].end())
      jv(idx) = j_it->second;

    ++idx;
  }
  return jv;
}

void ManipulationWidget::onCartesianTransformChanged(const Eigen::Isometry3d& transform)
{
  if (data_->kin_group != nullptr && ui->mode_combo_box->currentIndex() == 1)
  {
    std::string working_frame = getWorkingFrame().toStdString();
    std::string tcp_name = getTCPName().toStdString();
    Eigen::Isometry3d tcp_offset = getTCPOffset();
    Eigen::Isometry3d target = transform * tcp_offset.inverse();
    std::vector<std::string> joint_names = data_->kin_group->getJointNames();

    std::vector<std::string> tcp_names = data_->kin_group->getAllPossibleTipLinkNames();
    auto it = std::find(tcp_names.begin(), tcp_names.end(), tcp_name);
    if (it != tcp_names.end())
    {
      tesseract_kinematics::KinGroupIKInput inputs(target, working_frame, tcp_name);
      Eigen::VectorXd seed = getActiveJointValues();
      tesseract_kinematics::IKSolutions solutions = data_->kin_group->calcInvKin(inputs, seed);
      if (!solutions.empty())
      {
        // get the closest solution to the seed
        double dist = std::numeric_limits<double>::max();
        Eigen::VectorXd temp_seed = seed;
        for (const auto& solution : solutions)
        {
          double d = (solution - seed).norm();
          if (d < dist)
          {
            temp_seed = solution;
            dist = d;
          }
        }

        if (!tesseract_common::satisfiesPositionLimits<double>(temp_seed, data_->kin_group->getLimits().joint_limits))
          temp_seed = seed;

        std::unordered_map<std::string, double> state;
        for (int i = 0; i < joint_names.size(); ++i)
          state[joint_names[i]] = temp_seed[i];

        ui->joint_state_slider->setJointState(state);
      }
    }
  }
}

void ManipulationWidget::onReset()
{
  QString current_group_name = ui->group_combo_box->currentText();

  std::shared_ptr<EnvironmentWrapper> env_wrapper = EnvironmentManager::find(data_->parent_component_info);
  if (env_wrapper == nullptr)
    env_wrapper = EnvironmentManager::getDefault();

  if (env_wrapper == nullptr)
  {
    data_->group_names_model.setStringList(QStringList());
    return;
  }

  auto env = env_wrapper->getEnvironment();
  if (env == nullptr || !env->isInitialized())
  {
    data_->group_names_model.setStringList(QStringList());
    return;
  }

  for (const auto& state_name : data_->state_names)
  {
    QApplication::sendEvent(qApp,
                            new events::SceneStateChanged(
                                data_->state_models.at(state_name.toStdString())->getComponentInfo(), env->getState()));
    data_->states[state_name.toStdString()].clear();
  }

  if (!data_->use_parent_component_info)
  {
    for (const auto& state_name : data_->state_names)
    {
      auto child_env_wrapper = std::make_shared<DefaultEnvironmentWrapper>(
          data_->state_models.at(state_name.toStdString())->getComponentInfo(), env->clone());
      EnvironmentManager::set(child_env_wrapper);
    }
  }
  else
  {
    data_->environment = env;
  }

  QStringList group_names;
  std::vector<std::string> gn_v;
  {  // Scope the lock
    auto lock = env->lockRead();

    auto gn = env->getGroupNames();
    gn_v.insert(gn_v.begin(), gn.begin(), gn.end());

    for (const auto& group_name : env->getGroupNames())
    {
      tesseract_kinematics::KinematicGroup::UPtr kin_group;
      try
      {
        kin_group = env->getKinematicGroup(group_name);
      }
      catch (...)
      {
        CONSOLE_BRIDGE_logDebug("ManipulationWidget, Group '%s' is not supported!", group_name.c_str());
      }

      if (kin_group != nullptr)
        group_names.append(group_name.c_str());
    }
  }

  ui->group_combo_box->blockSignals(true);
  data_->group_names_model.setStringList(group_names);
  ui->group_combo_box->blockSignals(false);

  auto it = std::find(gn_v.begin(), gn_v.end(), current_group_name.toStdString());
  if (it != gn_v.end())
    ui->group_combo_box->setCurrentText(current_group_name);
  else
    ui->group_combo_box->setCurrentText("");

  onGroupNameChanged();

  // Update toolbar if one exists
  std::unordered_map<std::string, std::shared_ptr<const ComponentInfo>> state_component_infos;
  for (const auto& it : data_->state_models)
    state_component_infos[it.first] = it.second->getComponentInfo();
  QApplication::sendEvent(qApp,
                          new events::ManipulationChanged(data_->parent_component_info,
                                                          ui->state_combo_box->currentText().toStdString(),
                                                          state_component_infos));
}

}  // namespace tesseract_gui
