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
#include <tesseract_qt/rendering/gazebo_utils.h>
#include <tesseract_qt/common/entity_container.h>
#include <tesseract_qt/common/events/status_log_events.h>

#include <tesseract_common/resource_locator.h>
#include <tesseract_geometry/geometries.h>
#include <tesseract_scene_graph/graph.h>
#include <tesseract_scene_graph/link.h>
#include <tesseract_scene_graph/scene_state.h>
#include <tesseract_geometry/geometry.h>
#include <tesseract_geometry/impl/polygon_mesh.h>
#include <tesseract_environment/environment.h>
#include <tesseract_environment/commands.h>
#include <tesseract_environment/events.h>

#include <gz/rendering/Scene.hh>
#include <gz/rendering/Visual.hh>
#include <gz/rendering/Material.hh>
#include <gz/math/AxisAlignedBox.hh>
#include <gz/common/SubMesh.hh>

#include <gz/math/eigen3/Conversions.hh>
#include <gz/common/Mesh.hh>
#include <gz/common/MeshManager.hh>

#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/WireBox.hh>
#include <gz/rendering/AxisVisual.hh>
#include <gz/rendering/Capsule.hh>

#include <QApplication>

const std::string USER_VISIBILITY = "user_visibility";
const std::string USER_PARENT_VISIBILITY = "user_parent_visibility";

