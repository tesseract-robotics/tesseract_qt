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
#include <QStringListModel>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/kinematic_groups/groups_joint_states_editor_widget.h>
#include <tesseract_qt/kinematic_groups/group_joint_states_model.h>

#include <tesseract_urdf/urdf_parser.h>
#include <tesseract_support/tesseract_support_resource_locator.h>

tesseract_scene_graph::SceneGraph::UPtr sg;

std::vector<tesseract_scene_graph::Joint::ConstPtr> getGroupJoints(QString group_name)
{
  std::vector<tesseract_scene_graph::Joint::ConstPtr> joints;
  if (group_name == "group1")
  {
    joints.push_back(sg->getJoint("joint_a1"));
    joints.push_back(sg->getJoint("joint_a2"));
    joints.push_back(sg->getJoint("joint_a3"));
    joints.push_back(sg->getJoint("joint_a4"));
    joints.push_back(sg->getJoint("joint_a5"));
    joints.push_back(sg->getJoint("joint_a6"));
    joints.push_back(sg->getJoint("joint_a7"));
  }
  else if (group_name == "group2")
  {
    joints.push_back(sg->getJoint("joint_a1"));
    joints.push_back(sg->getJoint("joint_a4"));
    joints.push_back(sg->getJoint("joint_a7"));
  }
  return joints;
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);

  // Load Scene Graph
  std::string path = std::string(TESSERACT_SUPPORT_DIR) + "/urdf/lbr_iiwa_14_r820.urdf";
  tesseract_common::TesseractSupportResourceLocator locator;
  sg = tesseract_urdf::parseURDFFile(path, locator);

  tesseract_gui::GroupJointStatesModel model;
  QStringList list{ "group1", "group2" };
  QStringListModel group_names;
  group_names.setStringList(list);
  tesseract_gui::GroupJointsRetriever func = getGroupJoints;
  tesseract_gui::GroupsJointStatesEditorWidget widget(func);
  widget.setModel(&model);
  widget.setGroupNamesModel(&group_names);
  widget.show();

  return app.exec();
}
