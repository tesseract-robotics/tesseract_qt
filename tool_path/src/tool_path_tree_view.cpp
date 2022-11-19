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
#include <tesseract_qt/tool_path/tool_path_tree_view.h>
#include <tesseract_qt/tool_path/tool_path_model.h>
#include <tesseract_qt/common/tool_path_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <QMouseEvent>

namespace tesseract_gui
{
struct ToolPathTreeViewImpl
{
  ToolPathModel* model;

  // Store the selected item
  QStandardItem* selected_item;
};

ToolPathTreeView::ToolPathTreeView(QWidget* parent) : QTreeView(parent), data_(std::make_unique<ToolPathTreeViewImpl>())
{
  connect(this, &QTreeView::collapsed, [this]() { resizeColumnToContents(0); });
  connect(this, &QTreeView::expanded, [this]() { resizeColumnToContents(0); });
}

ToolPathTreeView::~ToolPathTreeView() = default;

void ToolPathTreeView::setModel(QAbstractItemModel* model)
{
  data_->model = qobject_cast<ToolPathModel*>(model);
  QTreeView::setModel(model);
}

}  // namespace tesseract_gui
