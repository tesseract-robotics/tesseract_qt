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

#include <tesseract_qt/environment/models/environment_standard_item.h>
#include <tesseract_qt/environment/models/environment_commands_standard_item.h>
#include <tesseract_qt/scene_graph/models/scene_state_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_environment/environment.h>
#include <tesseract_scene_graph/scene_state.h>

namespace tesseract_gui
{
EnvironmentStandardItem::EnvironmentStandardItem() : QStandardItem(icons::getTesseractIcon(), "Environment") {}

EnvironmentStandardItem::EnvironmentStandardItem(const tesseract_environment::Environment& env)
  : QStandardItem(icons::getTesseractIcon(), "Environment")
{
  ctor(env);
}

EnvironmentStandardItem::EnvironmentStandardItem(const QString& text, const tesseract_environment::Environment& env)
  : QStandardItem(icons::getTesseractIcon(), text)
{
  ctor(env);
}

EnvironmentStandardItem::EnvironmentStandardItem(const QIcon& icon,
                                                 const QString& text,
                                                 const tesseract_environment::Environment& env)
  : QStandardItem(icon, text)
{
  ctor(env);
}

int EnvironmentStandardItem::type() const { return static_cast<int>(StandardItemType::ENVIRONMENT); }

void EnvironmentStandardItem::ctor(const tesseract_environment::Environment& env)
{
  appendRow(createStandardItemString("name", env.getName()));
  appendRow(createStandardItemInt("revision", env.getRevision()));
  appendRow(createStandardItemString("root_link", env.getRootLinkName()));
  appendRow(new EnvironmentCommandsStandardItem(env.getCommandHistory()));
  appendRow(new SceneStateStandardItem(env.getState()));
}

}  // namespace tesseract_gui
