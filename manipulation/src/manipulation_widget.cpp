#include <tesseract_qt/manipulation/manipulation_widget.h>
#include "ui_manipulation_widget.h"

#include <tesseract_qt/scene_graph/models/scene_state_model.h>

#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_environment/environment.h>
#include <tesseract_kinematics/core/kinematic_group.h>

#include <QStringListModel>
#include <QApplication>

namespace tesseract_gui
{
struct ManipulationWidget::Implementation
{
  ComponentInfo parent_component_info;

  bool single_state;
  tesseract_kinematics::KinematicGroup::UPtr kin_group;
  std::vector<tesseract_scene_graph::StateSolver::UPtr> state_solvers;
  std::vector<std::unordered_map<std::string, double>> states;

  QStringListModel group_names_model;
  QStringListModel working_frames_model;
  QStringListModel tcp_names_model;
  QStringListModel tcp_offset_names_model;
  tesseract_common::TransformMap tcp_offsets;

  std::vector<std::shared_ptr<SceneStateModel>> state_models;

  //  // Toolbar
  //  QToolBar* toolbar{ nullptr };
  //  QAction* reset_action{ nullptr };
};

ManipulationWidget::ManipulationWidget(bool single_state, QWidget* parent)
  : ManipulationWidget(ComponentInfo(), single_state, parent)
{
}

ManipulationWidget::ManipulationWidget(ComponentInfo parent_component_info, bool single_state, QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::ManipulationWidget>()), data_(std::make_unique<Implementation>())
{
  ui->setupUi(this);

  data_->parent_component_info = std::move(parent_component_info);
  data_->single_state = single_state;
  data_->state_solvers.resize((single_state) ? 1 : 2);
  data_->states.resize((single_state) ? 1 : 2);
  data_->state_models.resize((single_state) ? 1 : 2);

  if (single_state)
  {
    data_->state_models[0] = std::make_shared<SceneStateModel>(data_->parent_component_info.createChild());
    ui->state_widget->setModel(data_->state_models[0]);
  }
  else
  {
    throw std::runtime_error("ManipulationWidget, only single state is currently supported");
  }

  ui->group_combo_box->setModel(&data_->group_names_model);
  ui->working_frame_combo_box->setModel(&data_->working_frames_model);
  ui->tcp_combo_box->setModel(&data_->tcp_names_model);
  ui->tcp_offset_combo_box->setModel(&data_->tcp_offset_names_model);

  connect(ui->group_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onGroupNameChanged()));
  connect(ui->working_frame_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onWorkingFrameChanged()));
  connect(ui->tcp_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onTCPNameChanged()));
  connect(ui->tcp_offset_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onTCPOffsetNameChanged()));
  connect(ui->state_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onManipulatorTypeChanged()));
  connect(ui->mode_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onModeChanged()));

  connect(ui->joint_state_slider,
          &tesseract_gui::JointStateSliderWidget::jointStateChanged,
          this,
          &tesseract_gui::ManipulationWidget::onJointStateSliderChanged);

  connect(ui->cartesian_widget,
          &tesseract_gui::CartesianEditorWidget::transformChanged,
          this,
          &tesseract_gui::ManipulationWidget::onCartesianTransformChanged);

  if (data_->single_state)
  {
    ui->state_label->hide();
    ui->state_combo_box->hide();
  }

  ui->tabWidget->setCurrentIndex(0);

  onModeChanged();
}

ManipulationWidget::~ManipulationWidget() = default;

void ManipulationWidget::setComponentInfo(ComponentInfo component_info)
{
  data_->parent_component_info = std::move(component_info);
  if (data_->single_state)
  {
    data_->state_models[0] = std::make_shared<SceneStateModel>(data_->parent_component_info.createChild());
    ui->state_widget->setModel(data_->state_models[0]);
  }
  else
  {
    throw std::runtime_error("ManipulationWidget, only single state is currently supported");
  }

  onReset();
}

const ComponentInfo& ManipulationWidget::getComponentInfo() const { return data_->parent_component_info; }

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

  for (const auto& state_solver : data_->state_solvers)
  {
    if (state_solver == nullptr)
      return false;
  }

  for (const auto& state : data_->states)
  {
    if (state.empty())
      return false;
  }

  return true;
}

int ManipulationWidget::getStateCount() const { return (data_->single_state) ? 1 : 2; }

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

int ManipulationWidget::getActiveStateIndex() const { return ui->state_combo_box->currentIndex(); }

tesseract_scene_graph::SceneState ManipulationWidget::getActiveState() const { return getState(getActiveStateIndex()); }

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

