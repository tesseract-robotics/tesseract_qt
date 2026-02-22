/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2025 Levi Armstrong <levi.armstrong@gmail.com>
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
#ifndef TESSERACT_QT_COMMON_COLLAPSIBLE_GROUP_BOX_H
#define TESSERACT_QT_COMMON_COLLAPSIBLE_GROUP_BOX_H

#include <qwidget.h>
#include <qgroupbox.h>
#include <qtoolbutton.h>

namespace tesseract::gui
{
class CollapsibleGroupBox : public QGroupBox
{
  Q_OBJECT
public:
  explicit CollapsibleGroupBox(QWidget* parent = nullptr);
  explicit CollapsibleGroupBox(const QString& title, QWidget* parent = nullptr);

  void setCollapsed(bool collapse);

Q_SIGNALS:
  void collapsed(bool collapsed);

protected:
  void resizeEvent(QResizeEvent* event) override;

private Q_SLOTS:
  void onToggled(bool checked);

private:
  QToolButton* toggle_button_;
  void updateToggleButtonPosition();
};

}  // namespace tesseract::gui

#endif  // TESSERACT_QT_COMMON_COLLAPSIBLE_GROUP_BOX_H
