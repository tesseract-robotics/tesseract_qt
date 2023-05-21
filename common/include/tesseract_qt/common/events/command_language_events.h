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
#ifndef TESSERACT_QT_COMMON_COMMAND_LANGUAGE_EVENTS_H
#define TESSERACT_QT_COMMON_COMMAND_LANGUAGE_EVENTS_H

#include <memory>
#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>

namespace tesseract_planning
{
class CompositeInstruction;
}

namespace tesseract_gui::events
{
class CompositeInstructionClear : public ComponentEvent
{
public:
  CompositeInstructionClear(std::shared_ptr<const ComponentInfo> component_info, const std::string& ns = "");
  ~CompositeInstructionClear() override;

  const std::string& getNamespace() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::CL_COMPOSITE_INSTRUCTION_CLEAR);

private:
  std::string ns_;
};

class CompositeInstructionSet : public ComponentEvent
{
public:
  CompositeInstructionSet(std::shared_ptr<const ComponentInfo> component_info,
                          const tesseract_planning::CompositeInstruction& composite_instruction,
                          const std::string& ns = "");
  ~CompositeInstructionSet() override;

  const std::string& getNamespace() const;
  const tesseract_planning::CompositeInstruction& getCompositeInstruction() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::CL_COMPOSITE_INSTRUCTION_SET);

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;
};

class CompositeInstructionRemove : public ComponentEventUUID
{
public:
  CompositeInstructionRemove(std::shared_ptr<const ComponentInfo> component_info, boost::uuids::uuid uuid);
  CompositeInstructionRemove(std::shared_ptr<const ComponentInfo> component_info,
                             boost::uuids::uuid uuid,
                             boost::uuids::uuid child_uuid);
  ~CompositeInstructionRemove() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::CL_COMPOSITE_INSTRUCTION_REMOVE);
};

}  // namespace tesseract_gui::events
#endif  // TESSERACT_QT_COMMON_COMMAND_LANGUAGE_EVENTS_H
