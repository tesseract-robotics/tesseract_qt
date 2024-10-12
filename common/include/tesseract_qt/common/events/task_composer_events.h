/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2024 Levi Armstrong <levi.armstrong@gmail.com>
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
#ifndef TESSERACT_QT_COMMON_TASK_COMPOSER_EVENTS_H
#define TESSERACT_QT_COMMON_TASK_COMPOSER_EVENTS_H

#include <tesseract_command_language/fwd.h>
#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>
#include <tesseract_common/filesystem.h>

namespace tesseract_gui::events
{
class TaskComposerLoadConfig : public ComponentEvent
{
public:
  TaskComposerLoadConfig(std::shared_ptr<const ComponentInfo> component_info, std::string resource_path);
  ~TaskComposerLoadConfig() override;

  const std::string& getResourcePath() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TASK_COMPOSER_LOAD_CONFIG);

private:
  std::string resource_path_;
};

class TaskComposerLoadLog : public ComponentEvent
{
public:
  TaskComposerLoadLog(std::shared_ptr<const ComponentInfo> component_info,
                      std::string resource_path,
                      std::string ns = "");
  ~TaskComposerLoadLog() override;

  const std::string& getResourcePath() const;
  const std::string& getNamespace() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TASK_COMPOSER_LOAD_LOG);

private:
  std::string resource_path_;
  std::string ns_;
};

class TaskComposerSaveLog : public ComponentEvent
{
public:
  TaskComposerSaveLog(std::shared_ptr<const ComponentInfo> component_info, std::string save_path);
  ~TaskComposerSaveLog() override;

  const std::string& getSavePath() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TASK_COMPOSER_SAVE_LOG);

private:
  std::string save_path_;
};

class TaskComposerPlotDotgraph : public ComponentEvent
{
public:
  TaskComposerPlotDotgraph(std::shared_ptr<const ComponentInfo> component_info);
  ~TaskComposerPlotDotgraph() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TASK_COMPOSER_PLOT_DOTGRAPH);
};

class TaskComposerSetProfiles : public ComponentEvent
{
public:
  TaskComposerSetProfiles(std::shared_ptr<const ComponentInfo> component_info,
                          std::shared_ptr<tesseract_planning::ProfileDictionary> profiles);
  ~TaskComposerSetProfiles() override;

  std::shared_ptr<tesseract_planning::ProfileDictionary> getProfiles() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TASK_COMPOSER_SET_PROFILES);

private:
  std::shared_ptr<tesseract_planning::ProfileDictionary> profiles_;
};
}  // namespace tesseract_gui::events

#endif  // TESSERACT_QT_COMMON_TASK_COMPOSER_EVENTS_H
