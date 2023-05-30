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
#include <tesseract_qt/common/widgets/load_environment_dialog.h>
#include "ui_load_environment_dialog.h"

#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_common/resource_locator.h>
#include <tesseract_environment/environment.h>

#include <QMessageBox>

namespace tesseract_gui
{
struct LoadEnvironmentDialog::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;
  tesseract_common::ResourceLocator::Ptr resource_locator;
};

LoadEnvironmentDialog::LoadEnvironmentDialog(QWidget* parent) : LoadEnvironmentDialog(nullptr, parent) {}

LoadEnvironmentDialog::LoadEnvironmentDialog(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent)
  : QDialog(parent), ui_(std::make_unique<Ui::LoadEnvironmentDialog>()), data_(std::make_unique<Implementation>())
{
  ui_->setupUi(this);

  setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
  setModal(true);

  data_->component_info = std::move(component_info);
  data_->resource_locator = std::make_shared<tesseract_common::GeneralResourceLocator>();

  connect(ui_->button_box, SIGNAL(accepted()), this, SLOT(onAccepted()));
}

LoadEnvironmentDialog::~LoadEnvironmentDialog() = default;

void LoadEnvironmentDialog::setResourceLocator(std::shared_ptr<tesseract_common::ResourceLocator> resource_locator)
{
  data_->resource_locator = resource_locator;
}

void LoadEnvironmentDialog::onAccepted()
{
  tesseract_common::fs::path urdf_filepath(ui_->load_widget->getURDFFilePath().toStdString());
  tesseract_common::fs::path srdf_filepath(ui_->load_widget->getSRDFFilePath().toStdString());
  bool urdf_filepath_exists = tesseract_common::fs::exists(urdf_filepath);
  bool srdf_filepath_exists = tesseract_common::fs::exists(srdf_filepath);
  if (urdf_filepath_exists && srdf_filepath_exists)
  {
    auto env = std::make_shared<tesseract_environment::Environment>();
    if (!env->init(urdf_filepath, srdf_filepath, data_->resource_locator) || !env->isInitialized())
    {
      QMessageBox messageBox;
      messageBox.critical(0, "Error", "Failed to load environment!");
      messageBox.setFixedSize(500, 200);
      messageBox.show();
      return;
    }
    auto env_wrapper = std::make_shared<DefaultEnvironmentWrapper>(data_->component_info, env);
    EnvironmentManager::set(env_wrapper);
  }

  if (urdf_filepath_exists && !srdf_filepath_exists)
  {
    auto env = std::make_shared<tesseract_environment::Environment>();
    if (!env->init(urdf_filepath, data_->resource_locator) || !env->isInitialized())
    {
      QMessageBox messageBox;
      messageBox.critical(0, "Error", "Failed to load environment!");
      messageBox.setFixedSize(500, 200);
      messageBox.show();
      return;
    }
    auto env_wrapper = std::make_shared<DefaultEnvironmentWrapper>(data_->component_info, env);
    EnvironmentManager::set(env_wrapper);
  }
}
}  // namespace tesseract_gui
