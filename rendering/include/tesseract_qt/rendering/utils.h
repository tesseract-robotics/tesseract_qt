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
#ifndef TESSERACT_QT_RENDERING_UTILS_H
#define TESSERACT_QT_RENDERING_UTILS_H

#include <gz/rendering/Scene.hh>

namespace tesseract_gui
{
/**
 * @brief Get the scene from first render engine with the provided name
 * @param scene_name The scene name to retrieve
 * @return The scene
 */
gz::rendering::ScenePtr sceneFromFirstRenderEngine(const std::string& scene_name);

/**
 * @brief Get the scene from render engine
 * @param engine_name The render engine to retrieve scene from
 * @param scene_name The scene name to retrieve
 * @return The scene
 */
gz::rendering::ScenePtr sceneFromRenderEngine(const std::string& engine_name, const std::string& scene_name);

}  // namespace tesseract_gui
#endif  // TESSERACT_QT_RENDERING_UTILS_H
