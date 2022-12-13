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
#include <tesseract_qt/environment/environment_widget.h>
#include "ui_environment_widget.h"

#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/environment/environment_widget_config.h>
#include <tesseract_qt/environment/environment_commands_model.h>
#include <tesseract_qt/scene_graph/scene_graph_model.h>
#include <tesseract_qt/scene_graph/scene_state_model.h>
#include <tesseract_qt/kinematic_groups/kinematic_groups_model.h>
#include <tesseract_qt/kinematic_groups/group_tcps_model.h>
#include <tesseract_qt/kinematic_groups/group_joint_states_model.h>
#include <tesseract_qt/acm/allowed_collision_matrix_model.h>
#include <tesseract_qt/collision/contact_results_widget.h>
#include <tesseract_qt/collision/contact_results_model.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/utils.h>
#include <tesseract_qt/common/image_viewer_widget.h>

#include <tesseract_collision/core/types.h>

#include <QStandardItemModel>

namespace tesseract_gui
{
struct EnvironmentWidgetImpl
{
  EnvironmentWidgetImpl() : config(std::make_shared<EnvironmentWidgetConfig>()) {}

  EnvironmentWidgetConfig::Ptr config{ nullptr };

  ContactResultsModel* contact_results_model;
};

EnvironmentWidget::EnvironmentWidget(QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::EnvironmentWidget>()), data_(std::make_unique<EnvironmentWidgetImpl>())
{
  ui->setupUi(this);

  ui->tab_widget->setCurrentIndex(0);

  // Setup Contacts tab
  data_->contact_results_model = new ContactResultsModel();
  ui->contacts_widget->setModel(data_->contact_results_model);

  connect(ui->scene_tree_view, &QTreeView::collapsed, [this]() { ui->scene_tree_view->resizeColumnToContents(0); });
  connect(ui->scene_tree_view, &QTreeView::expanded, [this]() { ui->scene_tree_view->resizeColumnToContents(0); });
  connect(ui->state_tree_view, &QTreeView::collapsed, [this]() { ui->state_tree_view->resizeColumnToContents(0); });
  connect(ui->state_tree_view, &QTreeView::expanded, [this]() { ui->state_tree_view->resizeColumnToContents(0); });
  connect(ui->groups_tree_view, &QTreeView::collapsed, [this]() { ui->groups_tree_view->resizeColumnToContents(0); });
  connect(ui->groups_tree_view, &QTreeView::expanded, [this]() { ui->groups_tree_view->resizeColumnToContents(0); });
  connect(ui->group_tcps_tree_view, &QTreeView::collapsed, [this]() {
    ui->group_tcps_tree_view->resizeColumnToContents(0);
  });
  connect(ui->group_tcps_tree_view, &QTreeView::expanded, [this]() {
    ui->group_tcps_tree_view->resizeColumnToContents(0);
  });

  connect(ui->acm_tree_view, &QTreeView::collapsed, [this]() { ui->acm_tree_view->resizeColumnToContents(0); });
  connect(ui->acm_tree_view, &QTreeView::expanded, [this]() { ui->acm_tree_view->resizeColumnToContents(0); });
  connect(ui->cmd_history_tree_view, &QTreeView::collapsed, [this]() {
    ui->cmd_history_tree_view->resizeColumnToContents(0);
  });
  connect(ui->cmd_history_tree_view, &QTreeView::expanded, [this]() {
    ui->cmd_history_tree_view->resizeColumnToContents(0);
  });

  //  connect(ui->acm_tree_view, SIGNAL(entrySelected()), this, SIGNAL(entrySelected()));
  connect(ui->acm_tree_view,
          SIGNAL(selectedLinksChanged(std::vector<std::string>)),
          this,
          SLOT(onACMSelectedLinks(std::vector<std::string>)));

  connect(ui->group_states_tree_view,
          SIGNAL(showGroupsJointState(std::unordered_map<std::string, double>)),
          this,
          SLOT(onShowGroupsJointState(std::unordered_map<std::string, double>)));
  connect(ui->contacts_widget,
          SIGNAL(showContactResults(tesseract_collision::ContactResultVector)),
          this,
          SIGNAL(showContactResults(tesseract_collision::ContactResultVector)));
}

EnvironmentWidget::~EnvironmentWidget() = default;

void EnvironmentWidget::setConfiguration(std::shared_ptr<EnvironmentWidgetConfig> config)
{
  if (config != nullptr)
  {
    disconnect(data_->config.get(), SIGNAL(modelsUpdated()), this, SLOT(onModelsUpdated()));
    disconnect(data_->config.get(),
               SIGNAL(environmentSet(std::shared_ptr<tesseract_environment::Environment>)),
               this,
               SIGNAL(environmentSet(std::shared_ptr<tesseract_environment::Environment>)));
    disconnect(data_->config.get(),
               SIGNAL(environmentChanged(tesseract_environment::Environment)),
               this,
               SIGNAL(environmentChanged(tesseract_environment::Environment)));
    disconnect(data_->config.get(),
               SIGNAL(environmentCurrentStateChanged(tesseract_environment::Environment)),
               this,
               SIGNAL(environmentCurrentStateChanged(tesseract_environment::Environment)));
  }

  data_->config = std::move(config);
  ui->scene_tree_view->setModel(&data_->config->getSceneGraphModel());
  ui->state_tree_view->setModel(&data_->config->getSceneStateModel());
  ui->groups_tree_view->setModel(&data_->config->getKinematicGroupsModel());
  ui->group_tcps_tree_view->setModel(&data_->config->getGroupTCPsModel());
  ui->group_states_tree_view->setModel(&data_->config->getGroupJointStatesModel());
  ui->acm_tree_view->setModel(&data_->config->getAllowedCollisionMatrixModel());
  ui->cmd_history_tree_view->setModel(&data_->config->getEnvironmentCommandsModel());

  ui->contacts_widget->setEnvironment(data_->config->getEnvironment());

  onModelsUpdated();

  connect(data_->config.get(), SIGNAL(modelsUpdated()), this, SLOT(onModelsUpdated()));
  connect(data_->config.get(),
          SIGNAL(environmentSet(std::shared_ptr<tesseract_environment::Environment>)),
          this,
          SIGNAL(environmentSet(std::shared_ptr<tesseract_environment::Environment>)));
  connect(data_->config.get(),
          SIGNAL(environmentChanged(tesseract_environment::Environment)),
          this,
          SIGNAL(environmentChanged(tesseract_environment::Environment)));
  connect(data_->config.get(),
          SIGNAL(environmentCurrentStateChanged(tesseract_environment::Environment)),
          this,
          SIGNAL(environmentCurrentStateChanged(tesseract_environment::Environment)));
  connect(&data_->config->getSceneGraphModel(),
          SIGNAL(itemChanged(QStandardItem*)),
          this,
          SLOT(onSceneGraphModelItemChanged(QStandardItem*)));
  connect(&data_->config->getSceneStateModel(),
          SIGNAL(itemChanged(QStandardItem*)),
          this,
          SLOT(onSceneStateModelItemChanged(QStandardItem*)));
  connect(this,
          SIGNAL(linkVisibilityChanged(std::vector<std::string>)),
          this,
          SLOT(updateVisibilityCheckedStates(std::vector<std::string>)));

  emit configurationSet(*data_->config);
  emit environmentSet(data_->config->getEnvironment());
}

const tesseract_environment::Environment& EnvironmentWidget::environment() const
{
  return data_->config->environment();
}

tesseract_environment::Environment& EnvironmentWidget::environment() { return data_->config->environment(); }

tesseract_environment::Environment::ConstPtr EnvironmentWidget::getEnvironment() const
{
  return data_->config->getEnvironment();
}

tesseract_environment::Environment::Ptr EnvironmentWidget::getEnvironment() { return data_->config->getEnvironment(); }

const std::unordered_map<std::string, LinkVisibilityProperties>& EnvironmentWidget::getLinkVisibilityProperties() const
{
  return data_->config->getLinkVisibilityProperties();
}

EnvironmentWidget* EnvironmentWidget::clone() const
{
  return new EnvironmentWidget(nullptr);  // NOLINT
}

void EnvironmentWidget::onModelsUpdated()
{
  if (!data_->config->environment().isInitialized())
    return;

  // This hides the root element
  ui->group_states_tree_view->setRootIndex(data_->config->getGroupJointStatesModel().index(0, 0));

  // This hides the root element
  ui->group_tcps_tree_view->setRootIndex(data_->config->getGroupTCPsModel().index(0, 0));

  // This hides the root element
  ui->cmd_history_tree_view->setRootIndex(data_->config->getEnvironmentCommandsModel().index(0, 0));

  // New data may have been added so resize first column
  ui->scene_tree_view->resizeColumnToContents(0);
  ui->state_tree_view->resizeColumnToContents(0);
  ui->acm_tree_view->resizeColumnToContents(0);
  ui->groups_tree_view->resizeColumnToContents(0);
  ui->group_tcps_tree_view->resizeColumnToContents(0);
  ui->group_states_tree_view->resizeColumnToContents(0);
  ui->cmd_history_tree_view->resizeColumnToContents(0);
}

void EnvironmentWidget::onRender(float /*dt*/) {}

void EnvironmentWidget::onPlotSceneGraph()
{
  tesseract_common::fs::path dot_path("/tmp/environment_widget_scene_graph.dot");
  tesseract_common::fs::path image_path("/tmp/environment_widget_scene_graph.png");
  data_->config->getEnvironment()->getSceneGraph()->saveDOT(dot_path.c_str());
  saveDotImage(dot_path, image_path, "png");

  auto* image_viewer = new ImageViewerWidget();
  image_viewer->loadImage(image_path.c_str());
  image_viewer->show();
}

void EnvironmentWidget::onEnable()
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  std::vector<std::string> link_names;
  link_names.reserve(link_visibility_properties.size());

  for (auto& link : link_visibility_properties)
    link_names.push_back(link.first);

  emit linkVisibilityChanged(link_names);
}

void EnvironmentWidget::onACMSelectedLinks(const std::vector<std::string>& link_names)
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  std::vector<std::string> changed_link_names;
  changed_link_names.reserve(link_visibility_properties.size());

  for (auto& link : link_visibility_properties)
  {
    link.second.wirebox = false;
    changed_link_names.push_back(link.first);
  }

  for (const auto& link : link_names)
  {
    auto it = link_visibility_properties.find(link);
    if (it != link_visibility_properties.end())
      it->second.wirebox = true;
  }

  emit linkVisibilityChanged(changed_link_names);
}

void EnvironmentWidget::onShowGroupsJointState(const std::unordered_map<std::string, double>& groups_joint_state)
{
  data_->config->environment().setState(groups_joint_state);
}

}  // namespace tesseract_gui
