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
#include <tesseract_qt/srdf/srdf_editor_widget.h>
#include "ui_srdf_editor_widget.h"

#include <tesseract_common/resource_locator.h>
#include <tesseract_srdf/srdf_model.h>
#include <tesseract_environment/environment.h>
#include <tesseract_collision/core/types.h>
#include <tesseract_collision/core/common.h>
#include <tesseract_scene_graph/joint.h>

#include <QStringListModel>
#include <QFileDialog>
#include <QStatusBar>

#include <tesseract_qt/acm/models/allowed_collision_matrix_model.h>
#include <tesseract_qt/kinematic_groups/models/kinematic_groups_model.h>
#include <tesseract_qt/kinematic_groups/models/group_joint_states_model.h>
#include <tesseract_qt/kinematic_groups/models/group_tcps_model.h>

#include <tesseract_qt/common/events/group_tcps_events.h>
#include <tesseract_qt/common/events/group_joint_states_events.h>
#include <tesseract_qt/common/events/kinematic_groups_events.h>
#include <tesseract_qt/common/events/allowed_collision_matrix_events.h>
#include <tesseract_qt/common/component_info.h>

static const tesseract_gui::ComponentInfo COMPONENT_INFO{ "srdf_scene" };

namespace tesseract_gui
{
struct SRDFEditorWidgetImpl
{
  /** @brief URDF file path */
  QString urdf_filepath;

  /** @brief SRDF file path */
  QString srdf_filepath;

  /** @brief The SRDF model */
  tesseract_srdf::SRDFModel srdf_model;

  /** @brief The Tesseract resource locator */
  tesseract_common::ResourceLocator::Ptr locator;

  tesseract_environment::Environment::Ptr env;

  QStringListModel link_model;

  QStringListModel joint_model;

  AllowedCollisionMatrixModel acm_model{ COMPONENT_INFO };

  QStringListModel group_names_model;

  KinematicGroupsModel kin_groups_model;

  GroupJointStatesModel group_joint_states_model;

  //    UserDefinedTCPModel user_tcp_model;

  //    OPWKinematicsModel opw_kinematics_model;

  QStringListModel group_link_list_model;

