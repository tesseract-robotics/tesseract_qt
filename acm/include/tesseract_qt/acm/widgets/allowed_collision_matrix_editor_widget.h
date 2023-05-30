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
#ifndef TESSERACT_QT_ACM_GENERATE_ALLOWED_COLLISION_MATRIX_WIDGET_H
#define TESSERACT_QT_ACM_GENERATE_ALLOWED_COLLISION_MATRIX_WIDGET_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_qt/acm/models/allowed_collision_matrix_model.h>
#include <tesseract_common/allowed_collision_matrix.h>
#include <memory>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <QWidget>

namespace Ui
{
class AllowedCollisionMatrixEditorWidget;
}

class QItemSelectionModel;

namespace tesseract_gui
{
class AddAllowedCollisionEntryDialog;
class AllowedCollisionMatrixEditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit AllowedCollisionMatrixEditorWidget(QWidget* parent = nullptr);
  explicit AllowedCollisionMatrixEditorWidget(std::shared_ptr<const ComponentInfo> component_info,
                                              QWidget* parent = nullptr);
  ~AllowedCollisionMatrixEditorWidget();

  void setComponentInfo(std::shared_ptr<const ComponentInfo> component_info);
  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

  void setModel(std::shared_ptr<AllowedCollisionMatrixModel> model);
  std::shared_ptr<AllowedCollisionMatrixModel> getModel();
  std::shared_ptr<const AllowedCollisionMatrixModel> getModel() const;

  QItemSelectionModel& getSelectionModel();
  const QItemSelectionModel& getSelectionModel() const;

public Q_SLOTS:
  void onRemoveButtonClicked();
  void onAddButtonClicked();
  void onGenerateButtonClicked();
  void onApplyButtonClicked();

private:
  struct Implementation;
  std::unique_ptr<Ui::AllowedCollisionMatrixEditorWidget> ui_;
  std::unique_ptr<Implementation> data_;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_ACM_GENERATE_ALLOWED_COLLISION_MATRIX_WIDGET_H
