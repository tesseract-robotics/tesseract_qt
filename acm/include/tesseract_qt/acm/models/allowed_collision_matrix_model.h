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
#include <memory>
#include <QStandardItemModel>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

namespace tesseract_common
{
class AllowedCollisionMatrix;
}

namespace tesseract_gui
{
struct ComponentInfo;
class AllowedCollisionMatrixModel : public QStandardItemModel
{
  Q_OBJECT
public:
  AllowedCollisionMatrixModel(QObject* parent = nullptr);
  explicit AllowedCollisionMatrixModel(std::shared_ptr<const ComponentInfo> component_info, QObject* parent = nullptr);
  AllowedCollisionMatrixModel(const AllowedCollisionMatrixModel& other);
  AllowedCollisionMatrixModel& operator=(const AllowedCollisionMatrixModel& other);
  ~AllowedCollisionMatrixModel() override;

  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

  tesseract_common::AllowedCollisionMatrix getAllowedCollisionMatrix() const;

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;

  void set(const tesseract_common::AllowedCollisionMatrix& acm);
  void add(const std::string& link1_name, const std::string& link2_name, const std::string& reason);
  void remove(const std::string& link1_name, const std::string& link2_name);
  void remove(const std::string& link_name);
  void clear();

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_ACM_ALLOWED_COLLISION_MATRIX_MODEL_H
