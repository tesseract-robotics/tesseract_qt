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
#include <tesseract_qt/planning/task_composer_node_info_standard_item.h>
#include <tesseract_qt/planning/task_composer_standard_item_utils.h>

#include <tesseract_qt/command_language/composite_instruction_standard_item.h>
#include <tesseract_qt/command_language/null_instruction_standard_item.h>
#include <tesseract_qt/command_language/instruction_standard_item.h>

#include <tesseract_qt/scene_graph/scene_state_standard_item.h>

#include <tesseract_qt/environment/environment_commands_standard_item.h>

#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_task_composer/task_composer_node_info.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/poly/instruction_poly.h>

#include <boost/uuid/uuid_io.hpp>

namespace tesseract_gui
{
TaskComposerNodeInfoStandardItem::TaskComposerNodeInfoStandardItem(const tesseract_planning::TaskComposerNodeInfo& info)
  : QStandardItem(icons::getUnknownIcon(), "Task Composer Node Info")
{
  ctor(info);
}

TaskComposerNodeInfoStandardItem::TaskComposerNodeInfoStandardItem(const QString& text,
                                                                   const tesseract_planning::TaskComposerNodeInfo& info)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(info);
}

TaskComposerNodeInfoStandardItem::TaskComposerNodeInfoStandardItem(const QIcon& icon,
                                                                   const QString& text,
                                                                   const tesseract_planning::TaskComposerNodeInfo& info)
  : QStandardItem(icon, text)
{
  ctor(info);
}

int TaskComposerNodeInfoStandardItem::type() const
{
  return static_cast<int>(StandardItemType::MP_TASK_COMPOSER_NODE_INFO);
}

void TaskComposerNodeInfoStandardItem::ctor(const tesseract_planning::TaskComposerNodeInfo& info)
{
  appendRow(createStandardItemString("name", info.name));
  appendRow(createStandardItemString("uuid", boost::uuids::to_string(info.uuid)));
  appendRow(createStandardItemInt("return_value", info.return_value));
  appendRow(createStandardItemString("message", info.message));
  appendRow(createStandardItemFloat("elapsed_time", info.elapsed_time));

  if (info.inbound_edges.empty())
  {
    appendRow(createStandardItemString("inbound_edges", "Empty"));
  }
  else
  {
    auto* inbound_edges = new QStandardItem(icons::getSetIcon(), "inbound_edges");
    for (const auto& key : info.inbound_edges)
      inbound_edges->appendRow(createStandardItemString("uuid", boost::uuids::to_string(key)));
    appendRow(inbound_edges);
  }

  if (info.outbound_edges.empty())
  {
    appendRow(createStandardItemString("outbound_edges", "Empty"));
  }
  else
  {
    auto* outbound_edges = new QStandardItem(icons::getSetIcon(), "outbound_edges");
    for (const auto& key : info.outbound_edges)
      outbound_edges->appendRow(createStandardItemString("uuid", boost::uuids::to_string(key)));
    appendRow(outbound_edges);
  }

  if (info.input_keys.empty())
  {
    appendRow(createStandardItemString("input_keys", "Empty"));
  }
  else
  {
    auto* input_keys = new QStandardItem(icons::getSetIcon(), "input_keys");
    for (const auto& key : info.input_keys)
      input_keys->appendRow(createStandardItemString("key", key));
    appendRow(input_keys);
  }

  if (info.output_keys.empty())
  {
    appendRow(createStandardItemString("output_keys", "Empty"));
  }
  else
  {
    auto* output_keys = new QStandardItem(icons::getSetIcon(), "output_keys");
    for (const auto& key : info.output_keys)
      output_keys->appendRow(createStandardItemString("key", key));
    appendRow(output_keys);
  }

  appendRow(createStandardItemAnyPoly("results", info.results));
}
}  // namespace tesseract_gui
