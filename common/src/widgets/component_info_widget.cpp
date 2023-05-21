#include <tesseract_qt/common/widgets/component_info_widget.h>

#include "ui_component_info_widget.h"

#include <tesseract_qt/common/component_info.h>

namespace tesseract_gui
{
ComponentInfoWidget::ComponentInfoWidget(QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::ComponentInfoWidget>())
{
  ui->setupUi(this);
}

ComponentInfoWidget::~ComponentInfoWidget() = default;

// void ComponentInfoWidget::setSceneNameModel(QStringListModel* scene_name_model) {}

// void ComponentInfoWidget::setParentNamespaceModel(QStringListModel* parent_ns_model) {}

// void ComponentInfoWidget::setComponentInfo(const ComponentInfo& component_info)
//{
//  //  if (component_info.hasParent())
//  //  {
//  //    auto parent = component_info.getParent();
//  //    ui->scene_name_line_edit->setText(parent->scene_name.c_str());
//  //    ui->ns_line_edit->setText(parent->ns.c_str());
//  //    ui->description_line_edit->setText(parent->description.c_str());
//  //    ui->create_child_check_box->setEnabled(true);
//  //    ui->child_desc_line_edit->setText(component_info.description);
//  //  }
//  //  else
//  //  {
//  //    ui->scene_name_line_edit->setText(component_info.scene_name.c_str());
//  //    ui->ns_line_edit->setText(component_info.ns.c_str());
//  //    ui->description_line_edit->setText(component_info.description.c_str());
//  //    ui->create_child_check_box->setEnabled(false);
//  //  }
//}

std::shared_ptr<ComponentInfo> ComponentInfoWidget::getComponentInfo() const
{
  //  ComponentInfo component_info;
  //  component_info.scene_name = ui->scene_name_line_edit->text().toStdString();
  //  component_info.ns = ui->ns_line_edit->text().toStdString();
  //  component_info.description = ui->description_line_edit->text().toStdString();

  //  if (!ui->create_child_check_box->isEnabled())
  //    return component_info;

  //  ComponentInfo child = component_info.createChild();
  //  child.description = ui->child_desc_line_edit->text().toStdString();
  //  return child;
}

}  // namespace tesseract_gui
