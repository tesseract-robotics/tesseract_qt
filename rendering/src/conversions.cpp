/*
 * Copyright (C) 2017 Open Source Robotics Foundation
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

#include <tesseract_qt/rendering/conversions.h>
#include <tesseract_geometry/geometries.h>
#include <gz/math/eigen3/Conversions.hh>
#include <gz/common/MeshManager.hh>
#include <gz/rendering/WireBox.hh>
#include <gz/rendering/AxisVisual.hh>
#include <gz/rendering/Capsule.hh>

const std::string USER_VISIBILITY = "user_visibility";
const std::string USER_PARENT_VISIBILITY = "user_parent_visibility";

namespace tesseract_gui
{
//////////////////////////////////////////////////
QColor convert(const gz::math::Color& color)
{
  return QColor(color.R() * 255.0, color.G() * 255.0, color.B() * 255.0, color.A() * 255.0);
}

//////////////////////////////////////////////////
gz::math::Color convert(const QColor& color)
{
  return gz::math::Color(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0, color.alpha() / 255.0);
}

//////////////////////////////////////////////////
QPointF convert(const gz::math::Vector2d& pt) { return QPointF(pt.X(), pt.Y()); }

//////////////////////////////////////////////////
gz::math::Vector2d convert(const QPointF& pt) { return gz::math::Vector2d(pt.x(), pt.y()); }

//////////////////////////////////////////////////
QVector3D convert(const gz::math::Vector3d& vec) { return QVector3D(vec.X(), vec.Y(), vec.Z()); }

//////////////////////////////////////////////////
gz::math::Vector3d convert(const QVector3D& vec) { return gz::math::Vector3d(vec.x(), vec.y(), vec.z()); }

//////////////////////////////////////////////////
gz::common::MouseEvent convert(const QMouseEvent& e)
{
  gz::common::MouseEvent event;
  event.SetPos(e.pos().x(), e.pos().y());

  // Button
  if (e.button() == Qt::LeftButton)
    event.SetButton(gz::common::MouseEvent::LEFT);
  else if (e.button() == Qt::RightButton)
    event.SetButton(gz::common::MouseEvent::RIGHT);
  else if (e.button() == Qt::MiddleButton)
    event.SetButton(gz::common::MouseEvent::MIDDLE);

  // Buttons
  if (e.buttons() & Qt::LeftButton)
    event.SetButtons(event.Buttons() | gz::common::MouseEvent::LEFT);

  if (e.buttons() & Qt::RightButton)
    event.SetButtons(event.Buttons() | gz::common::MouseEvent::RIGHT);

  if (e.buttons() & Qt::MiddleButton)
    event.SetButtons(event.Buttons() | gz::common::MouseEvent::MIDDLE);

  // Type
  if (e.type() == QEvent::MouseButtonPress)
    event.SetType(gz::common::MouseEvent::PRESS);
  else if (e.type() == QEvent::MouseButtonRelease)
    event.SetType(gz::common::MouseEvent::RELEASE);
  else if (e.type() == QEvent::MouseMove)
  {
    event.SetType(gz::common::MouseEvent::MOVE);

    // Dragging
    if (e.buttons() || e.button())
      event.SetDragging(true);
  }

  // Modifiers
  if (e.modifiers() & Qt::ShiftModifier)
    event.SetShift(true);

  if (e.modifiers() & Qt::ControlModifier)
    event.SetControl(true);

  if (e.modifiers() & Qt::AltModifier)
    event.SetAlt(true);

  return event;
}

//////////////////////////////////////////////////
gz::common::MouseEvent convert(const QWheelEvent& e)
{
  gz::common::MouseEvent event;

  event.SetType(gz::common::MouseEvent::SCROLL);
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
  event.SetPos(e.x(), e.y());
#else
  event.SetPos(e.position().x(), e.position().y());
#endif
  double scroll = (e.angleDelta().y() > 0) ? -1.0 : 1.0;
  event.SetScroll(scroll, scroll);

  // Buttons
  if (e.buttons() & Qt::LeftButton)
    event.SetButtons(event.Buttons() | gz::common::MouseEvent::LEFT);

  if (e.buttons() & Qt::RightButton)
    event.SetButtons(event.Buttons() | gz::common::MouseEvent::RIGHT);

  if (e.buttons() & Qt::MiddleButton)
    event.SetButtons(event.Buttons() | gz::common::MouseEvent::MIDDLE);

  // Modifiers
  if (e.modifiers() & Qt::ShiftModifier)
    event.SetShift(true);

  if (e.modifiers() & Qt::ControlModifier)
    event.SetControl(true);

  if (e.modifiers() & Qt::AltModifier)
    event.SetAlt(true);

  return event;
}

//////////////////////////////////////////////////
gz::common::KeyEvent convert(const QKeyEvent& e)
{
  gz::common::KeyEvent event;
  event.SetKey(e.key());
  event.SetText(e.text().toStdString());

  if (e.type() == QEvent::KeyPress)
  {
    event.SetType(gz::common::KeyEvent::PRESS);
  }
  else if (e.type() == QEvent::KeyRelease)
  {
    event.SetType(gz::common::KeyEvent::RELEASE);
  }
  else
  {
    event.SetType(gz::common::KeyEvent::NO_EVENT);
  }

  event.SetControl(e.modifiers() & Qt::ControlModifier);
  event.SetShift(e.modifiers() & Qt::ShiftModifier);
  event.SetAlt(e.modifiers() & Qt::AltModifier);

  return event;
}

bool isMeshWithColor(const std::string& file_path)
{
  if (file_path.length() >= 4)
  {
    std::string last_four = file_path.substr(file_path.length() - 4);
    std::string last_four_lower;
    last_four_lower.resize(4);
    std::transform(last_four.begin(), last_four.end(), last_four_lower.begin(), ::tolower);
    return (last_four_lower == ".dae") || (last_four_lower == ".obj");
  }

  return false;
}

std::vector<std::string> loadSceneGraph(gz::rendering::Scene& scene,
                                        tesseract_gui::EntityContainer& entity_container,
                                        const tesseract_scene_graph::SceneGraph& scene_graph,
                                        const std::string& prefix)
{
  std::vector<std::string> link_names;
  gz::rendering::VisualPtr root = scene.RootVisual();
  if (prefix.empty())
  {
    for (const auto& link : scene_graph.getLinks())
    {
      root->AddChild(loadLink(scene, entity_container, *link));
      link_names.push_back(link->getName());
    }
  }
  else
  {
    for (const auto& link : scene_graph.getLinks())
    {
      auto clone_link = link->clone(prefix + link->getName());
      root->AddChild(loadLink(scene, entity_container, clone_link));
      link_names.push_back(clone_link.getName());
    }
  }
  return link_names;
}

gz::rendering::VisualPtr loadLink(gz::rendering::Scene& scene,
                                  tesseract_gui::EntityContainer& entity_container,
                                  const tesseract_scene_graph::Link& link)
{
  auto entity = entity_container.addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, link.getName());
  gz::rendering::VisualPtr ign_link = scene.CreateVisual(entity.id, entity.unique_name);
  ign_link->SetUserData(USER_VISIBILITY, true);

  gz::rendering::VisualPtr ign_link_visuals = loadLinkVisuals(scene, entity_container, link);
  ign_link_visuals->SetUserData(USER_VISIBILITY, true);
  ign_link->AddChild(ign_link_visuals);

  gz::rendering::VisualPtr ign_link_collisions = loadLinkCollisions(scene, entity_container, link);
  ign_link_collisions->SetUserData(USER_VISIBILITY, false);
  ign_link->AddChild(ign_link_collisions);

  auto ign_link_axis = loadLinkAxis(scene, entity_container, link);
  ign_link_axis->SetVisible(false);
  ign_link_axis->SetUserData(USER_VISIBILITY, false);
  ign_link->AddChild(ign_link_axis);

  if (!link.visual.empty() || !link.collision.empty())
  {
    auto ign_link_wirebox = loadLinkWireBox(scene, entity_container, link, ign_link->LocalBoundingBox());
    ign_link_wirebox->SetUserData(USER_VISIBILITY, false);
    ign_link->AddChild(ign_link_wirebox);
  }

  return ign_link;
}

gz::rendering::VisualPtr loadLinkVisuals(gz::rendering::Scene& scene,
                                         EntityContainer& entity_container,
                                         const tesseract_scene_graph::Link& link)
{
  std::string name = link.getName() + "::Visuals";
  auto entity = entity_container.addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, name);
  gz::rendering::VisualPtr ign_link_visuals = scene.CreateVisual(entity.id, entity.unique_name);

  for (const auto& visual : link.visual)
    ign_link_visuals->AddChild(loadLinkGeometry(
        scene, entity_container, *visual->geometry, Eigen::Vector3d::Ones(), visual->origin, visual->material));

  return ign_link_visuals;
}

gz::rendering::VisualPtr loadLinkCollisions(gz::rendering::Scene& scene,
                                            EntityContainer& entity_container,
                                            const tesseract_scene_graph::Link& link)
{
  std::string name = link.getName() + "::Collisions";
  auto entity = entity_container.addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, name);
  gz::rendering::VisualPtr ign_link_collisions = scene.CreateVisual(entity.id, entity.unique_name);

  for (const auto& collision : link.collision)
    ign_link_collisions->AddChild(loadLinkGeometry(
        scene, entity_container, *collision->geometry, Eigen::Vector3d::Ones(), collision->origin, nullptr));

  ign_link_collisions->SetVisible(false);
  return ign_link_collisions;
}

gz::rendering::VisualPtr loadLinkWireBox(gz::rendering::Scene& scene,
                                         EntityContainer& entity_container,
                                         const tesseract_scene_graph::Link& link,
                                         const gz::math::AxisAlignedBox& aabb)
{
  std::string name = link.getName() + "::WireBox";
  auto entity = entity_container.addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, name);

  auto white = scene.Material("highlight_material");
  if (!white)
  {
    white = scene.CreateMaterial("highlight_material");
    white->SetAmbient(1.0, 1.0, 1.0);
    white->SetDiffuse(1.0, 1.0, 1.0);
    white->SetSpecular(1.0, 1.0, 1.0);
    white->SetEmissive(1.0, 1.0, 1.0);
  }

  gz::rendering::WireBoxPtr wire_box = scene.CreateWireBox();
  wire_box->SetBox(aabb);

  // Create visual and add wire box
  gz::rendering::VisualPtr wire_box_vis = scene.CreateVisual(entity.id, entity.unique_name);
  wire_box_vis->SetInheritScale(false);
  wire_box_vis->AddGeometry(wire_box);
  wire_box_vis->SetMaterial(white, false);
  wire_box_vis->SetVisible(false);

  return wire_box_vis;
}

gz::rendering::VisualPtr loadLinkAxis(gz::rendering::Scene& scene,
                                      EntityContainer& entity_container,
                                      const tesseract_scene_graph::Link& link)
{
  std::string name = link.getName() + "::Axis";
  auto entity = entity_container.addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, name);

  gz::rendering::AxisVisualPtr axis = scene.CreateAxisVisual(entity.id, entity.unique_name);
  axis->SetInheritScale(false);
  axis->Scale(0.1, 0.1, 0.1);
  axis->SetVisible(true);

  return axis;
}

gz::rendering::VisualPtr loadLinkGeometry(gz::rendering::Scene& scene,
                                          tesseract_gui::EntityContainer& entity_container,
                                          const tesseract_geometry::Geometry& geometry,
                                          const Eigen::Vector3d& scale,
                                          const Eigen::Isometry3d& local_pose,
                                          const tesseract_scene_graph::Material::ConstPtr& material)
{
  gz::rendering::MaterialPtr ign_material = loadMaterial(scene, material);
  switch (geometry.getType())
  {
    case tesseract_geometry::GeometryType::BOX:
    {
      auto gv_entity = entity_container.addUntrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS);
      gz::rendering::VisualPtr box = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      box->SetLocalPose(gz::math::eigen3::convert(local_pose));
      box->AddGeometry(scene.CreateBox());

      const auto& shape = static_cast<const tesseract_geometry::Box&>(geometry);
      box->Scale(shape.getX() * scale.x(), shape.getY() * scale.y(), shape.getZ() * scale.z());
      box->SetMaterial(ign_material);
      return box;
    }
    case tesseract_geometry::GeometryType::SPHERE:
    {
      auto gv_entity = entity_container.addUntrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS);
      gz::rendering::VisualPtr sphere = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      sphere->SetLocalPose(gz::math::eigen3::convert(local_pose));
      sphere->AddGeometry(scene.CreateSphere());

      const auto& shape = static_cast<const tesseract_geometry::Sphere&>(geometry);
      const double diameter = 2.0 * shape.getRadius();
      sphere->Scale(diameter * scale.x(), diameter * scale.y(), diameter * scale.z());
      sphere->SetMaterial(ign_material);
      return sphere;
    }
    case tesseract_geometry::GeometryType::CYLINDER:
    {
      auto gv_entity = entity_container.addUntrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS);
      gz::rendering::VisualPtr cylinder = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      cylinder->SetLocalPose(gz::math::eigen3::convert(local_pose));
      cylinder->AddGeometry(scene.CreateCylinder());

      const auto& shape = static_cast<const tesseract_geometry::Cylinder&>(geometry);
      const double diameter = 2.0 * shape.getRadius();
      cylinder->Scale(diameter * scale.x(), diameter * scale.y(), shape.getLength() * scale.z());
      cylinder->SetMaterial(ign_material);
      return cylinder;
    }
    case tesseract_geometry::GeometryType::CONE:
    {
      auto gv_entity = entity_container.addUntrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS);
      gz::rendering::VisualPtr cone = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      cone->SetLocalPose(gz::math::eigen3::convert(local_pose));
      cone->AddGeometry(scene.CreateCone());

      const auto& shape = static_cast<const tesseract_geometry::Cone&>(geometry);
      const double diameter = 2.0 * shape.getRadius();
      cone->Scale(diameter * scale.x(), diameter * scale.y(), shape.getLength() * scale.z());
      cone->SetMaterial(ign_material);
      return cone;
    }
    case tesseract_geometry::GeometryType::CAPSULE:
    {
      auto gv_entity = entity_container.addUntrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS);
      gz::rendering::VisualPtr capsule = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      capsule->SetLocalPose(gz::math::eigen3::convert(local_pose));
      capsule->AddGeometry(scene.CreateCapsule());

      const auto& shape = static_cast<const tesseract_geometry::Capsule&>(geometry);
      const double diameter = 2.0 * shape.getRadius();
      capsule->Scale(diameter * scale.x(), diameter * scale.y(), shape.getLength() * scale.z());
      capsule->SetMaterial(ign_material);
      return capsule;
    }
    case tesseract_geometry::GeometryType::MESH:
    {
      const auto& shape = static_cast<const tesseract_geometry::Mesh&>(geometry);
      auto resource = shape.getResource();
      if (resource)
      {
        auto gv_entity = entity_container.addUntrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS);
        gz::rendering::VisualPtr mesh = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
        mesh->SetLocalPose(gz::math::eigen3::convert(local_pose));

        gz::rendering::MeshDescriptor descriptor;
        descriptor.meshName = resource->getFilePath();
        gz::common::MeshManager* mesh_manager = gz::common::MeshManager::Instance();
        descriptor.mesh = mesh_manager->Load(descriptor.meshName);
        gz::rendering::MeshPtr mesh_geom = scene.CreateMesh(descriptor);

        if (!isMeshWithColor(resource->getFilePath()))
          mesh_geom->SetMaterial(ign_material);

        mesh->AddGeometry(mesh_geom);
        mesh->SetLocalScale(shape.getScale().x(), shape.getScale().y(), shape.getScale().z());
        return mesh;
      }

      assert(false);
      return nullptr;
    }
    case tesseract_geometry::GeometryType::CONVEX_MESH:
    {
      const auto& shape = static_cast<const tesseract_geometry::ConvexMesh&>(geometry);
      auto resource = shape.getResource();
      if (resource)
      {
        auto gv_entity = entity_container.addUntrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS);
        gz::rendering::VisualPtr mesh = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
        mesh->SetLocalPose(gz::math::eigen3::convert(local_pose));

        gz::rendering::MeshDescriptor descriptor;
        descriptor.meshName = resource->getFilePath();
        gz::common::MeshManager* mesh_manager = gz::common::MeshManager::Instance();
        descriptor.mesh = mesh_manager->Load(descriptor.meshName);
        gz::rendering::MeshPtr mesh_geom = scene.CreateMesh(descriptor);

        if (!isMeshWithColor(resource->getFilePath()))
          mesh_geom->SetMaterial(ign_material);

        mesh->AddGeometry(mesh_geom);
        mesh->SetLocalScale(shape.getScale().x(), shape.getScale().y(), shape.getScale().z());
        return mesh;
      }

      assert(false);
      return nullptr;
    }
    case tesseract_geometry::GeometryType::OCTREE:
    {
      const auto& shape = static_cast<const tesseract_geometry::Octree&>(geometry);

      // TODO: Need to implement
      assert(false);
      return nullptr;
    }
    default:
    {
      //      CONSOLE_BRIDGE_logError("This geometric shape type (%d) is not supported",
      //                              static_cast<int>(geometry->getType()));
      return nullptr;
    }
  }
}

gz::rendering::MaterialPtr loadMaterial(gz::rendering::Scene& scene,
                                        const tesseract_scene_graph::Material::ConstPtr& material)
{
  if (material == nullptr)
  {
    const Eigen::Vector4d& rgba = tesseract_scene_graph::DEFAULT_TESSERACT_MATERIAL->color;
    auto ign_material = scene.Material(tesseract_scene_graph::DEFAULT_TESSERACT_MATERIAL->getName());
    if (ign_material == nullptr)
    {
      ign_material = scene.CreateMaterial(tesseract_scene_graph::DEFAULT_TESSERACT_MATERIAL->getName());
      ign_material->SetAmbient(rgba(0), rgba(1), rgba(2), rgba(3));
      ign_material->SetDiffuse(rgba(0), rgba(1), rgba(2), rgba(3));
      ign_material->SetSpecular(rgba(0), rgba(1), rgba(2), rgba(3));
    }
    return ign_material;
  }

  if (material->texture_filename.empty())
  {
    const Eigen::Vector4d& rgba = material->color;
    auto ign_material = scene.Material(material->getName());
    if (ign_material == nullptr)
    {
      ign_material = scene.CreateMaterial(material->getName());
      ign_material->SetAmbient(rgba(0), rgba(1), rgba(2), rgba(3));
      ign_material->SetDiffuse(rgba(0), rgba(1), rgba(2), rgba(3));
      ign_material->SetSpecular(rgba(0), rgba(1), rgba(2), rgba(3));
    }
    return ign_material;
  }

  return nullptr;
}

gz::rendering::VisualPtr loadContactResults(gz::rendering::Scene& scene,
                                            EntityContainer& entity_container,
                                            const tesseract_collision::ContactResultVector& contact_results)
{
  std::string name = "ContactResults";
  auto entity = entity_container.addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, name);
  gz::rendering::VisualPtr ign_visual = scene.CreateVisual(entity.id, entity.unique_name);
  for (const auto& cr : contact_results)
  {
    // For some reason the ignition arrow default length is 0.75m
    double dist = (cr.nearest_points[0] - cr.nearest_points[1]).norm();

    auto arrow = scene.CreateVisual();

    float head_size = 0.03;
    float shaft_diameter = 0.01;
    float shaft_length = dist - (2.0 * head_size);

    auto cone1 = scene.CreateVisual();
    cone1->AddGeometry(scene.CreateCone());
    cone1->SetOrigin(0, 0, 0);
    cone1->SetLocalPosition(0, 0, shaft_length + (3.0 * head_size / 2.0));
    cone1->SetLocalScale(head_size, head_size, head_size);
    arrow->AddChild(cone1);

    auto cone2 = scene.CreateVisual();
    cone2->AddGeometry(scene.CreateCone());
    cone2->SetOrigin(0, 0, 0);
    cone2->SetLocalPosition(0, 0, (head_size / 2.0));
    cone2->SetLocalRotation(M_PI, 0, 0);
    cone2->SetLocalScale(head_size, head_size, head_size);
    arrow->AddChild(cone2);

    if (shaft_length > 0)
    {
      auto cylinder = scene.CreateVisual();
      cylinder->AddGeometry(scene.CreateCylinder());
      cylinder->SetOrigin(0, 0, 0);
      cylinder->SetLocalPosition(0, 0, (shaft_length / 2.0) + head_size);
      cylinder->SetLocalScale(shaft_diameter, shaft_diameter, shaft_length);
      arrow->AddChild(cylinder);
    }

    arrow->SetInheritScale(false);
    arrow->SetVisible(true);

    // Set pose
    Eigen::Isometry3d pose{ Eigen::Isometry3d::Identity() };
    Eigen::Vector3d x, y, z;
    z = (cr.nearest_points[1] - cr.nearest_points[0]).normalized();
    y = z.unitOrthogonal();
    x = (y.cross(z)).normalized();
    Eigen::Matrix3d rot;
    rot.col(0) = x;
    rot.col(1) = y;
    rot.col(2) = z;
    pose.linear() = rot;
    pose.translation() = cr.nearest_points[0];
    arrow->SetLocalPose(gz::math::eigen3::convert(pose));

    // Assign material
    if (cr.distance > 0)
      arrow->SetMaterial("Default/TransGreen");
    else
      arrow->SetMaterial("Default/TransRed");

    ign_visual->AddChild(arrow);
  }
  return ign_visual;
}
}  // namespace tesseract_gui
