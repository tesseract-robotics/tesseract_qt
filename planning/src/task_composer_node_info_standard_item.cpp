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
#include <tesseract_qt/planning/task_composer_keys_standard_item.h>
#include <tesseract_qt/planning/task_composer_data_storage_standard_item.h>

#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_task_composer/core/task_composer_node_info.h>

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
  appendRow(createStandardItemString("ns", info.ns));
  appendRow(createStandardItemString("uuid", boost::uuids::to_string(info.uuid)));
  appendRow(createStandardItemString("parent_uuid", boost::uuids::to_string(info.parent_uuid)));

  if (info.type == tesseract_planning::TaskComposerNodeType::NODE)
    appendRow(createStandardItemString("type", "NODE"));
  else if (info.type == tesseract_planning::TaskComposerNodeType::TASK)
    appendRow(createStandardItemString("type", "TASK"));
  else if (info.type == tesseract_planning::TaskComposerNodeType::PIPELINE)
    appendRow(createStandardItemString("type", "PIPELINE"));
  else if (info.type == tesseract_planning::TaskComposerNodeType::GRAPH)
    appendRow(createStandardItemString("type", "GRAPH"));

  appendRow(createStandardItemUnsigned("type_hash_code", info.type_hash_code));
  appendRow(createStandardItemBool("conditional", info.conditional));
  appendRow(createStandardItemInt("abort_terminal", info.abort_terminal));
  appendRow(createStandardItemInt("return_value", info.return_value));
  appendRow(createStandardItemInt("status_code", info.status_code));
  appendRow(createStandardItemString("status_message", info.status_message));
  appendRow(createStandardItemFloat("elapsed_time", info.elapsed_time));
  appendRow(createStandardItemString("color", info.color));

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

  if (info.terminals.empty())
  {
    appendRow(createStandardItemString("terminals", "Empty"));
  }
  else
  {
    auto* terminals = new QStandardItem(icons::getSetIcon(), "terminals");
    for (const auto& key : info.terminals)
      terminals->appendRow(createStandardItemString("uuid", boost::uuids::to_string(key)));
    appendRow(terminals);
  }

  if (info.input_keys.empty())
    appendRow(createStandardItemString("input_keys", "Empty"));
  else
    appendRow(new TaskComposerKeysStandardItem("input_keys", info.input_keys));  // NOLINT

  if (info.output_keys.empty())
    appendRow(createStandardItemString("output_keys", "Empty"));
  else
    appendRow(new TaskComposerKeysStandardItem("output_keys", info.output_keys));  // NOLINT

  appendRow(new TaskComposerDataStorageStandardItem("data_storage", info.data_storage));  // NOLINT
}
}  // namespace tesseract_gui
