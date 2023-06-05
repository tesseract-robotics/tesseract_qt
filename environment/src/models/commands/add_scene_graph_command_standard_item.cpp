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
#include <tesseract_qt/environment/models/commands/add_scene_graph_command_standard_item.h>
#include <tesseract_qt/scene_graph/models/scene_graph_standard_item.h>
#include <tesseract_qt/scene_graph/models/joint_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
AddSceneGraphCommandStandardItem::AddSceneGraphCommandStandardItem(
    tesseract_environment::AddSceneGraphCommand::ConstPtr command)
  : QStandardItem(icons::getCommandEntryIcon(), "Add Scene Graph"), command(std::move(command))
{
  ctor();
}

AddSceneGraphCommandStandardItem::AddSceneGraphCommandStandardItem(
    const QString& text,
    tesseract_environment::AddSceneGraphCommand::ConstPtr command)
  : QStandardItem(icons::getCommandEntryIcon(), text), command(std::move(command))
{
  ctor();
}

AddSceneGraphCommandStandardItem::AddSceneGraphCommandStandardItem(
    const QIcon& icon,
    const QString& text,
    tesseract_environment::AddSceneGraphCommand::ConstPtr command)
  : QStandardItem(icon, text), command(std::move(command))
{
  ctor();
}

int AddSceneGraphCommandStandardItem::type() const
{
  return static_cast<int>(StandardItemType::ENV_COMMAND_ADD_SCENE_GRAPH);
}

void AddSceneGraphCommandStandardItem::ctor()
{
  appendRow(createStandardItemString("prefix", command->getPrefix()));

  if (command->getJoint() != nullptr)
    appendRow(new JointStandardItem(std::make_shared<tesseract_scene_graph::Joint>(command->getJoint()->clone())));
  else
    appendRow(createStandardItemString(icons::getJointIcon(), "Joint", "NULL"));

  auto* sg = new SceneGraphStandardItem(false);
  sg->setSceneGraph(*command->getSceneGraph());
  appendRow(sg);
}
}  // namespace tesseract_gui
