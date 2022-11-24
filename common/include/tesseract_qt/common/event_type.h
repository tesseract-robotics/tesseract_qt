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
#ifndef TESSERACT_GUI_COMMON_EVENT_TYPE_H
#define TESSERACT_GUI_COMMON_EVENT_TYPE_H

#include <QEvent>
namespace tesseract_gui
{
/** @brief Event called  */
class SceneEvent : public QEvent
{
public:
  SceneEvent(std::string scene_name, QEvent::Type type) : QEvent(type), scene_name_(std::move(scene_name)) {}

  /**
   * @brief Get the scene name the event is associated with
   * @return The scene name
   */
  const std::string& getSceneName() const { return scene_name_; }

private:
  std::string scene_name_;
};

// clang-format off
enum class EventType : int
{
  EVENT_TYPE = QEvent::Type::User + 1000,

  // Render Events
  RENDER                              = EVENT_TYPE + 1,
  SNAP_INTERVALS                      = EVENT_TYPE + 2,
  SPAWN_FROM_DESCRIPTION              = EVENT_TYPE + 3,
  SPAWN_FROM_PATH                     = EVENT_TYPE + 4,
  HOVER_TO_SCENE                      = EVENT_TYPE + 5,
  LEFT_CLICK_TO_SCENE                 = EVENT_TYPE + 6,
  RIGHT_CLICK_TO_SCENE                = EVENT_TYPE + 7,
  DROPDOWN_MENU_ENABLED               = EVENT_TYPE + 8,
  KEY_RELEASE_ON_SCENE                = EVENT_TYPE + 9,
  KEY_PRESS_ON_SCENE                  = EVENT_TYPE + 10,
  LEFT_CLICK_ON_SCENE                 = EVENT_TYPE + 11,
  RIGHT_CLICK_ON_SCENE                = EVENT_TYPE + 12,
  BLOCK_ORBIT                         = EVENT_TYPE + 13,
  HOVER_ON_SCENE                      = EVENT_TYPE + 14,
  SPAWN_CLONE_FROM_NAME               = EVENT_TYPE + 15,
  DROP_ON_SCENE                       = EVENT_TYPE + 16,
  SCROLL_ON_SCENE                     = EVENT_TYPE + 17,
  DRAG_ON_SCENE                       = EVENT_TYPE + 18,
  MOUSE_PRESS_ON_SCENE                = EVENT_TYPE + 19,
  PRE_RENDER                          = EVENT_TYPE + 20,
  RENDER_TYPES_END                    = EVENT_TYPE + 21,

  // Tool Path
  TOOL_PATH_ADD             = RENDER_TYPES_END + 1,
  TOOL_PATH_REMOVE          = RENDER_TYPES_END + 2,
  TOOL_PATH_REMOVE_ALL      = RENDER_TYPES_END + 3,
  TOOL_PATH_REMOVE_SELECTED = RENDER_TYPES_END + 4,
  TOOL_PATH_HIDE_ALL        = RENDER_TYPES_END + 5,
  TOOL_PATH_HIDE_SELECTED   = RENDER_TYPES_END + 6,
  TOOL_PATH_SHOW_ALL        = RENDER_TYPES_END + 7,
  TOOL_PATH_SHOW_SELECTED   = RENDER_TYPES_END + 8,
  TOOL_PATH_TYPES_END       = RENDER_TYPES_END + 9,
};
}

#endif // TESSERACT_GUI_COMMON_EVENT_TYPE_H
