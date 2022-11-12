#ifndef TESSERACT_IGNITION_CONVERSIONS_H
#define TESSERACT_IGNITION_CONVERSIONS_H

#include <ignition/common/KeyEvent.hh>
#include <ignition/common/MouseEvent.hh>
#include <ignition/common/Time.hh>
#include <ignition/math/Color.hh>
#include <ignition/math/Vector2.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/rendering/Scene.hh>
#include <ignition/rendering/Visual.hh>
#include <ignition/rendering/Material.hh>

#include <tesseract_qt/common/entity_container.h>
#include <tesseract_scene_graph/graph.h>
#include <tesseract_scene_graph/link.h>
#include <tesseract_geometry/geometry.h>
#include <tesseract_collision/core/types.h>

#include <QtCore>
#include <QColor>
#include <QVector3D>
#include <QInputEvent>

namespace tesseract_gui
{
/**
 * @brief Return the equivalent Qt color
 * @param color Ignition color to convert
 * @return Qt color value
 */
QColor convert(const ignition::math::Color& color);

/**
 * @brief Return the equivalent Ignition color
 * @param color Qt color to convert
 * @return Ignition color value
 */
ignition::math::Color convert(const QColor& color);

/**
 * @brief Return the equivalent QPointF.
 * @param pt Ignition vector to convert.
 * @return Qt point value
 */
QPointF convert(const ignition::math::Vector2d& pt);

/**
 * @brief Return the equivalent Ignition vector.
 * @param pt Qt point to convert
 * @return Ignition Vector2d.
 */
ignition::math::Vector2d convert(const QPointF& pt);

/**
 * @brief Return the equivalent Qt vector 3d.
 * @param vec Ignition vector 3d to convert.
 * @return Qt vector 3d value.
 */
QVector3D convert(const ignition::math::Vector3d& vec);

/**
 * @brief Return the equivalent Ignition vector 3d.
 * @param vec Qt vector 3d to convert.
 * @return Ignition vector 3d value
 */
ignition::math::Vector3d convert(const QVector3D& vec);

/**
 * @brief Return the equivalent Ignition mouse event.
 * @note Tthere isn't a 1-1 mapping between these types, so fields such as common::MouseEvent::PressPos need to be set
 * afterwards.
 * @param e Qt mouse event
 * @return Ignition mouse event
 */
ignition::common::MouseEvent convert(const QMouseEvent& e);

/**
 * @brief Return the equivalent Ignition mouse event.
 * @note There isn't a 1-1 mapping between these types.
 * @param e Qt wheel event
 * @return Ignition mouse event
 */
ignition::common::MouseEvent convert(const QWheelEvent& e);

/**
 * @brief Return the equivalent ignition key event.
 * @param e Qt key event
 * @return  Ignition key event
 */
ignition::common::KeyEvent convert(const QKeyEvent& e);

bool isMeshWithColor(const std::string& file_path);

std::vector<std::string> loadSceneGraph(ignition::rendering::Scene& scene,
                                        EntityContainer& entity_container,
                                        const tesseract_scene_graph::SceneGraph& scene_graph,
                                        const std::string& prefix = "");

ignition::rendering::VisualPtr loadLink(ignition::rendering::Scene& scene,
                                        EntityContainer& entity_container,
                                        const tesseract_scene_graph::Link& link);

ignition::rendering::VisualPtr loadLinkVisuals(ignition::rendering::Scene& scene,
                                               EntityContainer& entity_container,
                                               const tesseract_scene_graph::Link& link);

ignition::rendering::VisualPtr loadLinkCollisions(ignition::rendering::Scene& scene,
                                                  EntityContainer& entity_container,
                                                  const tesseract_scene_graph::Link& link);

ignition::rendering::VisualPtr loadLinkWireBox(ignition::rendering::Scene& scene,
                                               EntityContainer& entity_container,
                                               const tesseract_scene_graph::Link& link,
                                               const ignition::math::AxisAlignedBox& aabb);

ignition::rendering::VisualPtr loadLinkAxis(ignition::rendering::Scene& scene,
                                            EntityContainer& entity_container,
                                            const tesseract_scene_graph::Link& link);

ignition::rendering::VisualPtr loadLinkGeometry(ignition::rendering::Scene& scene,
                                                EntityContainer& entity_container,
                                                const tesseract_geometry::Geometry& geometry,
                                                const Eigen::Vector3d& scale,
                                                const Eigen::Isometry3d& local_pose,
                                                const tesseract_scene_graph::Material::ConstPtr& material);

ignition::rendering::MaterialPtr loadMaterial(ignition::rendering::Scene& scene,
                                              const tesseract_scene_graph::Material::ConstPtr& material);
//  ignition::rendering::LightPtr loadLight(ignition::rendering::Scene& scene, const ignition::msgs::Light &msg);

ignition::rendering::VisualPtr loadContactResults(ignition::rendering::Scene& scene,
                                                  EntityContainer& entity_container,
                                                  const tesseract_collision::ContactResultVector& contact_results);
}  // namespace tesseract_gui

#endif  // TESSERACT_IGNITION_CONVERSIONS_H
