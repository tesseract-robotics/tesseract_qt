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
#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <QApplication>
#include <QDebug>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/joint_state_slider/joint_state_slider_widget.h>

#include <tesseract_urdf/urdf_parser.h>
#include <tesseract_support/tesseract_support_resource_locator.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  // Load Scene Graph
  std::string path = std::string(TESSERACT_SUPPORT_DIR) + "/urdf/lbr_iiwa_14_r820.urdf";
  tesseract_common::TesseractSupportResourceLocator locator;
  tesseract_scene_graph::SceneGraph::UPtr sg = tesseract_urdf::parseURDFFile(path, locator);
  std::vector<tesseract_scene_graph::Joint::ConstPtr> joints;
  for (const auto& joint : sg->getJoints())
  {
    if (joint->type == tesseract_scene_graph::JointType::REVOLUTE ||
        joint->type == tesseract_scene_graph::JointType::CONTINUOUS ||
        joint->type == tesseract_scene_graph::JointType::PRISMATIC)
      joints.push_back(joint);
  }
  tesseract_gui::JointStateSliderWidget widget;
  widget.setJoints(joints);
  widget.show();

  return app.exec();
}