tesseract_scene_graph::SceneState ManipulationWidget::getState(int index) const
{
  return data_->state_solvers.at(index)->getState(data_->states.at(index));
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

  auto lock = env->lockRead();
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
    for (std::size_t i = 0; i < getStateCount(); ++i)
    {
      data_->state_solvers[i] = env->getStateSolver();
      QApplication::sendEvent(qApp,
                              new events::SceneStateChanged(data_->state_models[0]->getComponentInfo(),
                                                            tesseract_scene_graph::SceneState()));
      data_->states[i].clear();
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

    ui->group_combo_box->blockSignals(false);
    ui->working_frame_combo_box->blockSignals(false);
    ui->tcp_combo_box->blockSignals(false);
    ui->tcp_offset_combo_box->blockSignals(false);

    // Update joint sliders
    std::vector<std::string> joint_names = data_->kin_group->getJointNames();
    std::vector<tesseract_scene_graph::Joint::ConstPtr> joints;
    joints.reserve(joint_names.size());
    for (const auto& joint_name : joint_names)
      joints.push_back(env->getJoint(joint_name));

    ui->joint_state_slider->setJoints(joints);

    for (std::size_t i = 0; i < getStateCount(); ++i)
      data_->states[i] = ui->joint_state_slider->getJointState();

    ui->cartesian_widget->setTransform(getActiveCartesianTransform());
  }
  else
  {
    ui->joint_state_slider->setJoints(std::vector<tesseract_scene_graph::Joint::ConstPtr>());
  }

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

void ManipulationWidget::onManipulatorTypeChanged()
{
  ui->joint_state_slider->setJointState(data_->states[ui->state_combo_box->currentIndex()]);
}

void ManipulationWidget::onJointStateSliderChanged(std::unordered_map<std::string, double> state)
{
  if (isValid())
  {
    data_->state_solvers[ui->state_combo_box->currentIndex()]->setState(state);
    data_->states[ui->state_combo_box->currentIndex()] = state;
    tesseract_scene_graph::SceneState scene_state =
        data_->state_solvers[ui->state_combo_box->currentIndex()]->getState();
    tesseract_scene_graph::SceneState reduced_scene_state = getReducedSceneState(scene_state);
    QApplication::sendEvent(
        qApp,
        new events::SceneStateChanged(data_->state_models[ui->state_combo_box->currentIndex()]->getComponentInfo(),
                                      reduced_scene_state));

    // Update the cartesian transform details
    if (ui->mode_combo_box->currentIndex() == 0)
      ui->cartesian_widget->setTransform(getActiveCartesianTransform());

    Q_EMIT manipulationStateChanged(reduced_scene_state, ui->state_combo_box->currentIndex());
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
  for (const auto& jn : joint_names)
  {
    auto j_it = data_->states[getActiveStateIndex()].find(jn);
    if (j_it != data_->states[getActiveStateIndex()].end())
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
  for (std::size_t i = 0; i < getStateCount(); ++i)
  {
    data_->state_solvers[i] = nullptr;
    QApplication::sendEvent(
        qApp,
        new events::SceneStateChanged(data_->state_models[i]->getComponentInfo(), tesseract_scene_graph::SceneState()));
    data_->states[i].clear();
  }

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

  auto child_env_wrapper =
      std::make_shared<DefaultEnvironmentWrapper>(data_->state_models[0]->getComponentInfo(), env->clone());
  EnvironmentManager::set(child_env_wrapper);

  auto lock = env->lockRead();

  auto gn = env->getGroupNames();
  std::vector<std::string> gn_v(gn.begin(), gn.end());

  QStringList group_names;
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

  ui->group_combo_box->blockSignals(true);
  data_->group_names_model.setStringList(group_names);
  ui->group_combo_box->blockSignals(false);

  auto it = std::find(gn_v.begin(), gn_v.end(), current_group_name.toStdString());
  if (it != gn_v.end())
    ui->group_combo_box->setCurrentText(current_group_name);
  else
    ui->group_combo_box->setCurrentText("");

  onGroupNameChanged();
}

// void ManipulationWidget::addToolBar()
//{
//  data_->toolbar = new QToolBar;  // NOLINT
//  data_->show_all_links_action =
//      data_->toolbar->addAction(icons::getShowAllLinksIcon(), "Show All Links", this, SLOT(onShowAllLinks()));
//  data_->hide_all_links_action =
//      data_->toolbar->addAction(icons::getHideAllLinksIcon(), "Hide All Links", this, SLOT(onHideAllLinks()));
//  data_->toolbar->addSeparator();
//  data_->show_visual_all_links_action = data_->toolbar->addAction(
//      icons::getShowVisualAllLinksIcon(), "Show Visual All Links", this, SLOT(onShowVisualAllLinks()));
//  data_->hide_visual_all_links_action = data_->toolbar->addAction(
//      icons::getHideVisualAllLinksIcon(), "Hide Visual All Links", this, SLOT(onHideVisualAllLinks()));
//  data_->toolbar->addSeparator();
//  data_->show_collision_all_links_action = data_->toolbar->addAction(
//      icons::getShowCollisionAllLinksIcon(), "Show Collision All Links", this, SLOT(onShowCollisionAllLinks()));
//  data_->hide_collision_all_links_action = data_->toolbar->addAction(
//      icons::getHideCollisionAllLinksIcon(), "Hide Collision All Links", this, SLOT(onHideCollisionAllLinks()));
//  data_->toolbar->addSeparator();
//  data_->select_all_links_action =
//      data_->toolbar->addAction(icons::getSelectAllLinksIcon(), "Select All Links", this, SLOT(onSelectAllLinks()));
//  data_->deselect_all_links_action = data_->toolbar->addAction(
//      icons::getDeselectAllLinksIcon(), "Deselect All Links", this, SLOT(onDeselectAllLinks()));
//  data_->toolbar->addSeparator();
//  data_->show_axis_all_links_action = data_->toolbar->addAction(
//      icons::getShowAxisAllLinksIcon(), "Show Axis All Links", this, SLOT(onShowAxisAllLinks()));
//  data_->hide_axis_all_links_action = data_->toolbar->addAction(
//      icons::getHideAxisAllLinksIcon(), "Hide Axis All Links", this, SLOT(onHideAxisAllLinks()));
//  data_->toolbar->addSeparator();
//  data_->reset_action = data_->toolbar->addAction(icons::getRestoreIcon(), "Reload Environment", this,
//  SLOT(onReset()));

//  ui->vertical_layout->insertWidget(0, data_->toolbar);
//}
}  // namespace tesseract_gui
