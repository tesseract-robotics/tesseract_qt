#include <tesseract_qt/tool_path/widgets/load_tool_path_dialog.h>
#include <tesseract_qt/common/component_info.h>

#include "ui_load_tool_path_dialog.h"

namespace tesseract_gui
{
LoadToolPathDialog::LoadToolPathDialog(QWidget* parent) : LoadToolPathDialog(ComponentInfo(), parent) {}

LoadToolPathDialog::LoadToolPathDialog(ComponentInfo component_info, QWidget* parent)
  : QDialog(parent)
  , ui_(std::make_unique<Ui::LoadToolPathDialog>())
  , component_info_(std::make_unique<ComponentInfo>(component_info))
{
  ui_->setupUi(this);
}

LoadToolPathDialog::~LoadToolPathDialog() = default;

QString LoadToolPathDialog::getLinkName() const { return ui_->frame_combo_box->currentText(); }
QString LoadToolPathDialog::getFilePath() const { return ui_->file_path_line_edit->text(); }

}  // namespace tesseract_gui
