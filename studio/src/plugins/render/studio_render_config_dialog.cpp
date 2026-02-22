#include <tesseract_qt/studio/plugins/render/studio_render_config_dialog.h>
#include "ui_studio_render_config_dialog.h"

namespace tesseract::gui
{
StudioRenderConfigDialog::StudioRenderConfigDialog(QWidget* parent)
  : QDialog(parent), ui(std::make_unique<Ui::StudioRenderConfigDialog>())
{
  ui->setupUi(this);
  setWindowTitle("Studio Render Config");
}

StudioRenderConfigDialog::~StudioRenderConfigDialog() = default;

std::shared_ptr<const ComponentInfo> StudioRenderConfigDialog::getComponentInfo() const
{
  return ui->component_info_widget->getComponentInfo();
}
bool StudioRenderConfigDialog::assignAsCentralWidget() const { return ui->central_widget_check_box->isChecked(); }
bool StudioRenderConfigDialog::skyEnabled() const { return ui->sky_check_box->isChecked(); }
bool StudioRenderConfigDialog::gridEnabled() const { return ui->grid_check_box->isChecked(); }
bool StudioRenderConfigDialog::shadowsEnabled() const { return ui->shadows_check_box->isChecked(); }
}  // namespace tesseract::gui
