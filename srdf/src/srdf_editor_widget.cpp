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
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

namespace tesseract_gui
{
struct SRDFEditorWidget::Implementation
{
  /** @brief The component information */
  ComponentInfo component_info;

  /** @brief URDF file path */
  QString urdf_filepath;

  /** @brief SRDF file path */
  QString srdf_filepath;

  /** @brief The SRDF model */
  tesseract_srdf::SRDFModel srdf_model;

  /** @brief The Tesseract resource locator */
  tesseract_common::ResourceLocator::Ptr locator;

  /** @brief The current environment */
  tesseract_environment::Environment::Ptr env;
};

SRDFEditorWidget::SRDFEditorWidget(std::shared_ptr<tesseract_common::ResourceLocator> locator, QWidget* parent)
  : SRDFEditorWidget(ComponentInfo(), std::move(locator), parent)
{
}

SRDFEditorWidget::SRDFEditorWidget(ComponentInfo component_info,
                                   std::shared_ptr<tesseract_common::ResourceLocator> locator,
                                   QWidget* parent)
  : QWidget(parent), ui_(std::make_unique<Ui::SRDFEditorWidget>()), data_(std::make_unique<Implementation>())
{
  ui_->setupUi(this);

  data_->component_info = std::move(component_info);
  data_->locator = std::move(locator);

  ui_->acm_widget->setComponentInfo(data_->component_info);
  ui_->groups_widget->setComponentInfo(data_->component_info);
  ui_->group_states_widget->setComponentInfo(data_->component_info);

  ui_->toolBox->setCurrentIndex(0);

  /** @todo Add validators to kinematic group editor widget */

  connect(ui_->urdf_browse_button, SIGNAL(clicked()), this, SLOT(onBrowseURDFLoadClicked()));
  connect(ui_->srdf_browse_button, SIGNAL(clicked()), this, SLOT(onBrowseSRDFLoadClicked()));
  connect(ui_->load_push_button, SIGNAL(clicked()), this, SLOT(onLoad()));

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
    // Remove existing environment wrapper
    EnvironmentManager::remove(data_->component_info);

    QApplication::sendEvent(qApp, new events::AllowedCollisionMatrixClear(data_->component_info));
    QApplication::sendEvent(qApp, new events::KinematicGroupsClear(data_->component_info));
    QApplication::sendEvent(qApp, new events::GroupJointStatesClear(data_->component_info));
    //    this->data_->user_tcp_model.clear();
    //    this->data_->opw_kinematics_model.clear();

    auto env_wrapper = std::make_shared<DefaultEnvironmentWrapper>(data_->component_info, data_->env);
    EnvironmentManager::set(env_wrapper);

    ui_->toolBox->setCurrentIndex(1);
  }

  Q_EMIT showStatusMessage("Successfully parsed URDF/SRDF!", 2000);
  enablePages(this->data_->env != nullptr);
}

void SRDFEditorWidget::onLoad() { onLoad(data_->urdf_filepath, data_->srdf_filepath); }

void SRDFEditorWidget::onSave(const QString& srdf_filepath)
{
  std::string local_path = this->data_->locator->locateResource(srdf_filepath.toStdString())->getFilePath();
  tesseract_srdf::SRDFModel srdf_model = this->data_->srdf_model;
  srdf_model.name = data_->env->getName();
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

void SRDFEditorWidget::enablePages(bool enable)
{
  ui_->acm_page->setEnabled(enable);
  ui_->kin_group_states_page->setEnabled(enable);
  ui_->kin_groups_page->setEnabled(enable);
  ui_->save_page->setEnabled(enable);
}

}  // namespace tesseract_gui
