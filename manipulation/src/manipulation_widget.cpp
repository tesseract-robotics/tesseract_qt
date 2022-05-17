#include <tesseract_qt/manipulation/manipulation_widget.h>
#include "ui_manipulation_widget.h"

#include <tesseract_qt/scene_graph/scene_state_model.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/link_visibility_properties.h>

#include <tesseract_environment/environment.h>
#include <tesseract_kinematics/core/kinematic_group.h>

#include <QStringListModel>
#include <QToolBar>

namespace tesseract_gui
{
struct ManipulationWidgetImpl
{
  ManipulationWidgetImpl(bool single_state) : single_state(single_state) {}

  bool single_state;
  tesseract_environment::Environment::ConstPtr env;
  tesseract_kinematics::KinematicGroup::UPtr kin_group;
  std::array<tesseract_scene_graph::StateSolver::UPtr, 2> state_solvers;
  std::array<std::unordered_map<std::string, double>, 2> states;

  QTreeView* end_state_tree_view;

  QStringListModel group_names_model;
  QStringListModel tcp_names_model;
  std::array<SceneStateModel, 2> state_models;

  LinkVisibilityPropertiesMap link_visibility_properties;

  // Toolbar
  QToolBar* toolbar{ nullptr };
  QAction* show_all_links_action{ nullptr };
  QAction* hide_all_links_action{ nullptr };

  QAction* show_visual_all_links_action{ nullptr };
  QAction* hide_visual_all_links_action{ nullptr };

  QAction* show_collision_all_links_action{ nullptr };
  QAction* hide_collision_all_links_action{ nullptr };

  QAction* select_all_links_action{ nullptr };
  QAction* deselect_all_links_action{ nullptr };

  QAction* show_axis_all_links_action{ nullptr };
  QAction* hide_axis_all_links_action{ nullptr };

  QAction* reset_action{ nullptr };
};

ManipulationWidget::ManipulationWidget(bool single_state, QWidget* parent)
  : QWidget(parent)
  , ui(std::make_unique<Ui::ManipulationWidget>())
  , data_(std::make_unique<ManipulationWidgetImpl>(single_state))
{
  ui->setupUi(this);

  ui->group_combo_box->setModel(&data_->group_names_model);
  ui->tcp_combo_box->setModel(&data_->tcp_names_model);
  ui->state_tree_view->setModel(&data_->state_models[0]);

  addToolBar();

  connect(ui->state_tree_view, &QTreeView::expanded, [this]() { ui->state_tree_view->resizeColumnToContents(0); });

  connect(ui->group_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onGroupNameChanged()));
  connect(ui->tcp_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onTCPNameChanged()));
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

int ManipulationWidget::getStateCount() const { return (data_->single_state) ? 1 : 2; }

void ManipulationWidget::setEnvironment(std::shared_ptr<const tesseract_environment::Environment> env)
{
  data_->env = env;
  onReset();
  Q_EMIT environmentSet(data_->env);
}

std::shared_ptr<const tesseract_environment::Environment> ManipulationWidget::getEnvironment() const
{
  return data_->env;
}

const tesseract_environment::Environment& ManipulationWidget::environment() const { return *data_->env; }

const std::unordered_map<std::string, LinkVisibilityProperties>& ManipulationWidget::getLinkVisibilityProperties() const
{
  return data_->link_visibility_properties;
}

std::unordered_map<std::string, LinkVisibilityProperties>& ManipulationWidget::getLinkVisibilityProperties()
{
  return data_->link_visibility_properties;
}

