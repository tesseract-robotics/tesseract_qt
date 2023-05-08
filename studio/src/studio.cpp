/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2023 Levi Armstrong <levi.armstrong@gmail.com>
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
#include <tesseract_qt/studio/studio.h>
#include "ui_studio.h"

#include <tesseract_qt/rendering/render_widget.h>
#include <tesseract_qt/rendering/ign_scene_graph_render_manager.h>
#include <tesseract_qt/rendering/ign_tool_path_render_manager.h>
#include <tesseract_qt/rendering/ign_contact_results_render_manager.h>

#include <tesseract_qt/environment/widgets/environment_widget.h>

#include <tesseract_qt/scene_graph/widgets/scene_graph_tool_bar.h>

#include <tesseract_qt/tool_path/widgets/tool_path_tool_bar.h>
#include <tesseract_qt/tool_path/widgets/tool_path_widget.h>

#include <tesseract_qt/manipulation/manipulation_widget.h>
#include <tesseract_qt/manipulation/manipulation_tool_bar.h>

#include <tesseract_qt/joint_trajectory/widgets/joint_trajectory_tool_bar.h>
#include <tesseract_qt/joint_trajectory/widgets/joint_trajectory_widget.h>

#include <tesseract_qt/common/events/tool_path_events.h>
#include <tesseract_qt/common/events/joint_trajectory_events.h>

#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/tool_path.h>
#include <tesseract_qt/common/joint_trajectory_set.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/entity_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>
#include <tesseract_qt/common/widgets/load_environment_dialog.h>
#include <tesseract_qt/studio/studio_plugin_loader_dialog.h>
#include <tesseract_qt/studio/studio_plugin_factory.h>

#include <DockManager.h>
#include <DockAreaWidget.h>

#include <QSettings>
#include <QWidgetAction>
#include <QComboBox>
#include <QInputDialog>
#include <QDialog>

namespace tesseract_gui
{
struct Studio::Implementation
{
  Implementation(Studio* app);

  Studio* app;
  ads::CDockManager* dock_manager{ nullptr };
  QWidgetAction* perspective_list_action{ nullptr };
  QComboBox* perspective_comboBox{ nullptr };
  ComponentInfo component_info{ "studio_environment" };
  ComponentInfo jt_component_info;
  EntityManager::Ptr entity_manager;
  IgnSceneGraphRenderManager scene_graph_manager;
  IgnToolPathRenderManager tool_path_manager;
  IgnContactResultsRenderManager contact_results_manager;

  LoadEnvironmentDialog open_dialog;

  std::shared_ptr<StudioPluginFactory> plugin_factory;
  StudioPluginLoaderDialog plugin_loader_dialog;

  /** @brief Saves the dock manager state and the main window geometry */
  void saveState();

  /** @brief Create a perspective */
  void createPerspective();

  /** @brief Save the list of perspectives */
  void savePerspectives();

  /** @brief Restores the dock manager state */
  void restoreState();

