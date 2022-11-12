/*
 * Copyright (C) 2020 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <tesseract_qt/rendering/utils.h>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <console_bridge/console.h>

namespace tesseract_gui
{
ignition::rendering::ScenePtr sceneFromFirstRenderEngine(const std::string& scene_name)
{
  auto loaded_eng_names = ignition::rendering::loadedEngines();
  if (loaded_eng_names.empty())
  {
    CONSOLE_BRIDGE_logDebug("No rendering engine is loaded yet");
    return nullptr;
  }

  auto engine_name = loaded_eng_names[0];
  if (loaded_eng_names.size() > 1)
  {
    CONSOLE_BRIDGE_logWarn("More than one engine is available. Using engine [%s]", engine_name);
  }

  auto* engine = ignition::rendering::engine(engine_name);
  if (!engine)
  {
    CONSOLE_BRIDGE_logError("Internal error: failed to load engine [%s]", engine_name);
    return nullptr;
  }

  if (engine->SceneCount() == 0)
  {
    CONSOLE_BRIDGE_logDebug("No scene has been created yet");
    return nullptr;
  }

  auto scene = engine->SceneByName(scene_name);
  if (nullptr == scene)
  {
    CONSOLE_BRIDGE_logError("Internal error: scene is null.");
    return nullptr;
  }

  return scene;
}

ignition::rendering::ScenePtr sceneFromFirstRenderEngine(const std::string& engine_name, const std::string& scene_name)
{
  auto* engine = ignition::rendering::engine(engine_name);
  if (!engine)
  {
    CONSOLE_BRIDGE_logError("Internal error: failed to load engine [%s]", engine_name);
    return nullptr;
  }

  if (engine->SceneCount() == 0)
  {
    CONSOLE_BRIDGE_logDebug("No scene has been created yet");
    return nullptr;
  }

  auto scene = engine->SceneByName(scene_name);
  if (nullptr == scene)
  {
    CONSOLE_BRIDGE_logError("Internal error: scene is null.");
    return nullptr;
  }

  return scene;
}
}  // namespace tesseract_gui
