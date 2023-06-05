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

#include <tesseract_qt/kinematic_groups/widgets/group_joint_states_widget.h>
#include <tesseract_qt/kinematic_groups/models/group_joint_states_model.h>
#include <tesseract_qt/common/widgets/tree_view.h>
#include <tesseract_qt/common/component_info.h>

#include <QVBoxLayout>

namespace tesseract_gui
{
struct GroupJointStatesWidget::Implementation
{
  std::shared_ptr<GroupJointStatesModel> model;
  QVBoxLayout* layout;
  TreeView* tree_view;
};

GroupJointStatesWidget::GroupJointStatesWidget(QWidget* parent) : GroupJointStatesWidget(nullptr, parent) {}

GroupJointStatesWidget::GroupJointStatesWidget(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent)
  : QWidget(parent), data_(std::make_unique<Implementation>())
{
  // Create model
  data_->model = std::make_shared<GroupJointStatesModel>(std::move(component_info));

  // Create tree widget
  data_->tree_view = new TreeView();
  data_->tree_view->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
  data_->tree_view->setModel(data_->model.get());

  // Create layout
  data_->layout = new QVBoxLayout();
  data_->layout->setMargin(0);
  data_->layout->setSpacing(0);
  data_->layout->addWidget(data_->tree_view);

  // Set layout
  setLayout(data_->layout);
}
GroupJointStatesWidget::~GroupJointStatesWidget() = default;

void GroupJointStatesWidget::setComponentInfo(std::shared_ptr<const ComponentInfo> component_info)
{
  // Create model
  data_->model = std::make_shared<GroupJointStatesModel>(std::move(component_info));
  data_->tree_view->setModel(data_->model.get());
}

std::shared_ptr<const ComponentInfo> GroupJointStatesWidget::getComponentInfo() const
{
  return data_->model->getComponentInfo();
}

void GroupJointStatesWidget::setModel(std::shared_ptr<GroupJointStatesModel> model)
{
  data_->model = std::move(model);
  data_->tree_view->setModel(data_->model.get());
}
std::shared_ptr<GroupJointStatesModel> GroupJointStatesWidget::getModel() { return data_->model; }
std::shared_ptr<const GroupJointStatesModel> GroupJointStatesWidget::getModel() const { return data_->model; }

QItemSelectionModel& GroupJointStatesWidget::getSelectionModel() { return *data_->tree_view->selectionModel(); }
const QItemSelectionModel& GroupJointStatesWidget::getSelectionModel() const
{
  return *data_->tree_view->selectionModel();
}

}  // namespace tesseract_gui