namespace tesseract::gui
{
//////////////////////////////////////////////////
std::shared_ptr<gz::rendering::Scene> sceneFromFirstRenderEngine(const std::string& scene_name)
{
  auto loaded_eng_names = gz::rendering::loadedEngines();
  if (loaded_eng_names.empty())
  {
    events::StatusLogInfo event("No rendering engine is loaded yet");
    QApplication::sendEvent(qApp, &event);
    return nullptr;
  }

  auto engine_name = loaded_eng_names[0];
  if (loaded_eng_names.size() > 1)
  {
    events::StatusLogWarn event(QString("More than one engine is available. Using engine %1").arg(engine_name.c_str()));
    QApplication::sendEvent(qApp, &event);
  }

  auto* engine = gz::rendering::engine(engine_name);
  if (engine == nullptr)
  {
    events::StatusLogError event(QString("Internal error: failed to load engine %1").arg(engine_name.c_str()));
    QApplication::sendEvent(qApp, &event);
    return nullptr;
  }

  if (engine->SceneCount() == 0)
  {
    events::StatusLogInfo event("No scene has been created yet");
    QApplication::sendEvent(qApp, &event);
    return nullptr;
  }

  auto scene = engine->SceneByName(scene_name);
  if (scene == nullptr)
  {
    events::StatusLogError event("Internal error: scene is null.");
    QApplication::sendEvent(qApp, &event);
    return nullptr;
  }

  return scene;
}

//////////////////////////////////////////////////
std::shared_ptr<gz::rendering::Scene> sceneFromRenderEngine(const std::string& scene_name,
                                                            const std::string& engine_name)
{
  auto* engine = gz::rendering::engine(engine_name);
  if (engine == nullptr)
  {
    events::StatusLogError event(QString("Internal error: failed to load engine %1").arg(engine_name.c_str()));
    QApplication::sendEvent(qApp, &event);
    return nullptr;
  }

  if (engine->SceneCount() == 0)
  {
    events::StatusLogInfo event("No scene has been created yet");
    QApplication::sendEvent(qApp, &event);
    return nullptr;
  }

  auto scene = engine->SceneByName(scene_name);
  if (scene == nullptr)
  {
    events::StatusLogError event("Internal error: scene is null.");
    QApplication::sendEvent(qApp, &event);
    return nullptr;
  }

  return scene;
}

//////////////////////////////////////////////////
void setSceneState(gz::rendering::Scene& scene,
                   const tesseract::gui::EntityContainer& entity_container,
                   const tesseract::common::TransformMap& link_transforms)
{
  for (const auto& pair : link_transforms)
  {
    if (entity_container.hasTrackedEntity(EntityContainer::VISUAL_NS, pair.first))
    {
      Entity entity = entity_container.getTrackedEntity(EntityContainer::VISUAL_NS, pair.first);
      scene.VisualById(entity.id)->SetWorldPose(gz::math::eigen3::convert(pair.second));
    }
  }
}

//////////////////////////////////////////////////
void clearScene(gz::rendering::Scene& scene, EntityContainer& entity_container)
{
  for (const auto& ns : entity_container.getTrackedEntities())
  {
    for (const auto& entity : ns.second)
      scene.DestroyNodeById(entity.second.id);
  }

  for (const auto& ns : entity_container.getUntrackedEntities())
  {
    for (const auto& entity : ns.second)
      scene.DestroyNodeById(entity.id);
  }
}

//////////////////////////////////////////////////
gz::common::SubMesh convert(const tesseract::geometry::PolygonMesh& mesh)
{
  gz::common::SubMesh gz_submesh;
  const std::shared_ptr<const tesseract::common::VectorVector3d>& verticies = mesh.getVertices();
  const std::shared_ptr<const Eigen::VectorXi>& faces = mesh.getFaces();
  const std::shared_ptr<const tesseract::common::VectorVector3d>& normals = mesh.getNormals();

  if (normals == nullptr)
  {
    tesseract::common::VectorVector3d computed_normals;
    computed_normals.reserve(verticies->size());
    for (const Eigen::Vector3d& v : (*verticies))
    {
      computed_normals.push_back(Eigen::Vector3d(0, 0, 0));
      gz_submesh.AddVertex(gz::math::Vector3d(v.x(), v.y(), v.z()));
    }

    for (long t = 0; t < faces->size(); ++t)
    {
      size_t num_verts = static_cast<size_t>((*faces)[t]);
      Eigen::Vector3d side1 = (*verticies)[(*faces)[t + 1]] - (*verticies)[(*faces)[t + 2]];
      Eigen::Vector3d side2 = (*verticies)[(*faces)[t + 2]] - (*verticies)[(*faces)[t + 3]];
      Eigen::Vector3d normal = side1.cross(side2);
      normal.normalize();

      for (size_t k = 2; k < num_verts; ++k)
      {
        if (k == 2)
        {
          gz_submesh.AddIndex((*faces)[t + 1]);
          gz_submesh.AddIndex((*faces)[t + 2]);
          gz_submesh.AddIndex((*faces)[t + 3]);

          computed_normals[(*faces)[t + 1]] += normal;
          computed_normals[(*faces)[t + 2]] += normal;
          computed_normals[(*faces)[t + 3]] += normal;
        }
        else
        {
          gz_submesh.AddIndex((*faces)[t + 1]);
          gz_submesh.AddIndex((*faces)[t + k]);
          gz_submesh.AddIndex((*faces)[t + (k + 1)]);

          computed_normals[(*faces)[t + (k + 1)]] += normal;
        }
      }

      t += num_verts;
    }

    for (size_t i = 0; i < verticies->size(); ++i)
    {
      auto& n = computed_normals[i];
      n.normalize();
      gz_submesh.AddNormal(gz::math::Vector3d(n.x(), n.y(), n.z()));
    }
  }
  else
  {
    for (long t = 0; t < verticies->size(); ++t)
    {
      const Eigen::Vector3d& v = (*verticies)[t];
      const Eigen::Vector3d& n = (*normals)[t];

      gz_submesh.AddVertex(gz::math::Vector3d(v.x(), v.y(), v.z()));
      gz_submesh.AddNormal(gz::math::Vector3d(n.x(), n.y(), n.z()));
    }

    for (long t = 0; t < faces->size(); ++t)
    {
      size_t num_verts = static_cast<size_t>((*faces)[t]);
      for (size_t k = 2; k < num_verts; ++k)
      {
        if (k == 2)
        {
          gz_submesh.AddIndex((*faces)[t + 1]);
          gz_submesh.AddIndex((*faces)[t + 2]);
          gz_submesh.AddIndex((*faces)[t + 3]);
        }
        else
        {
          gz_submesh.AddIndex((*faces)[t + 1]);
          gz_submesh.AddIndex((*faces)[t + k]);
          gz_submesh.AddIndex((*faces)[t + (k + 1)]);
        }
      }

      t += num_verts;
    }
  }

  return gz_submesh;
}

//////////////////////////////////////////////////
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

//////////////////////////////////////////////////
std::vector<std::string> loadSceneGraph(gz::rendering::Scene& scene,
                                        tesseract::gui::EntityContainer& entity_container,
                                        const tesseract::scene_graph::SceneGraph& scene_graph,
                                        const std::string& prefix)
{
  std::vector<std::string> link_names;
  std::shared_ptr<gz::rendering::Visual> root = scene.RootVisual();
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

std::shared_ptr<gz::rendering::Visual> loadLink(gz::rendering::Scene& scene,
                                                tesseract::gui::EntityContainer& entity_container,
                                                const tesseract::scene_graph::Link& link)
{
  auto entity = entity_container.addTrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS, link.getName());
  std::shared_ptr<gz::rendering::Visual> ign_link = scene.CreateVisual(entity.id, entity.unique_name);
  ign_link->SetUserData(USER_VISIBILITY, true);

  std::shared_ptr<gz::rendering::Visual> ign_link_visuals = loadLinkVisuals(scene, entity_container, link);
  ign_link_visuals->SetUserData(USER_VISIBILITY, true);
  ign_link->AddChild(ign_link_visuals);

  std::shared_ptr<gz::rendering::Visual> ign_link_collisions = loadLinkCollisions(scene, entity_container, link);
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

  // Cannot set visibilty to false in loadLinkCollisions because LocalBoundingBox only calculates for visible objects
  ign_link_collisions->SetVisible(false);

  return ign_link;
}

//////////////////////////////////////////////////
std::shared_ptr<gz::rendering::Visual> loadLinkVisuals(gz::rendering::Scene& scene,
                                                       EntityContainer& entity_container,
                                                       const tesseract::scene_graph::Link& link)
{
  std::string name = link.getName() + "::Visuals";
  auto entity = entity_container.addTrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS, name);
  std::shared_ptr<gz::rendering::Visual> ign_link_visuals = scene.CreateVisual(entity.id, entity.unique_name);