void ManipulationWidget::onRender() {}
void ManipulationWidget::onShowAllLinks()
{
  std::vector<std::string> link_names;
  link_names.reserve(data_->link_visibility_properties.size());

  for (auto& link : data_->link_visibility_properties)
  {
    link.second.link = true;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}
void ManipulationWidget::onHideAllLinks()
{
  std::vector<std::string> link_names;
  link_names.reserve(data_->link_visibility_properties.size());

  for (auto& link : data_->link_visibility_properties)
  {
    link.second.link = false;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}
void ManipulationWidget::onShowVisualAllLinks()
{
  std::vector<std::string> link_names;
  link_names.reserve(data_->link_visibility_properties.size());

  for (auto& link : data_->link_visibility_properties)
  {
    link.second.visual = true;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void ManipulationWidget::onHideVisualAllLinks()
{
  std::vector<std::string> link_names;
  link_names.reserve(data_->link_visibility_properties.size());

  for (auto& link : data_->link_visibility_properties)
  {
    link.second.visual = false;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void ManipulationWidget::onShowCollisionAllLinks()
{
  std::vector<std::string> link_names;
  link_names.reserve(data_->link_visibility_properties.size());

  for (auto& link : data_->link_visibility_properties)
  {
    link.second.collision = true;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void ManipulationWidget::onHideCollisionAllLinks()
{
  std::vector<std::string> link_names;
  link_names.reserve(data_->link_visibility_properties.size());

  for (auto& link : data_->link_visibility_properties)
  {
    link.second.collision = false;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void ManipulationWidget::onShowAxisAllLinks()
{
  std::vector<std::string> link_names;
  link_names.reserve(data_->link_visibility_properties.size());

  for (auto& link : data_->link_visibility_properties)
  {
    link.second.axis = true;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void ManipulationWidget::onHideAxisAllLinks()
{
  std::vector<std::string> link_names;
  link_names.reserve(data_->link_visibility_properties.size());

  for (auto& link : data_->link_visibility_properties)
  {
    link.second.axis = false;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void ManipulationWidget::onSelectAllLinks()
{
  std::vector<std::string> link_names;
  link_names.reserve(data_->link_visibility_properties.size());

  for (auto& link : data_->link_visibility_properties)
  {
    link.second.wirebox = true;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void ManipulationWidget::onDeselectAllLinks()
{
  std::vector<std::string> link_names;
  link_names.reserve(data_->link_visibility_properties.size());

  for (auto& link : data_->link_visibility_properties)
  {
    link.second.wirebox = false;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void ManipulationWidget::onEnable()
{
  std::vector<std::string> link_names;
  link_names.reserve(data_->link_visibility_properties.size());

  for (auto& link : data_->link_visibility_properties)
    link_names.push_back(link.first);

  emit linkVisibilityChanged(link_names);
}

void ManipulationWidget::onGroupNameChanged()
{
  if (data_->env != nullptr && data_->env->isInitialized())
  {
    auto lock = data_->env->lockRead();
    auto group_names = data_->env->getGroupNames();
    auto it = group_names.find(ui->group_combo_box->currentText().toStdString());
    if (it != group_names.end())
    {
      data_->kin_group = data_->env->getKinematicGroup(ui->group_combo_box->currentText().toStdString());
      for (std::size_t i = 0; i < getStateCount(); ++i)
      {
        data_->state_solvers[i] = data_->env->getStateSolver();
        data_->state_models[i].clear();
        data_->states[i].clear();
      }

      std::vector<std::string> tcp_names = data_->kin_group->getAllPossibleTipLinkNames();
      QStringList tcp_sl;
      for (const auto& tcp_name : tcp_names)
        tcp_sl.append(tcp_name.c_str());
      data_->tcp_names_model.setStringList(tcp_sl);

      if (!tcp_names.empty())
        ui->tcp_combo_box->setCurrentIndex(0);

      std::vector<std::string> joint_names = data_->kin_group->getJointNames();
      std::vector<tesseract_scene_graph::Joint::ConstPtr> joints;
      joints.reserve(joint_names.size());
      for (const auto& joint_name : joint_names)
        joints.push_back(data_->env->getJoint(joint_name));

      ui->joint_state_slider->setJoints(joints);

      for (std::size_t i = 0; i < getStateCount(); ++i)
        data_->states[i] = ui->joint_state_slider->getJointState();
    }
    else
    {
      ui->joint_state_slider->setJoints(std::vector<tesseract_scene_graph::Joint::ConstPtr>());
    }
  }
  Q_EMIT groupNameChanged(ui->group_combo_box->currentText());
  onJointStateSliderChanged(ui->joint_state_slider->getJointState());
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
}

void ManipulationWidget::onTCPNameChanged() {}

void ManipulationWidget::onManipulatorTypeChanged()
{
  ui->joint_state_slider->setJointState(data_->states[ui->state_combo_box->currentIndex()]);
}

void ManipulationWidget::onJointStateSliderChanged(std::unordered_map<std::string, double> state)
{
  data_->state_solvers[ui->state_combo_box->currentIndex()]->setState(state);
  data_->states[ui->state_combo_box->currentIndex()] = state;
  tesseract_scene_graph::SceneState scene_state = data_->state_solvers[ui->state_combo_box->currentIndex()]->getState();
  tesseract_scene_graph::SceneState reduced_scene_state = getReducedSceneState(scene_state);
  data_->state_models[ui->state_combo_box->currentIndex()].setState(reduced_scene_state);

  // Update the cartesian transform details
  if (ui->mode_combo_box->currentIndex() == 0)
  {
    auto it = scene_state.link_transforms.find(ui->tcp_combo_box->currentText().toStdString());
    if (it != scene_state.link_transforms.end())
      ui->cartesian_widget->setTransform(it->second);
  }

  Q_EMIT manipulationStateChanged(reduced_scene_state, ui->state_combo_box->currentIndex());
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

void ManipulationWidget::onCartesianTransformChanged(const Eigen::Isometry3d& transform)
{
  if (data_->kin_group != nullptr && ui->mode_combo_box->currentIndex() == 1)
  {
    std::vector<std::string> tcp_names = data_->kin_group->getAllPossibleTipLinkNames();
    auto it = std::find(tcp_names.begin(), tcp_names.end(), ui->tcp_combo_box->currentText().toStdString());
    if (it != tcp_names.end())
    {
      tesseract_kinematics::KinGroupIKInput inputs(transform, data_->kin_group->getBaseLinkName(), *it);
      Eigen::VectorXd seed{ Eigen::VectorXd::Zero(data_->kin_group->numJoints()) };
      std::vector<std::string> joint_names = data_->kin_group->getJointNames();
      Eigen::Index idx{ 0 };
      for (const auto& jn : joint_names)
      {
        auto j_it = data_->states[0].find(jn);
        if (j_it != data_->states[0].end())
          seed(idx) = j_it->second;

        ++idx;
      }
      tesseract_kinematics::IKSolutions solutions = data_->kin_group->calcInvKin(inputs, seed);
      if (!solutions.empty())
      {
        /** @todo Should sort solutions closest to the seed state */
        std::unordered_map<std::string, double> state;
        for (int i = 0; i < joint_names.size(); ++i)
          state[joint_names[i]] = solutions[ui->state_combo_box->currentIndex()][i];

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
    data_->state_models[i].clear();
    data_->states[i].clear();
  }

  QString current_group_name = ui->group_combo_box->currentText();
  if (data_->env != nullptr && data_->env->isInitialized())
  {
    auto lock = data_->env->lockRead();

    auto gn = data_->env->getGroupNames();
    std::vector<std::string> gn_v(gn.begin(), gn.end());

    QStringList group_names;
    for (const auto& group_name : data_->env->getGroupNames())
      group_names.append(group_name.c_str());

    ui->group_combo_box->blockSignals(true);
    data_->group_names_model.setStringList(group_names);
    ui->group_combo_box->blockSignals(false);

    auto it = std::find(gn_v.begin(), gn_v.end(), current_group_name.toStdString());
    if (it != gn_v.end())
      ui->group_combo_box->setCurrentText(current_group_name);
    else
      ui->group_combo_box->setCurrentText("");
  }
  else
  {
    data_->group_names_model.setStringList(QStringList());
  }
}

void ManipulationWidget::addToolBar()
{
  data_->toolbar = new QToolBar;  // NOLINT
  data_->show_all_links_action =
      data_->toolbar->addAction(icons::getShowAllLinksIcon(), "Show All Links", this, SLOT(onShowAllLinks()));
  data_->hide_all_links_action =
      data_->toolbar->addAction(icons::getHideAllLinksIcon(), "Hide All Links", this, SLOT(onHideAllLinks()));
  data_->toolbar->addSeparator();
  data_->show_visual_all_links_action = data_->toolbar->addAction(
      icons::getShowVisualAllLinksIcon(), "Show Visual All Links", this, SLOT(onShowVisualAllLinks()));
  data_->hide_visual_all_links_action = data_->toolbar->addAction(
      icons::getHideVisualAllLinksIcon(), "Hide Visual All Links", this, SLOT(onHideVisualAllLinks()));
  data_->toolbar->addSeparator();
  data_->show_collision_all_links_action = data_->toolbar->addAction(
      icons::getShowCollisionAllLinksIcon(), "Show Collision All Links", this, SLOT(onShowCollisionAllLinks()));
  data_->hide_collision_all_links_action = data_->toolbar->addAction(
      icons::getHideCollisionAllLinksIcon(), "Hide Collision All Links", this, SLOT(onHideCollisionAllLinks()));
  data_->toolbar->addSeparator();
  data_->select_all_links_action =
      data_->toolbar->addAction(icons::getSelectAllLinksIcon(), "Select All Links", this, SLOT(onSelectAllLinks()));
  data_->deselect_all_links_action = data_->toolbar->addAction(
      icons::getDeselectAllLinksIcon(), "Deselect All Links", this, SLOT(onDeselectAllLinks()));
  data_->toolbar->addSeparator();
  data_->show_axis_all_links_action = data_->toolbar->addAction(
      icons::getShowAxisAllLinksIcon(), "Show Axis All Links", this, SLOT(onShowAxisAllLinks()));
  data_->hide_axis_all_links_action = data_->toolbar->addAction(
      icons::getHideAxisAllLinksIcon(), "Hide Axis All Links", this, SLOT(onHideAxisAllLinks()));
  data_->toolbar->addSeparator();
  data_->reset_action = data_->toolbar->addAction(icons::getRestoreIcon(), "Reload Environment", this, SLOT(onReset()));

  ui->vertical_layout->insertWidget(0, data_->toolbar);
}
}  // namespace tesseract_gui
