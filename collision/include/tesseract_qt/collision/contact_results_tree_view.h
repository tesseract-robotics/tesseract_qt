/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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
#ifndef TESSERACT_QT_COLLISION_CONTACT_RESULTS_TREE_VIEW_H
#define TESSERACT_QT_COLLISION_CONTACT_RESULTS_TREE_VIEW_H

#include <QTreeView>
#include <memory>
#include <tesseract_collision/core/types.h>

namespace tesseract_gui
{
struct ContactResultsTreeViewImpl;
class ContactResultsTreeView : public QTreeView
{
  Q_OBJECT
public:
  explicit ContactResultsTreeView(QWidget* parent = nullptr);
  ~ContactResultsTreeView();

  void setModel(QAbstractItemModel* model) override;

Q_SIGNALS:
  void showContactResults(const tesseract_collision::ContactResultVector& contact_results);

public Q_SLOTS:
  void onCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous);
  void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
  void mousePressEvent(QMouseEvent* event) override;

private:
  std::unique_ptr<ContactResultsTreeViewImpl> data_;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_COLLISION_CONTACT_RESULTS_TREE_VIEW_H