  for (const auto& visual : link.visual)
    ign_link_visuals->AddChild(loadLinkGeometry(
        scene, entity_container, *visual->geometry, Eigen::Vector3d::Ones(), visual->origin, visual->material));

  return ign_link_visuals;
}

//////////////////////////////////////////////////
std::shared_ptr<gz::rendering::Visual> loadLinkCollisions(gz::rendering::Scene& scene,
                                                          EntityContainer& entity_container,
                                                          const tesseract::scene_graph::Link& link)
{
  std::string name = link.getName() + "::Collisions";
  auto entity = entity_container.addTrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS, name);
  std::shared_ptr<gz::rendering::Visual> ign_link_collisions = scene.CreateVisual(entity.id, entity.unique_name);

  for (const auto& collision : link.collision)
    ign_link_collisions->AddChild(loadLinkGeometry(
        scene, entity_container, *collision->geometry, Eigen::Vector3d::Ones(), collision->origin, nullptr));

  ign_link_collisions->SetVisible(true);
  return ign_link_collisions;
}

//////////////////////////////////////////////////
std::shared_ptr<gz::rendering::Visual> loadLinkWireBox(gz::rendering::Scene& scene,
                                                       EntityContainer& entity_container,
                                                       const tesseract::scene_graph::Link& link,
                                                       const gz::math::AxisAlignedBox& aabb)
{
  std::string name = link.getName() + "::WireBox";
  auto entity = entity_container.addTrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS, name);

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
  std::shared_ptr<gz::rendering::Visual> wire_box_vis = scene.CreateVisual(entity.id, entity.unique_name);
  wire_box_vis->SetInheritScale(false);
  wire_box_vis->AddGeometry(wire_box);
  wire_box_vis->SetMaterial(white, false);
  wire_box_vis->SetVisible(false);

  return wire_box_vis;
}

//////////////////////////////////////////////////
std::shared_ptr<gz::rendering::Visual> loadLinkAxis(gz::rendering::Scene& scene,
                                                    EntityContainer& entity_container,
                                                    const tesseract::scene_graph::Link& link)
{
  std::string name = link.getName() + "::Axis";
  auto entity = entity_container.addTrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS, name);

  gz::rendering::AxisVisualPtr axis = scene.CreateAxisVisual(entity.id, entity.unique_name);
  axis->SetInheritScale(false);
  axis->Scale(0.1, 0.1, 0.1);
  axis->SetVisible(true);

  return axis;
}

