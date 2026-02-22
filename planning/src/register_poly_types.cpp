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

#include <tesseract_qt/planning/register_poly_types.h>
#include <tesseract_qt/planning/models/task_composer_data_storage_standard_item.h>
#include <tesseract_qt/common/contact_results_types.h>
#include <tesseract_qt/common/factories/any_poly_standard_item_factory.h>
#include <tesseract_qt/common/factories/instruction_poly_standard_item_factory.h>
#include <tesseract_qt/collision/models/contact_result_map_standard_item.h>
#include <tesseract_qt/collision/models/contact_result_map_vector_standard_item.h>
#include <tesseract_qt/environment/models/environment_standard_item.h>
#include <tesseract_qt/command_language/models/composite_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/set_analog_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/set_digital_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/set_tool_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/timer_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/wait_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/composite_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/move_instruction_standard_item.h>

#include <tesseract_environment/environment.h>

#include <tesseract_command_language/poly/instruction_poly.h>
#include <tesseract_command_language/poly/move_instruction_poly.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/set_analog_instruction.h>
#include <tesseract_command_language/set_digital_instruction.h>
#include <tesseract_command_language/set_tool_instruction.h>
#include <tesseract_command_language/timer_instruction.h>
#include <tesseract_command_language/wait_instruction.h>
#include <tesseract_command_language/instruction_type.h>

namespace tesseract::gui
{
void registerCommonAnyPolyTypes()
{
  {  // tesseract::environment::Environment
    using Type = tesseract::environment::Environment;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      const auto& data = any_poly.as<Type>();
      return { new EnvironmentStandardItem(data) };
    };
    AnyPolyStandardItemManager::registerFactory<Type>(fn);
  }

  {  // std::shared_ptr<tesseract::environment::Environment>
    using Type = std::shared_ptr<tesseract::environment::Environment>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      const auto& data = any_poly.as<Type>();
      return { new EnvironmentStandardItem(*data) };
    };
    AnyPolyStandardItemManager::registerFactory<Type>(fn);
  }

  {  // std::shared_ptr<const tesseract::environment::Environment>
    using Type = std::shared_ptr<const tesseract::environment::Environment>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      const auto& data = any_poly.as<Type>();
      return { new EnvironmentStandardItem(*data) };
    };
    AnyPolyStandardItemManager::registerFactory<Type>(fn);
  }

  {  // std::shared_ptr<tesseract::task_composer::TaskComposerDataStorage>
    using Type = std::shared_ptr<tesseract::task_composer::TaskComposerDataStorage>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      const auto& data = any_poly.as<Type>();
      return { new TaskComposerDataStorageStandardItem(*data) };
    };
    AnyPolyStandardItemManager::registerFactory<Type>(fn);
  }

  {  // tesseract::task_composer::CompositeInstruction
    using Type = tesseract::command_language::CompositeInstruction;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      const auto& data = any_poly.as<Type>();
      return { new CompositeInstructionStandardItem(data) };
    };
    AnyPolyStandardItemManager::registerFactory<Type>(fn);
  }

  {  // tesseract::collision::ContactResultMap
    using Type = tesseract::collision::ContactResultMap;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      const auto& data = any_poly.as<Type>();
      return { new ContactResultMapStandardItem(convert(data)) };
    };
    AnyPolyStandardItemManager::registerFactory<Type>(fn);
  }

  {  // std::vector<tesseract::collision::ContactResultMap>
    using Type = std::vector<tesseract::collision::ContactResultMap>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      const auto& data = any_poly.as<Type>();
      return { new ContactResultMapVectorStandardItem(convert(data)) };
    };
    AnyPolyStandardItemManager::registerFactory<Type>(fn);
  }
}
void registerCommonInstructionPolyTypes()
{
  {  // tesseract::task_composer::CompositeInstruction
    using Type = tesseract::command_language::CompositeInstruction;
    auto fn = [](const tesseract::command_language::InstructionPoly& instruction_poly) -> QList<QStandardItem*> {
      const auto& data = instruction_poly.as<Type>();
      return { new CompositeInstructionStandardItem(data) };
    };
    InstructionPolyStandardItemManager::registerFactory<Type>(fn);
  }

  {  // tesseract::task_composer::MoveInstructionPoly
    using Type = tesseract::command_language::MoveInstructionPoly;
    auto fn = [](const tesseract::command_language::InstructionPoly& instruction_poly) -> QList<QStandardItem*> {
      const auto& data = instruction_poly.as<Type>();
      return { new MoveInstructionStandardItem(data) };
    };
    InstructionPolyStandardItemManager::registerFactory<Type>(fn);
  }

  {  // tesseract::task_composer::SetAnalogInstruction
    using Type = tesseract::command_language::SetAnalogInstruction;
    auto fn = [](const tesseract::command_language::InstructionPoly& instruction_poly) -> QList<QStandardItem*> {
      const auto& data = instruction_poly.as<Type>();
      return { new SetAnalogInstructionStandardItem(data) };
    };
    InstructionPolyStandardItemManager::registerFactory<Type>(fn);
  }

  {  // tesseract::task_composer::SetDigitalInstruction
    using Type = tesseract::command_language::SetDigitalInstruction;
    auto fn = [](const tesseract::command_language::InstructionPoly& instruction_poly) -> QList<QStandardItem*> {
      const auto& data = instruction_poly.as<Type>();
      return { new SetDigitalInstructionStandardItem(data) };
    };
    InstructionPolyStandardItemManager::registerFactory<Type>(fn);
  }

  {  // tesseract::task_composer::SetToolInstruction
    using Type = tesseract::command_language::SetToolInstruction;
    auto fn = [](const tesseract::command_language::InstructionPoly& instruction_poly) -> QList<QStandardItem*> {
      const auto& data = instruction_poly.as<Type>();
      return { new SetToolInstructionStandardItem(data) };
    };
    InstructionPolyStandardItemManager::registerFactory<Type>(fn);
  }

  {  // tesseract::task_composer::TimerInstruction
    using Type = tesseract::command_language::TimerInstruction;
    auto fn = [](const tesseract::command_language::InstructionPoly& instruction_poly) -> QList<QStandardItem*> {
      const auto& data = instruction_poly.as<Type>();
      return { new TimerInstructionStandardItem(data) };
    };
    InstructionPolyStandardItemManager::registerFactory<Type>(fn);
  }

  {  // tesseract::task_composer::WaitInstruction
    using Type = tesseract::command_language::WaitInstruction;
    auto fn = [](const tesseract::command_language::InstructionPoly& instruction_poly) -> QList<QStandardItem*> {
      const auto& data = instruction_poly.as<Type>();
      return { new WaitInstructionStandardItem(data) };
    };
    InstructionPolyStandardItemManager::registerFactory<Type>(fn);
  }
}
}  // namespace tesseract::gui
