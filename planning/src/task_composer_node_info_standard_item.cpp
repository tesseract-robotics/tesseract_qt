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

#include <tesseract_qt/command_language/models/composite_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/null_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/instruction_standard_item.h>

#include <tesseract_qt/scene_graph/models/scene_state_standard_item.h>

#include <tesseract_qt/environment/models/environment_commands_standard_item.h>

#include <tesseract_qt/collision/models/contact_result_vector_standard_item.h>

#include <tesseract_qt/common/contact_results_types.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_task_composer/core/task_composer_node_info.h>
#include <tesseract_task_composer/planning/nodes/fix_state_collision_task.h>
#include <tesseract_task_composer/planning/nodes/discrete_contact_check_task.h>
#include <tesseract_task_composer/planning/nodes/continuous_contact_check_task.h>
#include <tesseract_task_composer/planning/nodes/time_optimal_parameterization_task.h>

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
  appendRow(createStandardItemInt("status_code", info.status_code));
  appendRow(createStandardItemString("status_message", info.status_message));
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

  ////////////////////////////////
  /// Add data for known types ///
  ////////////////////////////////

  auto addContactResults = [this](const std::vector<tesseract_collision::ContactResultMap>& input) {
    std::vector<ContactResultMap> contact_results;
    contact_results.reserve(input.size());
    for (const auto& state_results : input)
    {
      // Convert to tracked objects
      tesseract_gui::ContactResultMap tracked_object;
      for (const auto& contact : state_results)
      {
        tesseract_gui::ContactResultVector crv;
        for (const auto& result : contact.second)
          crv().emplace_back(tesseract_gui::ContactResult(result));

        tracked_object[contact.first] = crv;
      }

      contact_results.push_back(tracked_object);
    }

    QStandardItem* crsv_item = new QStandardItem(icons::getSetIcon(), "contact_results");
    for (std::size_t i = 0; i < contact_results.size(); ++i)
    {
      QStandardItem* state_item = new QStandardItem(icons::getSetIcon(), QString("[%1]").arg(i));
      for (const auto& pair : contact_results[i])
      {
        auto text =
            QString("%1::%2").arg(QString::fromStdString(pair.first.first), QString::fromStdString(pair.first.second));
        auto* link_pair_item = new ContactResultVectorStandardItem(text, pair.second);
        state_item->appendRow(link_pair_item);
      }
      crsv_item->appendRow(state_item);
    }
    appendRow(crsv_item);
  };

  {  // Try FixStateCollisionTaskInfo
    const auto* derived_type = dynamic_cast<const tesseract_planning::FixStateCollisionTaskInfo*>(&info);
    if (derived_type != nullptr)
    {
      addContactResults(derived_type->contact_results);
      return;
    }
  }

  {  // Try DiscreteContactCheckTaskInfo
    const auto* derived_type = dynamic_cast<const tesseract_planning::DiscreteContactCheckTaskInfo*>(&info);
    if (derived_type != nullptr)
    {
      addContactResults(derived_type->contact_results);
      return;
    }
  }

  {  // Try ContinuousContactCheckTaskInfo
    const auto* derived_type = dynamic_cast<const tesseract_planning::ContinuousContactCheckTaskInfo*>(&info);
    if (derived_type != nullptr)
    {
      addContactResults(derived_type->contact_results);
      return;
    }
  }

  {  // Try ContinuousContactCheckTaskInfo
    const auto* derived_type = dynamic_cast<const tesseract_planning::TimeOptimalParameterizationTaskInfo*>(&info);
    if (derived_type != nullptr)
    {
      appendRow(createStandardItemFloat("max_velocity_scaling_factor", derived_type->max_velocity_scaling_factor));
      appendRow(
          createStandardItemFloat("max_acceleration_scaling_factor", derived_type->max_acceleration_scaling_factor));
      return;
    }
  }
}
}  // namespace tesseract_gui
