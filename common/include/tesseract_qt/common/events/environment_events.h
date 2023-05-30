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
#ifndef TESSERACT_QT_COMMON_ENVIRONMENT_EVENTS_H
#define TESSERACT_QT_COMMON_ENVIRONMENT_EVENTS_H

#include <vector>
#include <unordered_map>
#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>

namespace tesseract_environment
{
class Command;
}

namespace tesseract_gui::events
{
class EnvironmentCommandsClear : public ComponentEvent
{
public:
  EnvironmentCommandsClear(std::shared_ptr<const ComponentInfo> component_info);
  ~EnvironmentCommandsClear() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::ENVIRONMENT_COMMANDS_CLEAR);
};

class EnvironmentCommandsSet : public ComponentEvent
{
public:
  EnvironmentCommandsSet(std::shared_ptr<const ComponentInfo> component_info,
                         const std::vector<std::shared_ptr<const tesseract_environment::Command>>& commands);
  ~EnvironmentCommandsSet() override;

  const std::vector<std::shared_ptr<const tesseract_environment::Command>>& getCommands() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::ENVIRONMENT_COMMANDS_SET);

private:
  std::vector<std::shared_ptr<const tesseract_environment::Command>> commands_;
};

class EnvironmentCommandsAppend : public ComponentEvent
{
public:
  EnvironmentCommandsAppend(std::shared_ptr<const ComponentInfo> component_info,
                            const std::vector<std::shared_ptr<const tesseract_environment::Command>>& commands);
  ~EnvironmentCommandsAppend() override;

  const std::vector<std::shared_ptr<const tesseract_environment::Command>>& getCommands() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::ENVIRONMENT_COMMANDS_APPEND);

private:
  std::vector<std::shared_ptr<const tesseract_environment::Command>> commands_;
};

class EnvironmentApplyCommand : public ComponentEvent
{
public:
  EnvironmentApplyCommand(std::shared_ptr<const ComponentInfo> component_info,
                          const std::vector<std::shared_ptr<const tesseract_environment::Command>>& commands,
                          bool reset = false);
  ~EnvironmentApplyCommand() override;

  bool isReset() const;
  const std::vector<std::shared_ptr<const tesseract_environment::Command>>& getCommands() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::ENVIRONMENT_APPLY_COMMANDS);

private:
  std::vector<std::shared_ptr<const tesseract_environment::Command>> commands_;
  bool reset_;
};

}  // namespace tesseract_gui::events
#endif  // TESSERACT_QT_COMMON_ENVIRONMENT_EVENTS_H
