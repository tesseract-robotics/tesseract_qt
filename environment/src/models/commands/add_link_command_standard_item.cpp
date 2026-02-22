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
#include <tesseract_qt/environment/models/commands/add_link_command_standard_item.h>
#include <tesseract_qt/scene_graph/models/link_standard_item.h>
#include <tesseract_qt/scene_graph/models/joint_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_environment/commands/add_link_command.h>
#include <tesseract_scene_graph/link.h>
#include <tesseract_scene_graph/joint.h>

namespace tesseract::gui
{
AddLinkCommandStandardItem::AddLinkCommandStandardItem(
    std::shared_ptr<const tesseract::environment::AddLinkCommand> command)
  : QStandardItem(icons::getCommandEntryIcon(), "Add Link"), command(std::move(command))
{
  ctor();
}

AddLinkCommandStandardItem::AddLinkCommandStandardItem(
    const QString& text,
    std::shared_ptr<const tesseract::environment::AddLinkCommand> command)
  : QStandardItem(icons::getCommandEntryIcon(), text), command(std::move(command))
{
  ctor();
}

AddLinkCommandStandardItem::AddLinkCommandStandardItem(
    const QIcon& icon,
    const QString& text,
    std::shared_ptr<const tesseract::environment::AddLinkCommand> command)
  : QStandardItem(icon, text), command(std::move(command))
{
  ctor();
}

int AddLinkCommandStandardItem::type() const { return static_cast<int>(StandardItemType::ENV_COMMAND_ADD_LINK); }

void AddLinkCommandStandardItem::ctor()
{
  appendRow(new LinkStandardItem(std::make_shared<tesseract::scene_graph::Link>(command->getLink()->clone()), false));

  if (command->getJoint() != nullptr)
    appendRow(new JointStandardItem(std::make_shared<tesseract::scene_graph::Joint>(command->getJoint()->clone())));
  else
    appendRow(createStandardItemString(icons::getJointIcon(), "Joint", "NULL"));

  appendRow(createStandardItemString("replace allowed", (command->replaceAllowed()) ? "True" : "False"));
}
}  // namespace tesseract::gui
