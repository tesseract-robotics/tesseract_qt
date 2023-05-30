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
#ifndef TESSERACT_GUI_COMMON_LOAD_ENVIRONMENT_DIALOG_H
#define TESSERACT_GUI_COMMON_LOAD_ENVIRONMENT_DIALOG_H

#include <memory>
#include <QDialog>

namespace Ui
{
class LoadEnvironmentDialog;
}

namespace tesseract_common
{
class ResourceLocator;
}
namespace tesseract_gui
{
struct ComponentInfo;
class LoadEnvironmentDialog : public QDialog
{
  Q_OBJECT

public:
  explicit LoadEnvironmentDialog(QWidget* parent = nullptr);
  explicit LoadEnvironmentDialog(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent = nullptr);
  ~LoadEnvironmentDialog();

  void setResourceLocator(std::shared_ptr<tesseract_common::ResourceLocator> resource_locator);

public Q_SLOTS:
  void onAccepted();

private:
  std::unique_ptr<Ui::LoadEnvironmentDialog> ui_;
  struct Implementation;
  std::unique_ptr<Implementation> data_;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_GUI_COMMON_LOAD_ENVIRONMENT_DIALOG_H
