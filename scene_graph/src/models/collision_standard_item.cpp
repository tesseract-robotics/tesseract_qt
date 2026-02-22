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
#include <tesseract_qt/scene_graph/models/collision_standard_item.h>
#include <tesseract_qt/scene_graph/models/box_standard_item.h>
#include <tesseract_qt/scene_graph/models/capsule_standard_item.h>
#include <tesseract_qt/scene_graph/models/cone_standard_item.h>
#include <tesseract_qt/scene_graph/models/cylinder_standard_item.h>
#include <tesseract_qt/scene_graph/models/plane_standard_item.h>
#include <tesseract_qt/scene_graph/models/sphere_standard_item.h>
#include <tesseract_qt/scene_graph/models/polygon_mesh_standard_item.h>
#include <tesseract_qt/scene_graph/models/octree_standard_item.h>
#include <tesseract_qt/common/models/transform_standard_item.h>
#include <tesseract_qt/common/models/type_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_scene_graph/link.h>
#include <tesseract_geometry/geometry.h>
#include <tesseract_geometry/geometries.h>

namespace tesseract::gui
{
CollisionStandardItem::CollisionStandardItem(std::shared_ptr<tesseract::scene_graph::Collision> collision)
  : QStandardItem(icons::getCollisionIcon(), "Collision"), collision(std::move(collision))
{
  ctor();
}

CollisionStandardItem::CollisionStandardItem(const QString& text,
                                             std::shared_ptr<tesseract::scene_graph::Collision> collision)
  : QStandardItem(icons::getCollisionIcon(), text), collision(std::move(collision))
{
  ctor();
}
CollisionStandardItem::CollisionStandardItem(const QIcon& icon,
                                             const QString& text,
                                             std::shared_ptr<tesseract::scene_graph::Collision> collision)
  : QStandardItem(icon, text), collision(std::move(collision))
{
  ctor();
}

int CollisionStandardItem::type() const { return static_cast<int>(StandardItemType::SG_COLLISION); }

inline QStandardItem* getGeometryItem(const std::shared_ptr<const tesseract::geometry::Geometry>& geometry)
{
  QStandardItem* geometry_item{ nullptr };
  switch (geometry->getType())
  {
    case tesseract::geometry::GeometryType::BOX:
    {
      geometry_item = new BoxStandardItem(std::static_pointer_cast<const tesseract::geometry::Box>(geometry));
      break;
    }
    case tesseract::geometry::GeometryType::CAPSULE:
    {
      geometry_item = new CapsuleStandardItem(std::static_pointer_cast<const tesseract::geometry::Capsule>(geometry));
      break;
    }
    case tesseract::geometry::GeometryType::CONE:
    {
      geometry_item = new ConeStandardItem(std::static_pointer_cast<const tesseract::geometry::Cone>(geometry));
      break;
    }
    case tesseract::geometry::GeometryType::CYLINDER:
    {
      geometry_item = new CylinderStandardItem(std::static_pointer_cast<const tesseract::geometry::Cylinder>(geometry));
      break;
    }
    case tesseract::geometry::GeometryType::PLANE:
    {
      geometry_item = new PlaneStandardItem(std::static_pointer_cast<const tesseract::geometry::Plane>(geometry));
      break;
    }
    case tesseract::geometry::GeometryType::SPHERE:
    {
      geometry_item = new SphereStandardItem(std::static_pointer_cast<const tesseract::geometry::Sphere>(geometry));
      break;
    }
    case tesseract::geometry::GeometryType::OCTREE:
    {
      geometry_item = new OctreeStandardItem(std::static_pointer_cast<const tesseract::geometry::Octree>(geometry));
      break;
    }
    case tesseract::geometry::GeometryType::CONVEX_MESH:
    case tesseract::geometry::GeometryType::MESH:
    case tesseract::geometry::GeometryType::POLYGON_MESH:
    case tesseract::geometry::GeometryType::SDF_MESH:
    {
      geometry_item =
          new PolygonMeshStandardItem(std::static_pointer_cast<const tesseract::geometry::PolygonMesh>(geometry));
      break;
    }
    case tesseract::geometry::GeometryType::COMPOUND_MESH:
    {
      geometry_item = new TypeStandardItem(
          icons::getVisualVectorIcon(), "CompoundMesh", static_cast<int>(StandardItemType::SG_COMPOUND_MESH));
      auto meshes = std::static_pointer_cast<const tesseract::geometry::CompoundMesh>(geometry)->getMeshes();
      for (const auto& mesh : meshes)
        geometry_item->appendRow(getGeometryItem(mesh));

      if (meshes.front()->getType() == tesseract::geometry::GeometryType::MESH ||
          meshes.front()->getType() == tesseract::geometry::GeometryType::SDF_MESH)
        geometry_item->setIcon(icons::getMeshIcon());
      else if (meshes.front()->getType() == tesseract::geometry::GeometryType::CONVEX_MESH)
        geometry_item->setIcon(icons::getConvexMeshIcon());
    }
  }
  return geometry_item;
}

void CollisionStandardItem::ctor()
{
  appendRow(createStandardItemString("name", collision->name));
  appendRow(new TransformStandardItem(collision->origin));

  QStandardItem* geometry_item = getGeometryItem(collision->geometry);
  setIcon(geometry_item->icon());

  appendRow(geometry_item);
}
}  // namespace tesseract::gui
