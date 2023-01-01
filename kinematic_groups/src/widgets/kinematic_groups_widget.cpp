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

#include <tesseract_qt/kinematic_groups/widgets/kinematic_groups_widget.h>
#include <tesseract_qt/kinematic_groups/models/kinematic_groups_model.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/tree_view.h>

#include <QVBoxLayout>

namespace tesseract_gui
{
struct KinematicGroupsWidget::Implementation
{
  std::shared_ptr<KinematicGroupsModel> model;
  QVBoxLayout* layout;
  TreeView* tree_view;
};

KinematicGroupsWidget::KinematicGroupsWidget(QWidget* parent) : KinematicGroupsWidget(ComponentInfo(), parent) {}

KinematicGroupsWidget::KinematicGroupsWidget(ComponentInfo component_info, QWidget* parent)
  : QWidget(parent), data_(std::make_unique<Implementation>())
{
  // Create model
  data_->model = std::make_shared<KinematicGroupsModel>(std::move(component_info));

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
KinematicGroupsWidget::~KinematicGroupsWidget() = default;

void KinematicGroupsWidget::setComponentInfo(ComponentInfo component_info)
{
  // Create model
  data_->model = std::make_shared<KinematicGroupsModel>(std::move(component_info));
  data_->tree_view->setModel(data_->model.get());
}

const ComponentInfo& KinematicGroupsWidget::getComponentInfo() const { return data_->model->getComponentInfo(); }

void KinematicGroupsWidget::setModel(std::shared_ptr<KinematicGroupsModel> model)
{
  data_->model = std::move(model);
  data_->tree_view->setModel(data_->model.get());
}
std::shared_ptr<KinematicGroupsModel> KinematicGroupsWidget::getModel() { return data_->model; }
std::shared_ptr<const KinematicGroupsModel> KinematicGroupsWidget::getModel() const { return data_->model; }

QItemSelectionModel& KinematicGroupsWidget::getSelectionModel() { return *data_->tree_view->selectionModel(); }
const QItemSelectionModel& KinematicGroupsWidget::getSelectionModel() const
{
  return *data_->tree_view->selectionModel();
}

}  // namespace tesseract_gui
