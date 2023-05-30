/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2023 Levi Armstrong <levi.armstrong@gmail.com>
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
#ifndef TESSERACT_GUI_COMMON_COMPONENT_INFO_MANAGER_WIDGET_H
#define TESSERACT_GUI_COMMON_COMPONENT_INFO_MANAGER_WIDGET_H

#include <QWidget>
#include <memory>

namespace Ui
{
class ComponentInfoManagerWidget;
}

namespace tesseract_gui
{
class ComponentInfoManagerWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ComponentInfoManagerWidget(QWidget* parent = nullptr);
  ~ComponentInfoManagerWidget();

  /**
   * @brief Enable edit mode to allow user to add and remove component infos
   * @param enable If true user can add and remove component infos
   */
  void enableEditMode(bool enable);

  /** @brief Refresh the model by extracting data from ComponentInfoManager */
  void refresh();

Q_SIGNALS:
  void refreshed();

private Q_SLOTS:
  void showContextMenu(const QPoint& pos);

private:
  struct Implementation;
  std::unique_ptr<Ui::ComponentInfoManagerWidget> ui;
  std::unique_ptr<Implementation> data_;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_GUI_COMMON_COMPONENT_INFO_MANAGER_WIDGET_H
