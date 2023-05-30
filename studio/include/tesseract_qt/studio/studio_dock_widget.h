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
#ifndef TESSERACT_QT_STUDIO_STUDIO_DOCK_WIDGET_H
#define TESSERACT_QT_STUDIO_STUDIO_DOCK_WIDGET_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <yaml-cpp/yaml.h>
#include <ads_globals.h>
#include <DockAreaWidget.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

namespace tesseract_gui
{
class StudioDockWidget : public ads::CDockWidget
{
  Q_OBJECT
public:
  /**
   * This constructor creates a dock widget with the given title.
   * The title is the text that is shown in the window title when the dock
   * widget is floating and it is the title that is shown in the titlebar
   * or the tab of this dock widget if it is tabified.
   * The object name of the dock widget is also set to the title. The
   * object name is required by the dock manager to properly save and restore
   * the state of the dock widget. That means, the title needs to be unique.
   * If your title is not unique or if you would like to change the title
   * during runtime, you need to set a unique object name explicitely
   * by calling setObjectName() after construction.
   * Use the layoutFlags to configure the layout of the dock widget.
   */
  StudioDockWidget(const QString& title, QWidget* parent = nullptr);

  /**
   * @brief Get the class name
   * @return The class name
   */
  virtual std::string getFactoryClassName() const = 0;

  /**
   * @brief Load the plugin config
   * @param config The yaml config
   */
  virtual void loadConfig(const YAML::Node& config) = 0;

  /**
   * @brief Get the plugin config
   * @return The yaml config
   */
  virtual YAML::Node getConfig() const = 0;

  /**
   * @brief Set name associated with the plugin
   * @param name The name to assign to the plugin
   */
  virtual void setName(const QString& name);

  /**
   * @brief Get name associated with the plugin
   * @return The name assigned to the plugin
   */
  virtual QString getName() const;

  /**
   * @brief Indicate if this widget should be assinged as the central widget
   * @return True if it should be assigned as central widget, otherwise false
   */
  virtual bool assignAsCentralWidget() const;

  /**
   * @brief Check if initialized
   * @return True if initialized, otherwise false
   */
  virtual bool isInitialized() const;

public Q_SLOTS:
  virtual void onInitialize();

protected:
  bool assign_as_central_widget_{ false };
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_STUDIO_STUDIO_DOCK_WIDGET_H