  QStringListModel group_joint_list_model;
};

SRDFEditorWidget::SRDFEditorWidget(std::shared_ptr<tesseract_common::ResourceLocator> locator, QWidget* parent)
  : QWidget(parent), ui_(std::make_unique<Ui::SRDFEditorWidget>()), data_(std::make_unique<SRDFEditorWidgetImpl>())
{
  ui_->setupUi(this);
  data_->locator = std::move(locator);

  ui_->toolBox->setCurrentIndex(0);
  //  ui_->acm_widget->setModel(&data_->acm_model);
  //  ui_->group_states_widget->setModels(&data_->group_joint_states_model, &data_->group_names_model);
  //  ui_->groups_widget->setModels(&data_->kin_groups_model, &data_->link_model, &data_->joint_model);

  //  ui_->group_states_widget->config([this](QString group_name) { return groupJointsRetrevier(group_name); });

  /** @todo Add validators to kinematic group editor widget */

  connect(ui_->urdf_browse_button, SIGNAL(clicked()), this, SLOT(onBrowseURDFLoadClicked()));
  connect(ui_->srdf_browse_button, SIGNAL(clicked()), this, SLOT(onBrowseSRDFLoadClicked()));
  connect(ui_->load_push_button, SIGNAL(clicked()), this, SLOT(onLoad()));

  connect(ui_->acm_widget, SIGNAL(generateClicked(long)), this, SLOT(onGenerateACM(long)));

  connect(ui_->save_srdf_browse_button, SIGNAL(clicked()), this, SLOT(onBrowseSRDFSaveClicked()));
  connect(ui_->save_srdf_save_button, SIGNAL(clicked()), this, SLOT(onSave()));

  enablePages(false);
}

SRDFEditorWidget::~SRDFEditorWidget() {}

void SRDFEditorWidget::onLoad(const QString& urdf_filepath, const QString& srdf_filepath)
{
  Q_EMIT showStatusMessage("Parsing URDF/SRDF!", 0);
  this->data_->env = std::make_shared<tesseract_environment::Environment>();
  this->data_->urdf_filepath =
      QString::fromStdString(this->data_->locator->locateResource(urdf_filepath.toStdString())->getFilePath());
  bool loaded = false;
  if (srdf_filepath.toStdString().empty())
  {
    loaded =
        this->data_->env->init(boost::filesystem::path(this->data_->urdf_filepath.toStdString()), this->data_->locator);
  }
  else
  {
    this->data_->srdf_filepath =
        QString::fromStdString(this->data_->locator->locateResource(srdf_filepath.toStdString())->getFilePath());
    loaded = this->data_->env->init(boost::filesystem::path(this->data_->urdf_filepath.toStdString()),
                                    boost::filesystem::path(this->data_->srdf_filepath.toStdString()),
                                    this->data_->locator);

    // Store the base srdf model
    if (loaded)
      this->data_->srdf_model.initFile(
          *(this->data_->env->getSceneGraph()), this->data_->srdf_filepath.toStdString(), *(this->data_->locator));
  }

  if (!loaded)
  {
    Q_EMIT showStatusMessage("Failed to parse URDF/SRDF!", 2000);
    enablePages(false);
    return;
  }

  if (this->data_->env)
  {
    // Clear Models
    this->data_->joint_model.setStringList(QStringList());
    this->data_->link_model.setStringList(QStringList());
    this->data_->group_names_model.setStringList(QStringList());

    QApplication::sendEvent(qApp, new events::AllowedCollisionMatrixClear(COMPONENT_INFO));

    QApplication::sendEvent(qApp, new events::KinematicGroupsClear(COMPONENT_INFO));
    this->data_->group_link_list_model.setStringList(QStringList());
    this->data_->group_joint_list_model.setStringList(QStringList());

    QApplication::sendEvent(qApp, new events::GroupJointStatesClear(COMPONENT_INFO));
    //    this->data_->user_tcp_model.clear();
    //    this->data_->opw_kinematics_model.clear();

    // Get kinematics information
    tesseract_srdf::KinematicsInformation kin_info = this->data_->env->getKinematicsInformation();

    // Build link list model
    std::vector<std::string> link_names = this->data_->env->getLinkNames();
    QStringList links;
    links.reserve(static_cast<int>(link_names.size()));
    for (const auto& link_name : link_names)
      links.push_back(QString::fromStdString(link_name));

    data_->link_model.setStringList(links);
    data_->link_model.sort(0);

    // Build joint list model
    std::vector<std::string> joint_names = this->data_->env->getActiveJointNames();
    QStringList joints;
    joints.reserve(static_cast<int>(joint_names.size()));
    for (const auto& joint_name : joint_names)
      joints.push_back(QString::fromStdString(joint_name));

    data_->joint_model.setStringList(joints);
    data_->joint_model.sort(0);

    // Build group names list model
    QStringList group_names;
    group_names.reserve(static_cast<int>(kin_info.group_names.size()));
    for (const auto& group_name : kin_info.group_names)
      group_names.push_back(QString::fromStdString(group_name));

    data_->group_names_model.setStringList(group_names);
    data_->group_names_model.sort(0);

    // Build ACM Model
    QApplication::sendEvent(
        qApp, new events::AllowedCollisionMatrixSet(COMPONENT_INFO, *(data_->env->getAllowedCollisionMatrix())));

    // Build Kinematic Groups Model
    QApplication::sendEvent(qApp,
                            new events::KinematicGroupsSet(
                                COMPONENT_INFO, kin_info.chain_groups, kin_info.joint_groups, kin_info.link_groups));

    // Build Groups Joint States Model
    QApplication::sendEvent(qApp, new events::GroupJointStatesSet(COMPONENT_INFO, kin_info.group_states));

    //    // Build Groups TCPs Model
    //    this->data_->user_tcp_model.setEnvironment(this->data_->render_util.getEnvironment());

    //    // Build OPW kinematics Model
    //    this->data_->opw_kinematics_model.setEnvironment(this->data_->render_util.getEnvironment());

    ui_->toolBox->setCurrentIndex(1);

    Q_EMIT environmentSet(data_->env);
  }

  Q_EMIT showStatusMessage("Successfully parsed URDF/SRDF!", 2000);
  enablePages(this->data_->env != nullptr);
}

void SRDFEditorWidget::onLoad() { onLoad(data_->urdf_filepath, data_->srdf_filepath); }

void SRDFEditorWidget::onSave(const QString& srdf_filepath)
{
  std::string local_path = this->data_->locator->locateResource(srdf_filepath.toStdString())->getFilePath();
  tesseract_srdf::SRDFModel srdf_model = this->data_->srdf_model;
  srdf_model.acm = *(this->data_->env->getAllowedCollisionMatrix());
  srdf_model.kinematics_information = this->data_->env->getKinematicsInformation();
  srdf_model.saveToFile(local_path);
  Q_EMIT showStatusMessage("Successfully saved SRDF!", 2000);
}

void SRDFEditorWidget::onSave() { onSave(data_->srdf_filepath); }

void SRDFEditorWidget::onBrowseURDFLoadClicked()
{
  QStringList filters;
  filters.append("URDF (*.urdf)");

  QFileDialog dialog(this, "Open URDF");
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setNameFilters(filters);
  if (dialog.exec() == 1)
  {
    data_->urdf_filepath = dialog.selectedFiles().first();
    ui_->urdf_line_edit->setText(data_->urdf_filepath);
  }
}

void SRDFEditorWidget::onBrowseSRDFLoadClicked()
{
  QStringList filters;
  filters.append("SRDF (*.srdf)");

  QFileDialog dialog(this, "Open SRDF");
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setNameFilters(filters);
  if (dialog.exec() == 1)
  {
    data_->srdf_filepath = dialog.selectedFiles().first();
    ui_->srdf_line_edit->setText(data_->srdf_filepath);
    ui_->save_srdf_line_edit->setText(data_->srdf_filepath);
  }
}

void SRDFEditorWidget::onBrowseSRDFSaveClicked()
{
  QStringList filters;
  filters.append("SRDF (*.srdf)");

  QFileDialog dialog(this, "Save SRDF");
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setNameFilters(filters);
  if (dialog.exec() == 1)
  {
    data_->srdf_filepath = dialog.selectedFiles().first();
    ui_->srdf_line_edit->setText(data_->srdf_filepath);
    ui_->save_srdf_line_edit->setText(data_->srdf_filepath);
  }
}

void SRDFEditorWidget::onAddChainGroup(const QString& group_name, const QString& base_link, const QString& tip_link)
{
  // If group name is empty then return
  if (group_name.trimmed().isEmpty())
    return;

  tesseract_srdf::ChainGroup list = { std::make_pair(base_link.toStdString(), tip_link.toStdString()) };
  std::set<std::string> groups = this->data_->env->getKinematicsInformation().group_names;

  QApplication::sendEvent(qApp, new events::KinematicGroupsAddChain(COMPONENT_INFO, group_name.toStdString(), list));

  if (!group_name.isEmpty())
  {
    if (std::find(groups.begin(), groups.end(), group_name.toStdString()) != groups.end())
    {
      // Remove Group States, TCPs and OPW Kinematics associated with the group
      QApplication::sendEvent(qApp,
                              new events::GroupJointStatesRemoveGroup(COMPONENT_INFO, { group_name.toStdString() }));
      QApplication::sendEvent(qApp, new events::GroupTCPsRemoveGroup(COMPONENT_INFO, { group_name.toStdString() }));
      //    removeGroupOPWKinematics({group_name.toStdString()});
    }
  }
}

void SRDFEditorWidget::onAddJointGroup(const QString& group_name)
{
  // If group name is empty then return
  if (group_name.trimmed().isEmpty())
    return;

  std::set<std::string> groups = this->data_->env->getKinematicsInformation().group_names;
  QStringList qjoints = this->data_->group_joint_list_model.stringList();
  std::vector<std::string> joints;
  joints.reserve(qjoints.size());
  for (const auto& j : qjoints)
    joints.push_back(j.toStdString());

  QApplication::sendEvent(qApp, new events::KinematicGroupsAddJoint(COMPONENT_INFO, group_name.toStdString(), joints));

  if (!group_name.isEmpty())
  {
    if (std::find(groups.begin(), groups.end(), group_name.toStdString()) != groups.end())
    {
      // Remove Group States, TCPs and OPW Kinematics associated with the group
      QApplication::sendEvent(qApp,
                              new events::GroupJointStatesRemoveGroup(COMPONENT_INFO, { group_name.toStdString() }));
      QApplication::sendEvent(qApp, new events::GroupTCPsRemoveGroup(COMPONENT_INFO, { group_name.toStdString() }));
      //      removeGroupOPWKinematics(group_name);
    }
  }
}

void SRDFEditorWidget::onAddLinkGroup(const QString& group_name)
{
  // If group name is empty then return
  if (group_name.trimmed().isEmpty())
    return;

  std::set<std::string> groups = this->data_->env->getKinematicsInformation().group_names;

  QStringList qlinks = this->data_->group_link_list_model.stringList();
  std::vector<std::string> links;
  links.reserve(qlinks.size());
  for (const auto& l : qlinks)
    links.push_back(l.toStdString());

  QApplication::sendEvent(qApp, new events::KinematicGroupsAddLink(COMPONENT_INFO, group_name.toStdString(), links));

  if (!group_name.isEmpty())
  {
    if (std::find(groups.begin(), groups.end(), group_name.toStdString()) != groups.end())
    {
      // Remove Group States, TCPs and OPW Kinematics associated with the group
      QApplication::sendEvent(qApp,
                              new events::GroupJointStatesRemoveGroup(COMPONENT_INFO, { group_name.toStdString() }));
      QApplication::sendEvent(qApp, new events::GroupTCPsRemoveGroup(COMPONENT_INFO, { group_name.toStdString() }));
      //    removeGroupOPWKinematics(group_name);
    }
  }
}

void SRDFEditorWidget::onAddJointGroupJoint(const QString& joint_name)
{
  QStringList l = this->data_->group_joint_list_model.stringList();
  if (!l.contains(joint_name) && !joint_name.isEmpty())
  {
    l.append(joint_name);
    this->data_->group_joint_list_model.setStringList(l);
  }
}

void SRDFEditorWidget::onRemoveJointGroupJoint(int index) { this->data_->group_joint_list_model.removeRow(index); }

void SRDFEditorWidget::onAddLinkGroupLink(const QString& link_name)
{
  QStringList l = this->data_->group_link_list_model.stringList();
  if (!l.contains(link_name) && !link_name.isEmpty())
  {
    l.append(link_name);
    this->data_->group_link_list_model.setStringList(l);
  }
}

void SRDFEditorWidget::onRemoveLinkGroupLink(int index) { this->data_->group_link_list_model.removeRow(index); }

void SRDFEditorWidget::onRemoveKinematicGroup(int index)
{
  if (index >= 0)
  {
    std::vector<std::string> remove_groups;
    remove_groups.push_back(
        this->data_->kin_groups_model.item(index, 0)->data(Qt::DisplayRole).toString().toStdString());

    QApplication::sendEvent(qApp, new events::KinematicGroupsRemove(COMPONENT_INFO, remove_groups));
    QApplication::sendEvent(qApp, new events::GroupJointStatesRemoveGroup(COMPONENT_INFO, remove_groups));
    QApplication::sendEvent(qApp, new events::GroupTCPsRemoveGroup(COMPONENT_INFO, remove_groups));
    //    removeGroupOPWKinematics(group_name);
  }
}

void SRDFEditorWidget::onGenerateACM(long resolution)
{
  Q_EMIT showStatusMessage("Generating allowed collision matrix!", 0);

  auto contact_manager = this->data_->env->getDiscreteContactManager();
  auto state_solver = this->data_->env->getStateSolver();

  // We want to disable the allowed contact function for this process so it is set null
  contact_manager->setIsContactAllowedFn(nullptr);
  tesseract_collision::ContactResultMap results;
  tesseract_collision::ContactRequest request;
  request.type = tesseract_collision::ContactTestType::ALL;

  for (long i = 0; i < resolution; ++i)
  {
    tesseract_scene_graph::SceneState state = state_solver->getRandomState();
    contact_manager->setCollisionObjectsTransform(state.link_transforms);
    contact_manager->contactTest(results, request);
  }

  QApplication::sendEvent(qApp, new events::AllowedCollisionMatrixClear(COMPONENT_INFO));
  tesseract_common::AllowedCollisionMatrix acm;
  for (const auto& pair : results)
  {
    double percent = double(pair.second.size()) / double(resolution);
    if (percent > 0.95)
    {
      std::vector<std::string> adj_first = this->data_->env->getSceneGraph()->getAdjacentLinkNames(pair.first.first);
      std::vector<std::string> adj_second = this->data_->env->getSceneGraph()->getAdjacentLinkNames(pair.first.second);
      if (std::find(adj_first.begin(), adj_first.end(), pair.first.second) != adj_first.end())
        acm.addAllowedCollision(pair.first.first, pair.first.second, "Adjacent");
      else if (std::find(adj_second.begin(), adj_second.end(), pair.first.first) != adj_second.end())
        acm.addAllowedCollision(pair.first.second, pair.first.first, "Adjacent");
      else
        acm.addAllowedCollision(pair.first.second, pair.first.first, "Allways");
    }
  }

  std::vector<std::string> link_names = this->data_->env->getLinkNames();
  for (std::size_t i = 0; i < link_names.size() - 1; ++i)
  {
    const auto& link1 = this->data_->env->getLink(link_names[i]);
    if (link1->collision.empty())
      continue;

    for (std::size_t j = i + 1; j < link_names.size(); ++j)
    {
      const auto& link2 = this->data_->env->getLink(link_names[j]);
      if (link2->collision.empty())
        continue;

      if (results.find(tesseract_collision::getObjectPairKey(link_names[i], link_names[j])) == results.end())
        acm.addAllowedCollision(link_names[i], link_names[j], "Never");
    }
  }

  auto cmd = std::make_shared<tesseract_environment::ModifyAllowedCollisionsCommand>(
      acm, tesseract_environment::ModifyAllowedCollisionsType::REPLACE);
  this->data_->env->applyCommand(cmd);

  QApplication::sendEvent(qApp, new events::AllowedCollisionMatrixSet(COMPONENT_INFO, acm));

  Q_EMIT showStatusMessage("Finished generating allowed collision matrix!", 2000);
}

void SRDFEditorWidget::onRemoveACMEntry(int index) { this->data_->acm_model.removeRow(index); }

void SRDFEditorWidget::onClickedACMEntry(int /*index*/)
{
  //  this->data_->selected_entities.clear();
  //  this->data_->selected_entities.push_back(this->data_->entity_manager.getLink("link_1"));
  //  this->data_->update_selections = true;
}

void SRDFEditorWidget::onLoadJointGroup(const QString& group_name)
{
  //  this->data_->joint_group_model.clear();
  //  auto kin =
  //  this->data_->render_util.getEnvironmentConst()->getManipulatorManager()->getFwdKinematicSolver(group_name.toStdString());
  //  if (kin != nullptr)
  //    for (const auto& joint_name : kin->getJointNames())
  //      this->data_->joint_group_model.add(this->data_->render_util.getEnvironmentConst()->getSceneGraph()->getJoint(joint_name));
}

void SRDFEditorWidget::onJointValue(const QString& joint_name, double joint_value)
{
  //  for (int i = 0; i < this->data_->joint_group_model.rowCount(); ++i)
  //  {
  //    QStandardItem* item = this->data_->joint_group_model.item(i);
  //    if (item->data(this->data_->joint_group_model.NameRole).toString() == joint_name)
  //    {
  //      item->setData(QString::number(joint_value), this->data_->joint_group_model.ValueRole);
  //      break;
  //    }
  //  }
  //  this->data_->render_util.setEnvironmentState({joint_name.toStdString()}, {joint_value});
}

void SRDFEditorWidget::onAddUserDefinedJointState(const QString& group_name, const QString& state_name)
{
  //  if (!group_name.trimmed().isEmpty() && !state_name.trimmed().isEmpty())
  //  {
  //    QStringList joint_names;
  //    QStringList joint_values;
  //    for (int i = 0; i < this->data_->joint_group_model.rowCount(); ++i)
  //    {
  //      QStandardItem* item = this->data_->joint_group_model.item(i);
  //      joint_names.push_back(item->data(this->data_->joint_group_model.NameRole).toString());
  //      joint_values.push_back(item->data(this->data_->joint_group_model.ValueRole).toString());
  //    }
  //    this->data_->group_joint_states_model.add(group_name, state_name, joint_names, joint_values);
  //  }
}

void SRDFEditorWidget::onRemoveUserDefinedJointState(int index)
{
  this->data_->group_joint_states_model.removeRow(index);
}

void SRDFEditorWidget::onAddUserDefinedTCP(const QString& group_name,
                                           const QString& tcp_name,
                                           const QVector3D& position,
                                           const QVector3D& orientation)
{
  //  if (!group_name.trimmed().isEmpty() && !tcp_name.trimmed().isEmpty())
  //    this->data_->user_tcp_model.add(group_name, tcp_name, position, orientation);
}

void SRDFEditorWidget::onRemoveUserDefinedTCP(int index)
{
  //  this->data_->user_tcp_model.removeRow(index);
}

void SRDFEditorWidget::onAddGroupOPWKinematics(const QString& group_name,
                                               double a1,
                                               double a2,
                                               double b,
                                               double c1,
                                               double c2,
                                               double c3,
                                               double c4,
                                               double o1,
                                               double o2,
                                               double o3,
                                               double o4,
                                               double o5,
                                               double o6,
                                               int sc1,
                                               int sc2,
                                               int sc3,
                                               int sc4,
                                               int sc5,
                                               int sc6)
{
  //  if (!group_name.trimmed().isEmpty())
  //    this->data_->opw_kinematics_model.add(group_name, a1, a2, b, c1, c2, c3, c4, o1, o2, o3, o4, o5, o6, sc1, sc2,
  //    sc3, sc4, sc5, sc6);
}

void SRDFEditorWidget::onRemoveGroupOPWKinematics(int index)
{
  //  this->data_->opw_kinematics_model.removeRow(index);
}

void SRDFEditorWidget::enablePages(bool enable)
{
  ui_->acm_page->setEnabled(enable);
  ui_->kin_group_states_page->setEnabled(enable);
  ui_->kin_groups_page->setEnabled(enable);
  ui_->save_page->setEnabled(enable);
}

std::vector<std::shared_ptr<const tesseract_scene_graph::Joint>>
SRDFEditorWidget::groupJointsRetrevier(QString group_name)
{
  std::vector<tesseract_scene_graph::Joint::ConstPtr> joints;
  std::vector<std::string> joint_names = data_->env->getGroupJointNames(group_name.toStdString());
  joints.reserve(joint_names.size());
  for (const auto& joint_name : joint_names)
    joints.push_back(data_->env->getJoint(joint_name));

  return joints;
}
}  // namespace tesseract_gui
