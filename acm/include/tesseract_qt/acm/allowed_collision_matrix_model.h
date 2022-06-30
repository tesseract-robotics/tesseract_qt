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
#ifndef TESSERACT_QT_ACM_ALLOWED_COLLISION_MATRIX_MODEL_H
#define TESSERACT_QT_ACM_ALLOWED_COLLISION_MATRIX_MODEL_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_common/allowed_collision_matrix.h>
#include <QStandardItemModel>
#include <QMetaType>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

namespace tesseract_gui
{
class AllowedCollisionMatrixModel : public QStandardItemModel
{
  Q_OBJECT
public:
  AllowedCollisionMatrixModel(QObject* parent = nullptr);
  AllowedCollisionMatrixModel(const AllowedCollisionMatrixModel& other);
  AllowedCollisionMatrixModel& operator=(const AllowedCollisionMatrixModel& other);
  ~AllowedCollisionMatrixModel() override = default;

  Q_INVOKABLE void setAllowedCollisionMatrix(const tesseract_common::AllowedCollisionMatrix& acm);
  Q_INVOKABLE void add(const QString& link1_name, const QString& link2_name, const QString& reason);
  Q_INVOKABLE void clear();

  bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

  tesseract_common::AllowedCollisionMatrix getAllowedCollisionMatrix() const;

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
  void entryRemoved(QString link1_name, QString link2_name);
  void entryAdded(QString link1_name, QString link2_name, QString reason);

private:
  tesseract_common::AllowedCollisionMatrix acm_;
};
}  // namespace tesseract_gui
Q_DECLARE_METATYPE(tesseract_gui::AllowedCollisionMatrixModel)

#endif  // TESSERACT_QT_ACM_ALLOWED_COLLISION_MATRIX_MODEL_H
