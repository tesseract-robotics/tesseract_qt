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
#ifndef TESSERACT_QT_SRDF_EDITOR_WIDGET_H
#define TESSERACT_QT_SRDF_EDITOR_WIDGET_H

#include <QWidget>
#include <memory>

namespace tesseract_common
{
class ResourceLocator;
}

namespace tesseract_scene_graph
{
class Joint;
}

namespace tesseract_environment
{
class Environment;
}

namespace Ui
{
class SRDFEditorWidget;
}

namespace tesseract_gui
{
struct ComponentInfo;
class SRDFEditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit SRDFEditorWidget(std::shared_ptr<tesseract_common::ResourceLocator> locator, QWidget* parent = nullptr);
  explicit SRDFEditorWidget(std::shared_ptr<const ComponentInfo> component_info,
                            std::shared_ptr<tesseract_common::ResourceLocator> locator,
                            QWidget* parent = nullptr);
  ~SRDFEditorWidget();

Q_SIGNALS:
  void showStatusMessage(const QString& message, int timeout);

public Q_SLOTS:
  /**
   * @brief Callback trigged when the load button is pressed.
   * @param[in] urdf_filepath The urdf file path.
   * @param[in] srdf_filepath The srdf file path.
   */
  void onLoad(const QString& urdf_filepath, const QString& srdf_filepath);
  void onSave(const QString& srdf_filepath);

  void onBrowseURDFLoadClicked();
  void onBrowseSRDFLoadClicked();
  void onBrowseSRDFSaveClicked();

protected Q_SLOTS:
  void onLoad();
  void onSave();

private:
  std::unique_ptr<Ui::SRDFEditorWidget> ui_;

  struct Implementation;
  std::unique_ptr<Implementation> data_;

  void enablePages(bool enable);
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_SRDF_EDITOR_WIDGET_H
