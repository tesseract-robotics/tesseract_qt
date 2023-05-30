#include <tesseract_qt/studio/plugins/manipulation/studio_manipulation_config_dialog.h>
#include "ui_studio_manipulation_config_dialog.h"

#include <tesseract_qt/common/component_info.h>
namespace tesseract_gui
{
StudioManipulationConfigDialog::StudioManipulationConfigDialog(QWidget* parent)
  : QDialog(parent), ui(std::make_unique<Ui::StudioManipulationConfigDialog>())
{
  ui->setupUi(this);
}

StudioManipulationConfigDialog::~StudioManipulationConfigDialog() = default;

std::shared_ptr<const ComponentInfo> StudioManipulationConfigDialog::getComponentInfo() const
{
  return ui->component_info_widget->getComponentInfo();
}

bool StudioManipulationConfigDialog::createChildEnvironment() const
{
  return ui->create_child_env_check_box->isChecked();
}

}  // namespace tesseract_gui
