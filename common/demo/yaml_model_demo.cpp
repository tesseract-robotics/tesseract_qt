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

#include <QApplication>

#include <tesseract_qt/common/models/yaml_model.h>
#include <QTreeView>
#include <yaml-cpp/yaml.h>
#include <iostream>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);
  std::string str = R"(config:
                         boolean: true
                         string: "input_data"
                         int: 100
                         float: 100.5
                         vector:
                           - 1
                           - 2
                           - 3)";

  YAML::Node node = YAML::Load(str);
  tesseract_gui::QYamlModel model;
  model.showIcons(true);
  model.load(node);

  YAML::Emitter emitter;
  emitter << model.yaml();
  std::cout << emitter.c_str() << std::endl;

  QTreeView widget;
  widget.setModel(&model);
  widget.show();

  return QApplication::exec();
}
