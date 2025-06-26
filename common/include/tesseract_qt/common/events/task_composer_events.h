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

#include <tesseract_common/fwd.h>
#include <tesseract_task_composer/core/task_composer_log.h>
#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>

namespace tesseract_gui::events
{
class TaskComposerLoadConfig : public ComponentEvent
{
public:
  TaskComposerLoadConfig(std::shared_ptr<const ComponentInfo> component_info, std::string resource_path);
  ~TaskComposerLoadConfig() override;

  const std::string& getResourcePath() const;

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

private:
  std::string resource_path_;
  std::string ns_;
};

class TaskComposerAddLog : public ComponentEvent
{
public:
  TaskComposerAddLog(std::shared_ptr<const ComponentInfo> component_info,
                     tesseract_planning::TaskComposerLog log,
                     std::string ns = "");
  ~TaskComposerAddLog() override;

  const tesseract_planning::TaskComposerLog& getLog() const;
  const std::string& getNamespace() const;

private:
  tesseract_planning::TaskComposerLog log_;
  std::string ns_;
};

class TaskComposerSaveLog : public ComponentEvent
{
public:
  TaskComposerSaveLog(std::shared_ptr<const ComponentInfo> component_info, std::string save_path);
  ~TaskComposerSaveLog() override;

  const std::string& getSavePath() const;

private:
  std::string save_path_;
};

class TaskComposerPlotDotgraph : public ComponentEvent
{
public:
  TaskComposerPlotDotgraph(std::shared_ptr<const ComponentInfo> component_info);
  ~TaskComposerPlotDotgraph() override;
};

class TaskComposerSetProfiles : public ComponentEvent
{
public:
  TaskComposerSetProfiles(std::shared_ptr<const ComponentInfo> component_info,
                          std::shared_ptr<tesseract_common::ProfileDictionary> profiles);
  ~TaskComposerSetProfiles() override;

  std::shared_ptr<tesseract_common::ProfileDictionary> getProfiles() const;

private:
  std::shared_ptr<tesseract_common::ProfileDictionary> profiles_;
};
}  // namespace tesseract_gui::events

#endif  // TESSERACT_QT_COMMON_TASK_COMPOSER_EVENTS_H
