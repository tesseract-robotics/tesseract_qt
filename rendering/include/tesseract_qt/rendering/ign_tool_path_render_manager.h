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
#ifndef TESSERACT_QT_RENDERING_IGN_TOOL_PATH_RENDER_MANAGER_H
#define TESSERACT_QT_RENDERING_IGN_TOOL_PATH_RENDER_MANAGER_H

#include <tesseract_qt/common/events/tool_path_render_manager.h>

namespace tesseract_gui
{
class EntityManager;
struct ComponentInfo;

class IgnToolPathRenderManager : public ToolPathRenderManager
{
public:
  IgnToolPathRenderManager(std::shared_ptr<const ComponentInfo> component_info,
                           std::shared_ptr<EntityManager> entity_manager);
  ~IgnToolPathRenderManager();

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;

  void render() override;
  void updateWorkingFrameTransforms();
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_RENDERING_IGN_TOOL_PATH_RENDER_MANAGER_H
