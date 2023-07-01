#ifndef TESSERACT_IGNITION_CONVERSIONS_H
#define TESSERACT_IGNITION_CONVERSIONS_H

#include <gz/common/KeyEvent.hh>
#include <gz/common/MouseEvent.hh>
#include <gz/math/Color.hh>
#include <gz/math/Vector2.hh>
#include <gz/math/Vector3.hh>
#include <gz/rendering/Scene.hh>
#include <gz/rendering/Visual.hh>
#include <gz/rendering/Material.hh>

#include <tesseract_qt/common/entity_container.h>
#include <tesseract_scene_graph/graph.h>
#include <tesseract_scene_graph/link.h>
#include <tesseract_geometry/geometry.h>
#include <tesseract_geometry/impl/polygon_mesh.h>
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
QColor convert(const gz::math::Color& color);

/**
 * @brief Return the equivalent Ignition color
 * @param color Qt color to convert
 * @return Ignition color value
 */
gz::math::Color convert(const QColor& color);

/**
 * @brief Return the equivalent QPointF.
 * @param pt Ignition vector to convert.
 * @return Qt point value
 */
QPointF convert(const gz::math::Vector2d& pt);

/**
 * @brief Return the equivalent Ignition vector.
 * @param pt Qt point to convert
 * @return Ignition Vector2d.
 */
gz::math::Vector2d convert(const QPointF& pt);

/**
 * @brief Return the equivalent Qt vector 3d.
 * @param vec Ignition vector 3d to convert.
 * @return Qt vector 3d value.
 */
QVector3D convert(const gz::math::Vector3d& vec);

/**
 * @brief Return the equivalent Ignition vector 3d.
 * @param vec Qt vector 3d to convert.
 * @return Ignition vector 3d value
 */
gz::math::Vector3d convert(const QVector3D& vec);

/**
 * @brief Return the equivalent Ignition mouse event.
 * @note Tthere isn't a 1-1 mapping between these types, so fields such as common::MouseEvent::PressPos need to be set
 * afterwards.
 * @param e Qt mouse event
 * @return Ignition mouse event
 */
gz::common::MouseEvent convert(const QMouseEvent& e);

/**
 * @brief Return the equivalent Ignition mouse event.
 * @note There isn't a 1-1 mapping between these types.
 * @param e Qt wheel event
 * @return Ignition mouse event
 */
gz::common::MouseEvent convert(const QWheelEvent& e);

/**
 * @brief Return the equivalent ignition key event.
 * @param e Qt key event
 * @return  Ignition key event
 */
gz::common::KeyEvent convert(const QKeyEvent& e);

/**
 * @brief Convert polygon mesh to ignition SubMesh
 * @param mesh The polygon mesh to convert
 * @return A Ignition SubMesh
 */
gz::common::SubMesh convert(const tesseract_geometry::PolygonMesh& mesh);

bool isMeshWithColor(const std::string& file_path);

std::vector<std::string> loadSceneGraph(gz::rendering::Scene& scene,
                                        EntityContainer& entity_container,
                                        const tesseract_scene_graph::SceneGraph& scene_graph,
                                        const std::string& prefix = "");

gz::rendering::VisualPtr loadLink(gz::rendering::Scene& scene,
                                  EntityContainer& entity_container,
                                  const tesseract_scene_graph::Link& link);

gz::rendering::VisualPtr loadLinkVisuals(gz::rendering::Scene& scene,
                                         EntityContainer& entity_container,
                                         const tesseract_scene_graph::Link& link);

gz::rendering::VisualPtr loadLinkCollisions(gz::rendering::Scene& scene,
                                            EntityContainer& entity_container,
                                            const tesseract_scene_graph::Link& link);

gz::rendering::VisualPtr loadLinkWireBox(gz::rendering::Scene& scene,
                                         EntityContainer& entity_container,
                                         const tesseract_scene_graph::Link& link,
                                         const gz::math::AxisAlignedBox& aabb);

gz::rendering::VisualPtr loadLinkAxis(gz::rendering::Scene& scene,
                                      EntityContainer& entity_container,
                                      const tesseract_scene_graph::Link& link);

gz::rendering::VisualPtr loadLinkGeometry(gz::rendering::Scene& scene,
                                          EntityContainer& entity_container,
                                          const tesseract_geometry::Geometry& geometry,
                                          const Eigen::Vector3d& scale,
                                          const Eigen::Isometry3d& local_pose,
                                          const tesseract_scene_graph::Material::ConstPtr& material);

gz::rendering::MaterialPtr loadMaterial(gz::rendering::Scene& scene,
                                        const tesseract_scene_graph::Material::ConstPtr& material);
//  gz::rendering::LightPtr loadLight(gz::rendering::Scene& scene, const gz::msgs::Light &msg);

gz::rendering::VisualPtr loadContactResults(gz::rendering::Scene& scene,
                                            EntityContainer& entity_container,
                                            const tesseract_collision::ContactResultVector& contact_results);
}  // namespace tesseract_gui

#endif  // TESSERACT_IGNITION_CONVERSIONS_H
