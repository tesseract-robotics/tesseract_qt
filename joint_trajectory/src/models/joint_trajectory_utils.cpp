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

#include <tesseract_qt/joint_trajectory/models/joint_trajectory_utils.h>

#include <tesseract_qt/common/events/joint_trajectory_events.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/joint_trajectory_set.h>
#include <tesseract_qt/common/cereal_serialization.h>

#include <tesseract_command_language/poly/instruction_poly.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/utils.h>

#include <tesseract_environment/cereal_serialization.h>
#include <tesseract_command_language/cereal_serialization.h>

#include <tesseract_common/serialization.h>

#include <QApplication>
#include <QString>
#include <QDir>

namespace tesseract_gui
{
bool openJointTrajectorySet(const std::shared_ptr<const ComponentInfo>& component_info,
                            const QString& filename,
                            const QString& suffix)
{
  QFileInfo file_info(filename);
  tesseract_common::JointTrajectorySet joint_trajectory_set;
  if (suffix == "jtsx" && file_info.suffix() == "jtsx")
  {
    auto jts = tesseract_common::Serialization::fromArchiveFileXML<tesseract_common::JointTrajectorySet>(
        filename.toStdString());

    events::JointTrajectoryAdd event(component_info, std::move(jts));
    QApplication::sendEvent(qApp, &event);
    return true;
  }

  if (suffix == "jtsb" && file_info.suffix() == "jtsb")
  {
    auto jts = tesseract_common::Serialization::fromArchiveFileBinary<tesseract_common::JointTrajectorySet>(
        filename.toStdString());
    events::JointTrajectoryAdd event(component_info, std::move(jts));
    QApplication::sendEvent(qApp, &event);
    return true;
  }

  if (suffix == "cpix" && file_info.suffix() == "cpix")
  {
    auto cpi =
        tesseract_common::Serialization::fromArchiveFileXML<tesseract_planning::InstructionPoly>(filename.toStdString())
            .as<tesseract_planning::CompositeInstruction>();
    tesseract_common::JointTrajectory jt = tesseract_planning::toJointTrajectory(cpi);
    if (jt.empty())
      return false;

    std::unordered_map<std::string, double> initial_state;
    for (std::size_t i = 0; i < jt.states.front().joint_names.size(); ++i)
      initial_state[jt.states.front().joint_names[i]] = jt.states.front().position[i];

    tesseract_common::JointTrajectorySet jts(initial_state, jt.description);
    jts.appendJointTrajectory(jt);

    events::JointTrajectoryAdd event(component_info, std::move(jts));
    QApplication::sendEvent(qApp, &event);
    return true;
  }

  if (suffix == "cpib" && file_info.suffix() == "cpib")
  {
    auto cpi = tesseract_common::Serialization::fromArchiveFileBinary<tesseract_planning::InstructionPoly>(
                   filename.toStdString())
                   .as<tesseract_planning::CompositeInstruction>();
    tesseract_common::JointTrajectory jt = tesseract_planning::toJointTrajectory(cpi);
    if (jt.empty())
      return false;

    std::unordered_map<std::string, double> initial_state;
    for (std::size_t i = 0; i < jt.states.front().joint_names.size(); ++i)
      initial_state[jt.states.front().joint_names[i]] = jt.states.front().position[i];

    tesseract_common::JointTrajectorySet jts(initial_state, jt.description);
    events::JointTrajectoryAdd event(component_info, std::move(jts));
    QApplication::sendEvent(qApp, &event);
    return true;
  }

  return false;
}

bool saveJointTrajectorySet(const tesseract_common::JointTrajectorySet& jts, QString filename, const QString& suffix)
{
  QFileInfo file_info(filename);
  if (suffix == "jtsx")
  {
    if (file_info.suffix() != "jtsx")
      filename = file_info.absolutePath() + QDir::separator() + file_info.baseName() + ".jtsx";

    tesseract_common::Serialization::toArchiveFileXML<tesseract_common::JointTrajectorySet>(jts,
                                                                                            filename.toStdString());
    return true;
  }

  if (suffix == "jtsb")
  {
    if (file_info.suffix() != "jtsb")
      filename = file_info.absolutePath() + QDir::separator() + file_info.baseName() + ".jtsb";

    tesseract_common::Serialization::toArchiveFileBinary<tesseract_common::JointTrajectorySet>(jts,
                                                                                               filename.toStdString());
    return true;
  }

  return false;
}

}  // namespace tesseract_gui
