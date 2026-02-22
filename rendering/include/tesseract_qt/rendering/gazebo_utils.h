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
#ifndef TESSERACT_QT_RENDERING_GAZEBO_UTILS_H
#define TESSERACT_QT_RENDERING_GAZEBO_UTILS_H

#include <memory>
#include <tesseract_common/eigen_types.h>
#include <tesseract_geometry/fwd.h>
#include <tesseract_scene_graph/fwd.h>
#include <tesseract_collision/core/fwd.h>
#include <tesseract_collision/core/types.h>
#include <tesseract_environment/fwd.h>

#include <gz/rendering/config.hh>
#include <gz/math/config.hh>

namespace gz::rendering
{
inline namespace GZ_RENDERING_VERSION_NAMESPACE
{
class Scene;
class Visual;
class Material;
}  // namespace GZ_RENDERING_VERSION_NAMESPACE
}  // namespace gz::rendering

namespace gz::math
{
inline namespace GZ_MATH_VERSION_NAMESPACE
{
class AxisAlignedBox;
}
}  // namespace gz::math

namespace gz::common
{
class SubMesh;
}

namespace tesseract::gui
{
class EntityContainer;

/**
 * @brief Get the scene from first render engine with the provided name
 * @param scene_name The scene name to retrieve
 * @return The scene
 */
std::shared_ptr<gz::rendering::Scene> sceneFromFirstRenderEngine(const std::string& scene_name);

/**
 * @brief Get the scene from render engine
 * @param scene_name The scene name to retrieve
 * @param engine_name The render engine to retrieve scene from
 * @return The scene
 */
std::shared_ptr<gz::rendering::Scene>
sceneFromRenderEngine(const std::string& scene_name,
                      const std::string& engine_name,
                      const std::map<std::string, std::string>& engine_params = {});

/**
 * @brief Set the Gazebo Scene given link transform map
 * @param scene The scene to update
 * @param entity_container The associated entity container
 * @param link_transforms The link transform map used to update state
 */
void setSceneState(gz::rendering::Scene& scene,
                   const EntityContainer& entity_container,
                   const tesseract::common::TransformMap& link_transforms);

/**
 * @brief Clear all items associated with the provided entity container from the scene
 * @param scene The scene to clear
 * @param entity_container The entity container containing the items to remove
 */
void clearScene(gz::rendering::Scene& scene, EntityContainer& entity_container);

/**
 * @brief Convert polygon mesh to ignition SubMesh
 * @param mesh The polygon mesh to convert
 * @return A Ignition SubMesh
 */
gz::common::SubMesh convert(const tesseract::geometry::PolygonMesh& mesh);

/**
 * @brief Check if mesh has color
 * @param file_path The file path to mesh to check
 * @return True if the mesh contains color, otherwise false
 */
bool isMeshWithColor(const std::string& file_path);

/**
 * @brief Load scene graph into gazeo scene
 * @param scene The gazebo scene
 * @param entity_container The entity container to use
 * @param scene_graph The tesseract scene graph
 * @param prefix A prefix to apply if necessary
 * @return A list of link name added
 */
std::vector<std::string> loadSceneGraph(gz::rendering::Scene& scene,
                                        EntityContainer& entity_container,
                                        const tesseract::scene_graph::SceneGraph& scene_graph,
                                        const std::string& prefix = "");

/**
 * @brief Convert tesseract link to gazebo object
 * @note This does not added it to the scene
 * @param scene The gazebo scene
 * @param entity_container The entity container to use
 * @param link The tesseract link to convert
 * @return A gazebo visual object
 */
std::shared_ptr<gz::rendering::Visual> loadLink(gz::rendering::Scene& scene,
                                                EntityContainer& entity_container,
                                                const tesseract::scene_graph::Link& link);

/**
 * @brief Convert tesseract link visual objects to gazebo objects
 * @note This does not added it to the scene
 * @param scene The gazebo scene
 * @param entity_container The entity container to use
 * @param link The tesseract link to convert
 * @return A gazebo visual object
 */
std::shared_ptr<gz::rendering::Visual> loadLinkVisuals(gz::rendering::Scene& scene,
                                                       EntityContainer& entity_container,
                                                       const tesseract::scene_graph::Link& link);

/**
 * @brief Convert tesseract link collision objects to gazebo objects
 * @note This does not added it to the scene
 * @param scene The gazebo scene
 * @param entity_container The entity container to use
 * @param link The tesseract link to convert
 * @return A gazebo visual object
 */
std::shared_ptr<gz::rendering::Visual> loadLinkCollisions(gz::rendering::Scene& scene,
                                                          EntityContainer& entity_container,
                                                          const tesseract::scene_graph::Link& link);

/**
 * @brief Create a gazebo wire frame box for tesseract link
 * @note This does not added it to the scene
 * @param scene The gazebo scene
 * @param entity_container The entity container to use
 * @param link The tesseract link to convert
 * @param aabb The axis aligned bounding box of the link
 * @return A gazebo visual object
 */
std::shared_ptr<gz::rendering::Visual> loadLinkWireBox(gz::rendering::Scene& scene,
                                                       EntityContainer& entity_container,
                                                       const tesseract::scene_graph::Link& link,
                                                       const gz::math::AxisAlignedBox& aabb);

/**
 * @brief Create gazebo axis object for tesseract link
 * @note This does not added it to the scene
 * @param scene The gazebo scene
 * @param entity_container The entity container to use
 * @param link The tesseract link to convert
 * @return A gazebo visual object
 */
std::shared_ptr<gz::rendering::Visual> loadLinkAxis(gz::rendering::Scene& scene,
                                                    EntityContainer& entity_container,
                                                    const tesseract::scene_graph::Link& link);

/**
 * @brief Convert tesseract geometry objects to gazebo objects
 * @note This does not added it to the scene
 * @param scene The gazebo scene
 * @param entity_container The entity container to use
 * @param geometry The tesseract geometry to convert
 * @param scale The scale to use
 * @param local_pose The local pose to use
 * @param material The material to use
 * @return A gazebo visual object
 */
std::shared_ptr<gz::rendering::Visual>
loadLinkGeometry(gz::rendering::Scene& scene,
                 EntityContainer& entity_container,
                 const tesseract::geometry::Geometry& geometry,
                 const Eigen::Vector3d& scale,
                 const Eigen::Isometry3d& local_pose,
                 const std::shared_ptr<const tesseract::scene_graph::Material>& material);

/**
 * @brief Convert tesseract material objects to gazebo objects
 * @note This does not added it to the scene
 * @param scene The gazebo scene
 * @param material The tesseract material to convert
 * @return A gazebo visual object
 */
std::shared_ptr<gz::rendering::Material>
loadMaterial(gz::rendering::Scene& scene, const std::shared_ptr<const tesseract::scene_graph::Material>& material);

/**
 * @brief Convert tesseract contact results to gazebo objects
 * @note This does not added it to the scene
 * @param scene The gazebo scene
 * @param entity_container The entity container to use
 * @param contact_results The tesseract contact results to convert
 * @return A gazebo visual object
 */
std::shared_ptr<gz::rendering::Visual>
loadContactResults(gz::rendering::Scene& scene,
                   EntityContainer& entity_container,
                   const tesseract::collision::ContactResultVector& contact_results);

/**
 * @brief A tesseract event filter compatible with gazebo scene
 * @param event The tesseract event
 * @param env
 * @param current_revision
 * @param scene
 * @param entity_container
 */
void tesseractEventFilter(const tesseract::environment::Event& event,
                          const tesseract::environment::Environment& env,
                          int& current_revision,
                          gz::rendering::Scene& scene,
                          EntityContainer& entity_container); /** @brief Private data class for Renderer */

}  // namespace tesseract::gui
#endif  // TESSERACT_QT_RENDERING_GAZEBO_UTILS_H
