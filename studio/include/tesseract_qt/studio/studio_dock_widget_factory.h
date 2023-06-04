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
#ifndef TESSERACT_QT_STUDIO_STUDIO_DOCK_WIDGET_FACTORY_H
#define TESSERACT_QT_STUDIO_STUDIO_DOCK_WIDGET_FACTORY_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <string>
#include <map>
#include <yaml-cpp/yaml.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_common/plugin_loader.h>
#include <tesseract_qt/studio/studio_dock_widget.h>

// clang-format off
#define TESSERACT_ADD_STUDIO_PLUGIN(DERIVED_CLASS, ALIAS)                                           \
  TESSERACT_ADD_PLUGIN_SECTIONED(DERIVED_CLASS, ALIAS, Studio)
// clang-format on

namespace tesseract_gui
{
class StudioDockWidgetFactory
{
public:
  using Ptr = std::shared_ptr<StudioDockWidgetFactory>;
  using ConstPtr = std::shared_ptr<const StudioDockWidgetFactory>;

  virtual ~StudioDockWidgetFactory() = default;

  virtual StudioDockWidget* create(const QString& name) const = 0;

  static const std::string& getSectionName();

protected:
  static const std::string SECTION_NAME;
  friend class PluginLoader;
};

template <typename StudioDockWidgetType>
class StudioDockWidgetFactoryImpl : public StudioDockWidgetFactory
{
public:
  StudioDockWidget* create(const QString& name) const override { return new StudioDockWidgetType(name); }
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_STUDIO_STUDIO_DOCK_WIDGET_FACTORY_H
