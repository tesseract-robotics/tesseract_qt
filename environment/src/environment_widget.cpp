/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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

#include <tesseract_collision/core/types.h>

#include <QStandardItemModel>
#include <QToolBar>

namespace tesseract_gui
{
struct EnvironmentWidgetImpl
{
  EnvironmentWidgetImpl() : config(std::make_shared<EnvironmentWidgetConfig>()) {}

  EnvironmentWidgetConfig::Ptr config{ nullptr };

  ContactResultsModel* contact_results_model;

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
};

EnvironmentWidget::EnvironmentWidget(QWidget* parent, bool add_toolbar)
  : QWidget(parent), ui(std::make_unique<Ui::EnvironmentWidget>()), data_(std::make_unique<EnvironmentWidgetImpl>())
{
  ui->setupUi(this);

  ui->tab_widget->setCurrentIndex(0);

  if (add_toolbar)
  {
    createToolBar();
    ui->verticalLayout->insertWidget(0, data_->toolbar);
  }

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
  return new EnvironmentWidget(nullptr, data_->toolbar != nullptr);  // NOLINT
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

void EnvironmentWidget::onRender() {}

void EnvironmentWidget::onShowAllLinks()
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  std::vector<std::string> link_names;
  link_names.reserve(link_visibility_properties.size());

  for (auto& link : link_visibility_properties)
  {
    link.second.link = true;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void EnvironmentWidget::onHideAllLinks()
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  std::vector<std::string> link_names;
  link_names.reserve(link_visibility_properties.size());

  for (auto& link : link_visibility_properties)
  {
    link.second.link = false;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void EnvironmentWidget::onShowVisualAllLinks()
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  std::vector<std::string> link_names;
  link_names.reserve(link_visibility_properties.size());

  for (auto& link : link_visibility_properties)
  {
    link.second.visual = true;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void EnvironmentWidget::onHideVisualAllLinks()
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  std::vector<std::string> link_names;
  link_names.reserve(link_visibility_properties.size());

  for (auto& link : link_visibility_properties)
  {
    link.second.visual = false;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void EnvironmentWidget::onShowCollisionAllLinks()
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  std::vector<std::string> link_names;
  link_names.reserve(link_visibility_properties.size());

  for (auto& link : link_visibility_properties)
  {
    link.second.collision = true;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}
void EnvironmentWidget::onHideCollisionAllLinks()
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  std::vector<std::string> link_names;
  link_names.reserve(link_visibility_properties.size());

  for (auto& link : link_visibility_properties)
  {
    link.second.collision = false;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void EnvironmentWidget::onShowAxisAllLinks()
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  std::vector<std::string> link_names;
  link_names.reserve(link_visibility_properties.size());

  for (auto& link : link_visibility_properties)
  {
    link.second.axis = true;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void EnvironmentWidget::onHideAxisAllLinks()
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  std::vector<std::string> link_names;
  link_names.reserve(link_visibility_properties.size());

  for (auto& link : link_visibility_properties)
  {
    link.second.axis = false;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void EnvironmentWidget::onSelectAllLinks()
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  std::vector<std::string> link_names;
  link_names.reserve(link_visibility_properties.size());

  for (auto& link : link_visibility_properties)
  {
    link.second.wirebox = true;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
}

void EnvironmentWidget::onDeselectAllLinks()
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  std::vector<std::string> link_names;
  link_names.reserve(link_visibility_properties.size());

  for (auto& link : link_visibility_properties)
  {
    link.second.wirebox = false;
    link_names.push_back(link.first);
  }

  emit linkVisibilityChanged(link_names);
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

QStandardItem* findLinkStandardItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::SG_LINK))
    return item;

  return findLinkStandardItem(item->parent());
}

void EnvironmentWidget::onSceneGraphModelItemChanged(QStandardItem* item)
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  if (item->type() == static_cast<int>(StandardItemType::SG_LINK))
  {
    auto it = link_visibility_properties.find(item->text().toStdString());
    if (it != link_visibility_properties.end())
    {
      it->second.link = (item->checkState() == Qt::CheckState::Checked);
      emit linkVisibilityChanged({ it->first });
    }
  }
  else if (item->type() == static_cast<int>(StandardItemType::SG_VISUALS))
  {
    QStandardItem* link_item = findLinkStandardItem(item);
    auto it = link_visibility_properties.find(link_item->text().toStdString());
    if (it != link_visibility_properties.end())
    {
      it->second.visual = (item->checkState() == Qt::CheckState::Checked);
      emit linkVisibilityChanged({ it->first });
    }
  }
  else if (item->type() == static_cast<int>(StandardItemType::SG_COLLISIONS))
  {
    QStandardItem* link_item = findLinkStandardItem(item);
    auto it = link_visibility_properties.find(link_item->text().toStdString());
    if (it != link_visibility_properties.end())
    {
      it->second.collision = (item->checkState() == Qt::CheckState::Checked);
      emit linkVisibilityChanged({ it->first });
    }
  }
}

void EnvironmentWidget::onSceneStateModelItemChanged(QStandardItem* item)
{
  LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();

  if (item->type() == static_cast<int>(StandardItemType::COMMON_TRANSFORM))
  {
    auto it = link_visibility_properties.find(item->text().toStdString());
    if (it != link_visibility_properties.end())
    {
      it->second.axis = (item->checkState() == Qt::CheckState::Checked);
      emit linkVisibilityChanged({ it->first });
    }
  }
}

void EnvironmentWidget::updateVisibilityCheckedStates(const std::vector<std::string>& links)
{
  const LinkVisibilityPropertiesMap& link_visibility_properties = data_->config->getLinkVisibilityProperties();
  data_->config->getSceneGraphModel().blockSignals(true);
  data_->config->getSceneStateModel().blockSignals(true);
  for (const auto& link : links)
  {
    QString link_name = QString::fromStdString(link);
    auto it = link_visibility_properties.find(link);
    if (it != link_visibility_properties.end())
    {
      data_->config->getSceneGraphModel().onLinkCheckedStateChanged(link_name, it->second.link);
      data_->config->getSceneGraphModel().onLinkVisualsCheckedStateChanged(link_name, it->second.visual);
      data_->config->getSceneGraphModel().onLinkCollisionsCheckedStateChanged(link_name, it->second.collision);

      data_->config->getSceneStateModel().onLinkAxisCheckedStateChanged(link_name, it->second.axis);
    }
  }
  data_->config->getSceneGraphModel().blockSignals(false);
  data_->config->getSceneStateModel().blockSignals(false);
}

void EnvironmentWidget::createToolBar()
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
}
}  // namespace tesseract_gui