  /** @brief Restore the perspective list of the dock manager */
  void restorePerspectives();
};

Studio::Implementation::Implementation(Studio* app)
  : app(app)
  , jt_component_info(component_info.createChild())
  , entity_manager(std::make_shared<EntityManager>())
  , scene_graph_manager(component_info, entity_manager)
  , tool_path_manager(component_info, entity_manager)
  , contact_results_manager(component_info)
  , open_dialog(component_info, app)
  , plugin_factory(std::make_shared<StudioPluginFactory>())
  , plugin_loader_dialog(plugin_factory, app)
{
}

void Studio::Implementation::saveState()
{
  QSettings Settings("TesseractQtStudioSettings.ini", QSettings::IniFormat);
  Settings.setValue("TesseractQtStudio/Geometry", app->saveGeometry());
  Settings.setValue("TesseractQtStudio/State", app->saveState());
  Settings.setValue("TesseractQtStudio/DockingState", dock_manager->saveState());
}

void Studio::Implementation::createPerspective()
{
  QString PerspectiveName = QInputDialog::getText(app, "Create Perspective", "Enter unique name:");
  if (PerspectiveName.isEmpty())
    return;

  dock_manager->addPerspective(PerspectiveName);
  QSignalBlocker Blocker(perspective_comboBox);
  perspective_comboBox->clear();
  perspective_comboBox->addItems(dock_manager->perspectiveNames());
  perspective_comboBox->setCurrentText(PerspectiveName);

  savePerspectives();
}

void Studio::Implementation::savePerspectives()
{
  QSettings Settings("TesseractQtStudioSettings.ini", QSettings::IniFormat);
  dock_manager->savePerspectives(Settings);
}

void Studio::Implementation::restoreState()
{
  QSettings Settings("TesseractQtStudioSettings.ini", QSettings::IniFormat);
  app->restoreGeometry(Settings.value("TesseractQtStudio/Geometry").toByteArray());
  app->restoreState(Settings.value("TesseractQtStudio/State").toByteArray());
  dock_manager->restoreState(Settings.value("TesseractQtStudio/DockingState").toByteArray());
}

void Studio::Implementation::restorePerspectives()
{
  QSettings Settings("TesseractQtStudioSettings.ini", QSettings::IniFormat);
  dock_manager->loadPerspectives(Settings);
  perspective_comboBox->clear();
  perspective_comboBox->addItems(dock_manager->perspectiveNames());
}

Studio::Studio(QWidget* parent)
  : QMainWindow(parent), ui(std::make_unique<Ui::Studio>()), data_(std::make_unique<Implementation>(this))
{
  ui->setupUi(this);
  setWindowTitle(QApplication::applicationName());

  data_->dock_manager = new ads::CDockManager(this);
  data_->dock_manager->setStyleSheet("");

  // Setup actions
  ui->actionOpen->setIcon(icons::getOpenIcon());
  ui->actionSave_State->setIcon(icons::getSaveIcon());
  ui->actionRestore_State->setIcon(icons::getRestoreIcon());
  ui->actionCreate_Perspective->setIcon(icons::getLayoutIcon());
  ui->actionLoad_Plugins->setIcon(icons::getPluginIcon());
  connect(ui->actionOpen, &QAction::triggered, [this]() { data_->open_dialog.show(); });
  connect(ui->actionSave_State, &QAction::triggered, [this]() { data_->saveState(); });
  connect(ui->actionRestore_State, &QAction::triggered, [this]() { data_->restoreState(); });
  connect(ui->actionCreate_Perspective, &QAction::triggered, [this]() { data_->createPerspective(); });
  connect(ui->actionLoad_Plugins, &QAction::triggered, [this]() { data_->plugin_loader_dialog.show(); });
  data_->perspective_list_action = new QWidgetAction(this);
  data_->perspective_comboBox = new QComboBox(this);
  data_->perspective_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  data_->perspective_comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  data_->perspective_list_action->setDefaultWidget(data_->perspective_comboBox);
  ui->toolBar->addAction(data_->perspective_list_action);
  connect(data_->perspective_comboBox,
          SIGNAL(activated(const QString&)),
          data_->dock_manager,
          SLOT(openPerspective(const QString&)));

  auto render_widget = new tesseract_gui::RenderWidget(data_->component_info.scene_name);
  render_widget->setSkyEnabled(true);
  auto* render_dock_widget = new ads::CDockWidget("Scene");
  render_dock_widget->setWidget(render_widget);
  render_dock_widget->setFeature(ads::CDockWidget::DockWidgetFocusable, true);
  auto* central_area = data_->dock_manager->setCentralWidget(render_dock_widget);
  central_area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

  auto* env_dock_widget = new ads::CDockWidget("Environment");
  env_dock_widget->setWidget(new tesseract_gui::EnvironmentWidget(data_->component_info));
  env_dock_widget->setToolBar(new tesseract_gui::SceneGraphToolBar(data_->component_info));
  env_dock_widget->toolBar()->setIconSize(QSize(25, 25));
  data_->dock_manager->addDockWidget(ads::LeftDockWidgetArea, env_dock_widget, central_area);
  ui->menuView->addAction(env_dock_widget->toggleViewAction());

  auto* tool_path_dock_widget = new ads::CDockWidget("Tool Paths");
  tool_path_dock_widget->setWidget(new tesseract_gui::ToolPathWidget(data_->component_info));
  tool_path_dock_widget->setToolBar(new tesseract_gui::ToolPathToolBar(data_->component_info));
  tool_path_dock_widget->toolBar()->setIconSize(QSize(25, 25));
  auto* right_area = data_->dock_manager->addDockWidget(ads::RightDockWidgetArea, tool_path_dock_widget, central_area);
  ui->menuView->addAction(tool_path_dock_widget->toggleViewAction());

  auto* joint_traj_dock_widget = new ads::CDockWidget("Joint Trajectories");
  joint_traj_dock_widget->setWidget(new tesseract_gui::JointTrajectoryWidget(data_->jt_component_info));
  joint_traj_dock_widget->setToolBar(new tesseract_gui::JointTrajectoryToolBar(data_->jt_component_info));
  joint_traj_dock_widget->toolBar()->setIconSize(QSize(25, 25));
  data_->dock_manager->addDockWidgetTabToArea(joint_traj_dock_widget, right_area);
  ui->menuView->addAction(joint_traj_dock_widget->toggleViewAction());

  auto* manipulation_dock_widget = new ads::CDockWidget("Manipulation");
  manipulation_dock_widget->setWidget(new tesseract_gui::ManipulationWidget(data_->component_info, true));
  manipulation_dock_widget->setToolBar(new tesseract_gui::ManipulationToolBar(data_->component_info));
  manipulation_dock_widget->toolBar()->setIconSize(QSize(25, 25));
  data_->dock_manager->addDockWidgetTabToArea(manipulation_dock_widget, right_area);
  ui->menuView->addAction(manipulation_dock_widget->toggleViewAction());

  //  data_->restoreState(); If this is enabled widgets do not show up if name change
  data_->restorePerspectives();
}

Studio::~Studio() = default;
}  // namespace tesseract_gui
