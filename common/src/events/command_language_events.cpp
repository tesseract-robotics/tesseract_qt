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
#include <tesseract_qt/common/events/command_language_events.h>

#include <tesseract_command_language/composite_instruction.h>

namespace tesseract_gui::events
{
CompositeInstructionClear::CompositeInstructionClear(std::shared_ptr<const ComponentInfo> component_info,
                                                     const std::string& ns)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::CL_COMPOSITE_INSTRUCTION_CLEAR)), ns_(ns)
{
}

CompositeInstructionClear::~CompositeInstructionClear() = default;

const std::string& CompositeInstructionClear::getNamespace() const { return ns_; }

//////////////////////////////////////////

struct CompositeInstructionSet::Implementation
{
  std::string ns;
  tesseract_planning::CompositeInstruction composite_instruction;
};

CompositeInstructionSet::CompositeInstructionSet(std::shared_ptr<const ComponentInfo> component_info,
                                                 const tesseract_planning::CompositeInstruction& composite_instruction,
                                                 const std::string& ns)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::CL_COMPOSITE_INSTRUCTION_SET))
  , data_(std::make_unique<Implementation>())
{
  data_->ns = ns;
  data_->composite_instruction = composite_instruction;
}

CompositeInstructionSet::~CompositeInstructionSet() = default;

const std::string& CompositeInstructionSet::getNamespace() const { return data_->ns; }
const tesseract_planning::CompositeInstruction& CompositeInstructionSet::getCompositeInstruction() const
{
  return data_->composite_instruction;
}

//////////////////////////////////////////

CompositeInstructionRemove::CompositeInstructionRemove(std::shared_ptr<const ComponentInfo> component_info,
                                                       boost::uuids::uuid uuid)
  : ComponentEventUUID(std::move(component_info), uuid, QEvent::Type(EventType::CL_COMPOSITE_INSTRUCTION_REMOVE))
{
}
CompositeInstructionRemove::CompositeInstructionRemove(std::shared_ptr<const ComponentInfo> component_info,
                                                       boost::uuids::uuid uuid,
                                                       boost::uuids::uuid child_uuid)
  : ComponentEventUUID(std::move(component_info),
                       uuid,
                       child_uuid,
                       QEvent::Type(EventType::CL_COMPOSITE_INSTRUCTION_REMOVE))
{
}
CompositeInstructionRemove::~CompositeInstructionRemove() = default;

//////////////////////////////////////////

}  // namespace tesseract_gui::events
