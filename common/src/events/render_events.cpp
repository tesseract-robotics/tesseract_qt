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
#include <tesseract_qt/common/events/render_events.h>

namespace tesseract_gui::events
{
/////////////////////////////////////////////////
RenderEvent::RenderEvent(std::string scene_name, QEvent::Type type) : QEvent(type), scene_name_(std::move(scene_name))
{
}
RenderEvent::~RenderEvent() = default;

const std::string& RenderEvent::getSceneName() const { return scene_name_; }

/////////////////////////////////////////////////
Render::Render(std::string scene_name) : RenderEvent(std::move(scene_name), kType) {}
Render::~Render() = default;

/////////////////////////////////////////////////

PreRender::PreRender(std::string scene_name) : RenderEvent(std::move(scene_name), kType) {}
PreRender::~PreRender() = default;
}  // namespace tesseract_gui::events
