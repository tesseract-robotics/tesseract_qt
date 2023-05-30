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
#ifndef TESSERACT_QT_TOOL_PATH_TOOL_PATH_FILE_DIALOG_H
#define TESSERACT_QT_TOOL_PATH_TOOL_PATH_FILE_DIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <memory>

namespace Ui
{
class ToolPathFileDialog;
}

namespace tesseract_gui
{
struct ComponentInfo;
class ToolPathFileDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ToolPathFileDialog(QFileDialog::AcceptMode accept_mode = QFileDialog::AcceptOpen, QWidget* parent = nullptr);
  explicit ToolPathFileDialog(std::shared_ptr<const ComponentInfo> component_info,
                              QFileDialog::AcceptMode accept_mode = QFileDialog::AcceptOpen,
                              QWidget* parent = nullptr);
  ~ToolPathFileDialog();

  void setComponentInfo(std::shared_ptr<const ComponentInfo> component_info);
  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

  void setAcceptMode(QFileDialog::AcceptMode accept_mode);
  QString getLinkName() const;
  QString getFilePath() const;

public Q_SLOTS:
  void onBrowseClicked();

private:
  std::unique_ptr<Ui::ToolPathFileDialog> ui_;
  std::shared_ptr<const ComponentInfo> component_info_;
  QFileDialog::AcceptMode accept_mode_;
  QString default_directory_;

  void showEvent(QShowEvent* e) override;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_TOOL_PATH_TOOL_PATH_FILE_DIALOG_H
