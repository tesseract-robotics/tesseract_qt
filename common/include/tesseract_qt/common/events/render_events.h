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
#ifndef TESSERACT_QT_COMMON_RENDER_EVENTS_H
#define TESSERACT_QT_COMMON_RENDER_EVENTS_H

#include <memory>
#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>

namespace tesseract_gui::events
{
class RenderEvent : public QEvent
{
public:
  RenderEvent(std::string scene_name, QEvent::Type type);
  ~RenderEvent() override;

  /** @brief Get the scene name the event is associated with */
  const std::string& getSceneName() const;

private:
  std::string scene_name_;
};

/**
 * @brief Event called in the render thread of a 3D scene, before the user camera is rendered.
 * @details It's safe to make rendering calls in this event's callback.
 */
class PreRender : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param scene_name The scene name
   */
  PreRender(std::string scene_name);
  ~PreRender() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::PRE_RENDER);
};

/**
 * @brief Event called in the render thread of a 3D scene after the user camera has rendered.
 * It's safe to make rendering calls in this event's callback.
 */
class Render : public RenderEvent
{
public:
  Render(std::string scene_name);
  ~Render() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::RENDER);
};
}  // namespace tesseract_gui::events
#endif  // TESSERACT_QT_COMMON_RENDER_EVENTS_H
