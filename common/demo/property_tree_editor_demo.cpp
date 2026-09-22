/*
 * @copyright Copyright (C) 2026 Levi Armstrong <levi.armstrong@gmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 */
#include <tesseract_qt/common/widgets/property_tree_editor_widget.h>

#include <tesseract/common/property_tree.h>

#include <QApplication>

int main(int argc, char** argv)
{
  QApplication application(argc, argv);

  using namespace tesseract::common;
  // clang-format off
  const PropertyTree schema = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .string("name").required().label("Name").placeholder("Solver name").done()
      .boolean("enabled").defaultVal(true).done()
      .int32("iterations").defaultVal(100).minimum(1).maximum(1000).done()
      .string("mode").defaultVal("safe").enumValues({ "fast", "safe" }).done()
      .container("limits").label("Limits")
          .float64("position").minimum(0.0).done()
          .float64("velocity").minimum(0.0).done()
      .done()
      .build();
  // clang-format on

  tesseract::gui::PropertyTreeEditorWidget editor;
  editor.load(schema, YAML::Load("name: Example\nlimits:\n  position: 1.0\n  velocity: 2.0"));
  editor.resize(800, 500);
  editor.show();

  return application.exec();
}
