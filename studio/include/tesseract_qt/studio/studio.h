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
#ifndef TESSERACT_QT_STUDIO_STUDIO_H
#define TESSERACT_QT_STUDIO_STUDIO_H

#include <QMainWindow>
#include <memory>

namespace Ui
{
class Studio;
}

namespace tesseract_common
{
class PluginLoader;
struct PluginInfo;
}  // namespace tesseract_common

namespace tesseract_gui
{
class StudioPluginFactory;
class StudioDockWidget;

class Studio : public QMainWindow
{
  Q_OBJECT

public:
  explicit Studio(QWidget* parent = nullptr);
  ~Studio();

protected:
  virtual void closeEvent(QCloseEvent* event) override;

private:
  friend class StudioPluginLoaderDialog;
  struct Implementation;
  std::unique_ptr<Ui::Studio> ui;
  std::unique_ptr<Implementation> data_;

  tesseract_common::PluginLoader& getPluginLoader();
  const tesseract_common::PluginLoader& getPluginLoader() const;

  /**
   * @brief Get studio dock widget given name and plugin info
   * @details This looks for studio plugin info. If not found nullptr is returned.
   * @param name The name
   * @param plugin_info The plugin information to create task composer executor object
   */
  StudioDockWidget* createDockWidget(const QString& name, const tesseract_common::PluginInfo& plugin_info);

  /**
   * @brief Add the dock widget to the manager
   * @param dock_widget The dock widget to add
   */
  void addDockWidget(StudioDockWidget* dock_widget);
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_STUDIO_STUDIO_H