//////////////////////////////////////////////////
std::shared_ptr<gz::rendering::Visual>
loadLinkGeometry(gz::rendering::Scene& scene,
                 tesseract::gui::EntityContainer& entity_container,
                 const tesseract::geometry::Geometry& geometry,
                 const Eigen::Vector3d& scale,
                 const Eigen::Isometry3d& local_pose,
                 const std::shared_ptr<const tesseract::scene_graph::Material>& material)
{
  gz::rendering::MaterialPtr ign_material = loadMaterial(scene, material);
  switch (geometry.getType())
  {
    case tesseract::geometry::GeometryType::BOX:
    {
      auto gv_entity = entity_container.addUntrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS);
      std::shared_ptr<gz::rendering::Visual> box = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      box->SetLocalPose(gz::math::eigen3::convert(local_pose));
      box->AddGeometry(scene.CreateBox());

      const auto& shape = static_cast<const tesseract::geometry::Box&>(geometry);
      box->Scale(shape.getX() * scale.x(), shape.getY() * scale.y(), shape.getZ() * scale.z());
      box->SetMaterial(ign_material);
      return box;
    }
    case tesseract::geometry::GeometryType::SPHERE:
    {
      auto gv_entity = entity_container.addUntrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS);
      std::shared_ptr<gz::rendering::Visual> sphere = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      sphere->SetLocalPose(gz::math::eigen3::convert(local_pose));
      sphere->AddGeometry(scene.CreateSphere());

      const auto& shape = static_cast<const tesseract::geometry::Sphere&>(geometry);
      const double diameter = 2.0 * shape.getRadius();
      sphere->Scale(diameter * scale.x(), diameter * scale.y(), diameter * scale.z());
      sphere->SetMaterial(ign_material);
      return sphere;
    }
    case tesseract::geometry::GeometryType::CYLINDER:
    {
      auto gv_entity = entity_container.addUntrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS);
      std::shared_ptr<gz::rendering::Visual> cylinder = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      cylinder->SetLocalPose(gz::math::eigen3::convert(local_pose));
      cylinder->AddGeometry(scene.CreateCylinder());

      const auto& shape = static_cast<const tesseract::geometry::Cylinder&>(geometry);
      const double diameter = 2.0 * shape.getRadius();
      cylinder->Scale(diameter * scale.x(), diameter * scale.y(), shape.getLength() * scale.z());
      cylinder->SetMaterial(ign_material);
      return cylinder;
    }
    case tesseract::geometry::GeometryType::CONE:
    {
      auto gv_entity = entity_container.addUntrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS);
      std::shared_ptr<gz::rendering::Visual> cone = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      cone->SetLocalPose(gz::math::eigen3::convert(local_pose));
      cone->AddGeometry(scene.CreateCone());

      const auto& shape = static_cast<const tesseract::geometry::Cone&>(geometry);
      const double diameter = 2.0 * shape.getRadius();
      cone->Scale(diameter * scale.x(), diameter * scale.y(), shape.getLength() * scale.z());
      cone->SetMaterial(ign_material);
      return cone;
    }
    case tesseract::geometry::GeometryType::CAPSULE:
    {
      auto gv_entity = entity_container.addUntrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS);
      std::shared_ptr<gz::rendering::Visual> capsule = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      capsule->SetLocalPose(gz::math::eigen3::convert(local_pose));
      // Capsule is a special shape and should not use scale to change the size of the shape
      gz::rendering::CapsulePtr geom = scene.CreateCapsule();
      const auto& shape = static_cast<const tesseract::geometry::Capsule&>(geometry);
      geom->SetRadius(shape.getRadius());
      geom->SetLength(shape.getLength());
      capsule->AddGeometry(geom);
      capsule->SetMaterial(ign_material);
      return capsule;
    }
    case tesseract::geometry::GeometryType::MESH:
    {
      const auto& shape = static_cast<const tesseract::geometry::Mesh&>(geometry);
      auto resource = shape.getResource();

      auto gv_entity = entity_container.addUntrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS);
      std::shared_ptr<gz::rendering::Visual> mesh = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      mesh->SetLocalPose(gz::math::eigen3::convert(local_pose));
      gz::common::MeshManager* mesh_manager = gz::common::MeshManager::Instance();
      gz::rendering::MeshPtr mesh_geom;
      if (resource)
      {
        gz::rendering::MeshDescriptor descriptor;
        descriptor.meshName = resource->getFilePath();
        descriptor.mesh = mesh_manager->Load(descriptor.meshName);
        mesh_geom = scene.CreateMesh(descriptor);

        if (!isMeshWithColor(resource->getFilePath()))
          mesh_geom->SetMaterial(ign_material);
      }
      else
      {
        std::string name = std::to_string(std::hash<const tesseract::geometry::Geometry*>{}(&geometry));

        const gz::common::Mesh* gz_mesh{ nullptr };
        if (!mesh_manager->HasMesh(name))
        {
          auto* nm = new gz::common::Mesh();
          nm->SetName(name);
          nm->AddSubMesh(convert(shape));
          mesh_manager->AddMesh(nm);
          gz_mesh = nm;
        }
        else
        {
          gz_mesh = mesh_manager->MeshByName(name);
        }

        gz::rendering::MeshDescriptor descriptor;
        descriptor.meshName = name;
        descriptor.mesh = gz_mesh;
        mesh_geom = scene.CreateMesh(descriptor);
        mesh_geom->SetMaterial(ign_material);
      }

      mesh->AddGeometry(mesh_geom);
      mesh->SetLocalScale(shape.getScale().x(), shape.getScale().y(), shape.getScale().z());
      return mesh;
    }
    case tesseract::geometry::GeometryType::CONVEX_MESH:
    {
      const auto& shape = static_cast<const tesseract::geometry::ConvexMesh&>(geometry);
      auto resource = shape.getResource();
      auto gv_entity = entity_container.addUntrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS);
      std::shared_ptr<gz::rendering::Visual> mesh = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      mesh->SetLocalPose(gz::math::eigen3::convert(local_pose));
      gz::common::MeshManager* mesh_manager = gz::common::MeshManager::Instance();
      gz::rendering::MeshPtr mesh_geom;
      if (resource && shape.getCreationMethod() != tesseract::geometry::ConvexMesh::CreationMethod::CONVERTED)
      {
        gz::rendering::MeshDescriptor descriptor;
        descriptor.meshName = resource->getFilePath();
        descriptor.mesh = mesh_manager->Load(descriptor.meshName);
        mesh_geom = scene.CreateMesh(descriptor);

        if (!isMeshWithColor(resource->getFilePath()))
          mesh_geom->SetMaterial(ign_material);
      }
      else
      {
        std::string name = std::to_string(std::hash<const tesseract::geometry::Geometry*>{}(&geometry));
        if (resource)
          name = resource->getFilePath() + "::CONVERTED_CONVEX_HULL";

        const gz::common::Mesh* gz_mesh{ nullptr };
        if (!mesh_manager->HasMesh(name))
        {
          auto* nm = new gz::common::Mesh();
          nm->SetName(name);
          nm->AddSubMesh(convert(shape));
          mesh_manager->AddMesh(nm);
          gz_mesh = nm;
        }
        else
        {
          gz_mesh = mesh_manager->MeshByName(name);
        }

        gz::rendering::MeshDescriptor descriptor;
        descriptor.meshName = name;
        descriptor.mesh = gz_mesh;
        mesh_geom = scene.CreateMesh(descriptor);
        mesh_geom->SetMaterial(ign_material);
      }

      mesh->AddGeometry(mesh_geom);
      mesh->SetLocalScale(shape.getScale().x(), shape.getScale().y(), shape.getScale().z());
      return mesh;
    }
    case tesseract::geometry::GeometryType::OCTREE:
    {
      const auto& shape = static_cast<const tesseract::geometry::Octree&>(geometry);

      // TODO: Need to implement
      assert(false);
      return nullptr;
    }
    case tesseract::geometry::GeometryType::COMPOUND_MESH:
    {
      const auto& shape = static_cast<const tesseract::geometry::CompoundMesh&>(geometry);
      auto resource = shape.getResource();

      auto gv_entity = entity_container.addUntrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS);
      std::shared_ptr<gz::rendering::Visual> mesh = scene.CreateVisual(gv_entity.id, gv_entity.unique_name);
      mesh->SetLocalPose(gz::math::eigen3::convert(local_pose));
      gz::common::MeshManager* mesh_manager = gz::common::MeshManager::Instance();

      const auto& sub_meshes = shape.getMeshes();
      if (sub_meshes.front()->getType() == tesseract::geometry::GeometryType::CONVEX_MESH)
      {
        if (resource && static_cast<const tesseract::geometry::ConvexMesh&>(*sub_meshes.front()).getCreationMethod() !=
                            tesseract::geometry::ConvexMesh::CreationMethod::CONVERTED)
        {
          gz::rendering::MeshDescriptor descriptor;
          descriptor.meshName = resource->getFilePath();
          descriptor.mesh = mesh_manager->Load(descriptor.meshName);
          gz::rendering::MeshPtr mesh_geom = scene.CreateMesh(descriptor);

          if (!isMeshWithColor(resource->getFilePath()))
            mesh_geom->SetMaterial(ign_material);

          mesh->AddGeometry(mesh_geom);
          mesh->SetLocalScale(shape.getScale().x(), shape.getScale().y(), shape.getScale().z());
          return mesh;
        }

        for (const auto& sub_mesh : sub_meshes)
        {
          const std::string geom_hash_str =
              std::to_string(std::hash<const tesseract::geometry::Geometry*>{}(sub_mesh.get()));
          std::string name{ geom_hash_str };
          if (resource)
          {
            name.append(resource->getFilePath());
            name.append("::");
            name.append(geom_hash_str);
            name.append("::CONVERTED_CONVEX_HULL");
          }

          const gz::common::Mesh* gz_mesh{ nullptr };
          if (!mesh_manager->HasMesh(name))
          {
            auto* nm = new gz::common::Mesh();
            nm->SetName(name);
            nm->AddSubMesh(convert(*sub_mesh));
            mesh_manager->AddMesh(nm);
            gz_mesh = nm;
          }
          else
          {
            gz_mesh = mesh_manager->MeshByName(name);
          }

          gz::rendering::MeshDescriptor descriptor;
          descriptor.meshName = name;
          descriptor.mesh = gz_mesh;
          gz::rendering::MeshPtr mesh_geom = scene.CreateMesh(descriptor);
          mesh_geom->SetMaterial(ign_material);
          mesh->AddGeometry(mesh_geom);
        }

        mesh->SetLocalScale(shape.getScale().x(), shape.getScale().y(), shape.getScale().z());
        return mesh;
      }

      if (sub_meshes.front()->getType() == tesseract::geometry::GeometryType::MESH)
      {
        if (resource)
        {
          gz::rendering::MeshDescriptor descriptor;
          descriptor.meshName = resource->getFilePath();
          descriptor.mesh = mesh_manager->Load(descriptor.meshName);
          gz::rendering::MeshPtr mesh_geom = scene.CreateMesh(descriptor);

          if (!isMeshWithColor(resource->getFilePath()))
            mesh_geom->SetMaterial(ign_material);

          mesh->AddGeometry(mesh_geom);
          mesh->SetLocalScale(shape.getScale().x(), shape.getScale().y(), shape.getScale().z());
          return mesh;
        }

        for (const auto& sub_mesh : sub_meshes)
        {
          std::string name = std::to_string(std::hash<const tesseract::geometry::Geometry*>{}(sub_mesh.get()));

          const gz::common::Mesh* gz_mesh{ nullptr };
          if (!mesh_manager->HasMesh(name))
          {
            auto* nm = new gz::common::Mesh();
            nm->SetName(name);
            nm->AddSubMesh(convert(*sub_mesh));
            mesh_manager->AddMesh(nm);
            gz_mesh = nm;
          }
          else
          {
            gz_mesh = mesh_manager->MeshByName(name);
          }

          gz::rendering::MeshDescriptor descriptor;
          descriptor.meshName = name;
          descriptor.mesh = gz_mesh;
          gz::rendering::MeshPtr mesh_geom = scene.CreateMesh(descriptor);
          mesh_geom->SetMaterial(ign_material);
          mesh->AddGeometry(mesh_geom);
        }
        mesh->SetLocalScale(shape.getScale().x(), shape.getScale().y(), shape.getScale().z());
        return mesh;
      }

      if (sub_meshes.front()->getType() == tesseract::geometry::GeometryType::SDF_MESH)
        throw std::runtime_error("SDF Mesh, currently not supported!");
    }
    default:
    {
      return nullptr;
    }
  }
}

