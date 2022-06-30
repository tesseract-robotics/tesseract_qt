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
#include <tesseract_qt/kinematic_groups/group_joint_states_tree_view.h>
#include <tesseract_qt/kinematic_groups/group_joint_states_model.h>
#include <tesseract_qt/common/standard_item_type.h>

namespace tesseract_gui
{
struct GroupJointStatesTreeViewImpl
{
  GroupJointStatesModel* model;

  // Store the selected item
  QStandardItem* selected_item;
};

GroupJointStatesTreeView::GroupJointStatesTreeView(QWidget* parent)
  : QTreeView(parent), data_(std::make_unique<GroupJointStatesTreeViewImpl>())
{
  connect(this, &QTreeView::collapsed, [this]() { resizeColumnToContents(0); });
  connect(this, &QTreeView::expanded, [this]() { resizeColumnToContents(0); });
}

GroupJointStatesTreeView::~GroupJointStatesTreeView() = default;

void GroupJointStatesTreeView::setModel(QAbstractItemModel* model)
{
  data_->model = qobject_cast<GroupJointStatesModel*>(model);
  QTreeView::setModel(model);

  connect(selectionModel(),
          SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
          this,
          SLOT(onCurrentRowChanged(QModelIndex, QModelIndex)));
}

void GroupJointStatesTreeView::onCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous)
{
  QModelIndex current_index = current;  // This appears to be changing so copy
  data_->selected_item = data_->model->itemFromIndex(current_index);

  if (data_->selected_item->type() != static_cast<int>(StandardItemType::COMMON_NAMESPACE))
  {
    const tesseract_srdf::GroupsJointState& state = data_->model->getGroupsJointState(current_index);
    Q_EMIT showGroupsJointState(state);
  }
}

}  // namespace tesseract_gui
