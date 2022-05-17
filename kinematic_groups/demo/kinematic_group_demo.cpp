/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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
#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <QApplication>
#include <QStandardItemModel>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/kinematic_groups/kinematic_groups_editor_widget.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);

  QStringList joint_names{ "joint_1", "joint_2", "joint_3", "joint_4", "joint_5", "joint_6" };
  QStringList link_names{ "link_1", "link_2", "link_3", "link_4", "link_5", "link_6" };

  tesseract_gui::KinematicGroupsModel model;
  tesseract_gui::KinematicGroupsEditorWidget widget(joint_names, link_names);
  widget.setModel(&model);
  widget.show();

  return app.exec();
}
