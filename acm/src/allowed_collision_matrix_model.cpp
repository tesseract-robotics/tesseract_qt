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

#include <tesseract_qt/acm/allowed_collision_matrix_model.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
AllowedCollisionMatrixModel::AllowedCollisionMatrixModel(QObject* parent) : QStandardItemModel(parent)
{
  setColumnCount(3);
}

AllowedCollisionMatrixModel::AllowedCollisionMatrixModel(const AllowedCollisionMatrixModel& other)
  : QStandardItemModel(other.d_ptr->parent)
{
  this->acm_ = other.acm_;
}

AllowedCollisionMatrixModel& AllowedCollisionMatrixModel::operator=(const AllowedCollisionMatrixModel& other)
{
  this->acm_ = other.acm_;
  return *this;
}

void AllowedCollisionMatrixModel::setAllowedCollisionMatrix(const tesseract_common::AllowedCollisionMatrix& acm)
{
  clear();

  acm_ = acm;

  double elapse{ 0 };
  for (const auto& ac : acm.getAllAllowedCollisions())
  {
    auto col0 = new QStandardItem(QString::fromStdString(ac.first.first));   // NOLINT
    auto col1 = new QStandardItem(QString::fromStdString(ac.first.second));  // NOLINT
    auto col2 = new QStandardItem(QString::fromStdString(ac.second));        // NOLINT
    appendRow({ col0, col1, col2 });
  }

  sort(0);
}

void AllowedCollisionMatrixModel::add(const QString& link1_name, const QString& link2_name, const QString& reason)
{
  acm_.addAllowedCollision(link1_name.toStdString(), link2_name.toStdString(), reason.toStdString());
  QList<QStandardItem*> item;
  item.push_back(new QStandardItem(link1_name));
  item.push_back(new QStandardItem(link2_name));
  item.push_back(new QStandardItem(reason));
  appendRow(item);
  sort(0);

  // If the count does not match then it was a replace so rebuild model.
  if (rowCount() != static_cast<int>(acm_.getAllAllowedCollisions().size()))
    setAllowedCollisionMatrix(acm_);

  emit entryAdded(link1_name, link2_name, reason);
}

bool AllowedCollisionMatrixModel::removeRows(int row, int count, const QModelIndex& parent)
{
  if (row >= 0)
  {
    QString link1_name = item(row, 0)->text();
    QString link2_name = item(row, 1)->text();
    acm_.removeAllowedCollision(link1_name.toStdString(), link2_name.toStdString());

    bool success = QStandardItemModel::removeRows(row, count, parent);

    if (success)
    {
      emit entryRemoved(link1_name, link2_name);
    }

    return success;
  }
  return false;
}

void AllowedCollisionMatrixModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(3);
  acm_.clearAllowedCollisions();
}

tesseract_common::AllowedCollisionMatrix AllowedCollisionMatrixModel::getAllowedCollisionMatrix() const { return acm_; }

QVariant AllowedCollisionMatrixModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
      case 0:
        return QString("Link 1");
      case 1:
        return QString("Link 2");
      case 2:
        return QString("Reason");
    }
  }
  return QVariant();
}

}  // namespace tesseract_gui
