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
#ifndef TESSERACT_GUI_COMMON_STATUS_LOG_WIDGET_H
#define TESSERACT_GUI_COMMON_STATUS_LOG_WIDGET_H

#ifndef Q_MOC_RUN
#include <memory>
#include <QWidget>
#endif

class QItemSelectionModel;

namespace tesseract_gui
{
class StatusLogModel;
class StatusLogWidget : public QWidget
{
  Q_OBJECT

public:
  explicit StatusLogWidget(QWidget* parent = nullptr);
  ~StatusLogWidget() override;

  void setModel(std::shared_ptr<StatusLogModel> model);
  std::shared_ptr<StatusLogModel> getModel();
  std::shared_ptr<const StatusLogModel> getModel() const;

private Q_SLOTS:
  void onCurrentRowChanged(const QModelIndex& current, const QModelIndex& /*previous*/);

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_GUI_COMMON_STATUS_LOG_WIDGET_H
