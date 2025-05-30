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

#include <tesseract_qt/common/events/task_composer_events.h>
#include <tesseract_common/profile_dictionary.h>

namespace tesseract_gui::events
{
TaskComposerLoadConfig::TaskComposerLoadConfig(std::shared_ptr<const ComponentInfo> component_info,
                                               std::string resource_path)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::TASK_COMPOSER_LOAD_CONFIG))
  , resource_path_(std::move(resource_path))
{
}

TaskComposerLoadConfig::~TaskComposerLoadConfig() = default;
const std::string& TaskComposerLoadConfig::getResourcePath() const { return resource_path_; }

//////////////////////////////////////////////////////

TaskComposerLoadLog::TaskComposerLoadLog(std::shared_ptr<const ComponentInfo> component_info,
                                         std::string resource_path,
                                         std::string ns)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::TASK_COMPOSER_LOAD_LOG))
  , resource_path_(std::move(resource_path))
  , ns_(std::move(ns))
{
}

TaskComposerLoadLog::~TaskComposerLoadLog() = default;
const std::string& TaskComposerLoadLog::getResourcePath() const { return resource_path_; }
const std::string& TaskComposerLoadLog::getNamespace() const { return ns_; }

//////////////////////////////////////////////////////

TaskComposerSaveLog::TaskComposerSaveLog(std::shared_ptr<const ComponentInfo> component_info, std::string save_path)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::TASK_COMPOSER_SAVE_LOG))
  , save_path_(std::move(save_path))
{
}

TaskComposerSaveLog::~TaskComposerSaveLog() = default;
const std::string& TaskComposerSaveLog::getSavePath() const { return save_path_; }

//////////////////////////////////////////////////////

TaskComposerPlotDotgraph::TaskComposerPlotDotgraph(std::shared_ptr<const ComponentInfo> component_info)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::TASK_COMPOSER_PLOT_DOTGRAPH))
{
}

TaskComposerPlotDotgraph::~TaskComposerPlotDotgraph() = default;

//////////////////////////////////////////////////////

TaskComposerSetProfiles::TaskComposerSetProfiles(std::shared_ptr<const ComponentInfo> component_info,
                                                 std::shared_ptr<tesseract_common::ProfileDictionary> profiles)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::TASK_COMPOSER_SET_PROFILES))
  , profiles_(std::move(profiles))
{
}

TaskComposerSetProfiles::~TaskComposerSetProfiles() = default;
std::shared_ptr<tesseract_common::ProfileDictionary> TaskComposerSetProfiles::getProfiles() const { return profiles_; }
}  // namespace tesseract_gui::events