//////////////////////////////////////////////////
std::shared_ptr<gz::rendering::Material>
loadMaterial(gz::rendering::Scene& scene, const std::shared_ptr<const tesseract::scene_graph::Material>& material)
{
  if (material == nullptr)
  {
    auto default_material = tesseract::scene_graph::Material::getDefaultMaterial();
    const Eigen::Vector4d& rgba = default_material->color;
    auto ign_material = scene.Material(default_material->getName());
    if (ign_material == nullptr)
    {
      ign_material = scene.CreateMaterial(default_material->getName());
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

//////////////////////////////////////////////////
std::shared_ptr<gz::rendering::Visual>
loadContactResults(gz::rendering::Scene& scene,
                   EntityContainer& entity_container,
                   const tesseract::collision::ContactResultVector& contact_results)
{
  std::string name = "ContactResults";
  auto entity = entity_container.addTrackedEntity(tesseract::gui::EntityContainer::VISUAL_NS, name);
  std::shared_ptr<gz::rendering::Visual> ign_visual = scene.CreateVisual(entity.id, entity.unique_name);
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

void tesseractEventFilter(const tesseract::environment::Event& event,
                          const tesseract::environment::Environment& env,
                          int& current_revision,
                          gz::rendering::Scene& scene,
                          EntityContainer& entity_container)
{
  if (!env.isInitialized())
    return;

  switch (event.type)
  {
    case tesseract::environment::Events::COMMAND_APPLIED:
    {
      bool reset{ false };
      const auto& e = static_cast<const tesseract::environment::CommandAppliedEvent&>(event);
      if (current_revision == 0 || e.revision < current_revision)
      {
        reset = true;
      }
      else
      {
        /**
         * @todo update to handle explicit commands
         * @note See ign_scene_graph_render_manager.cpp
         */
        for (std::size_t i = current_revision; i < e.revision; ++i)
        {
          const auto& cmd = e.commands.at(i);
          switch (cmd->getType())
          {
            case tesseract::environment::CommandType::ADD_SCENE_GRAPH:
            case tesseract::environment::CommandType::ADD_LINK:
            case tesseract::environment::CommandType::ADD_TRAJECTORY_LINK:
            case tesseract::environment::CommandType::CHANGE_LINK_VISIBILITY:
            case tesseract::environment::CommandType::REMOVE_LINK:
            case tesseract::environment::CommandType::REMOVE_JOINT:
            {
              reset = true;
              break;
            }
            case tesseract::environment::CommandType::MOVE_LINK:
            case tesseract::environment::CommandType::MOVE_JOINT:
            case tesseract::environment::CommandType::REPLACE_JOINT:
            case tesseract::environment::CommandType::CHANGE_JOINT_ORIGIN:
            case tesseract::environment::CommandType::CHANGE_LINK_ORIGIN:
            case tesseract::environment::CommandType::CHANGE_LINK_COLLISION_ENABLED:
            case tesseract::environment::CommandType::MODIFY_ALLOWED_COLLISIONS:
            case tesseract::environment::CommandType::REMOVE_ALLOWED_COLLISION_LINK:
            case tesseract::environment::CommandType::CHANGE_JOINT_POSITION_LIMITS:
            case tesseract::environment::CommandType::CHANGE_JOINT_VELOCITY_LIMITS:
            case tesseract::environment::CommandType::CHANGE_JOINT_ACCELERATION_LIMITS:
            case tesseract::environment::CommandType::ADD_KINEMATICS_INFORMATION:
            case tesseract::environment::CommandType::CHANGE_COLLISION_MARGINS:
            case tesseract::environment::CommandType::ADD_CONTACT_MANAGERS_PLUGIN_INFO:
            case tesseract::environment::CommandType::SET_ACTIVE_CONTINUOUS_CONTACT_MANAGER:
            case tesseract::environment::CommandType::SET_ACTIVE_DISCRETE_CONTACT_MANAGER:
            {
              break;
            }
            // LCOV_EXCL_START
            default:
            {
              events::StatusLogError event("Tesseract Qt Gazebo Utils, Unhandled environment command");
              QApplication::sendEvent(qApp, &event);
            }
          }
        }
      }
      if (reset)
      {
        // Clear Scene
        clearScene(scene, entity_container);
        entity_container.clear();

        auto lock = env.lockRead();
        loadSceneGraph(scene, entity_container, *(env.getSceneGraph()), "");
      }
      current_revision = e.revision;
      break;
    }
    case tesseract::environment::Events::SCENE_STATE_CHANGED:
    {
      const auto& e = static_cast<const tesseract::environment::SceneStateChangedEvent&>(event);
      setSceneState(scene, entity_container, e.state.link_transforms);
      break;
    }
  }
}
}  // namespace tesseract::gui
