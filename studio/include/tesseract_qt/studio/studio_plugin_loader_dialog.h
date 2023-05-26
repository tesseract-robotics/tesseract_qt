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
#ifndef TESSERACT_QT_STUDIO_STUDIO_PLUGIN_LOADER_WIDGET_H
#define TESSERACT_QT_STUDIO_STUDIO_PLUGIN_LOADER_WIDGET_H

#include <QDialog>
#include <memory>

namespace Ui
{
class StudioPluginLoaderDialog;
}

namespace tesseract_gui
{
class StudioPluginFactory;
class StudioPluginConfigWidget;
class Studio;

class StudioPluginLoaderDialog : public QDialog
{
  Q_OBJECT

public:
  explicit StudioPluginLoaderDialog(Studio* app);
  ~StudioPluginLoaderDialog();

protected:
  void showEvent(QShowEvent* event) override;

private Q_SLOTS:
  void showSearchPathContextMenu(const QPoint& pos);
  void showSearchLibraryContextMenu(const QPoint& pos);
  void addPluginWidget();
  void refreshSearchPathsAndLibraries();
  void generateUniqueName();

private:
  struct Implementation;
  std::unique_ptr<Ui::StudioPluginLoaderDialog> ui;
  std::unique_ptr<Implementation> data_;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_STUDIO_STUDIO_PLUGIN_LOADER_WIDGET_H
