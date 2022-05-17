/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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
#include <tesseract_qt/scene_graph/visual_standard_item.h>
#include <tesseract_qt/scene_graph/material_standard_item.h>
#include <tesseract_qt/scene_graph/box_standard_item.h>
#include <tesseract_qt/scene_graph/capsule_standard_item.h>
#include <tesseract_qt/scene_graph/cone_standard_item.h>
#include <tesseract_qt/scene_graph/cylinder_standard_item.h>
#include <tesseract_qt/scene_graph/plane_standard_item.h>
#include <tesseract_qt/scene_graph/sphere_standard_item.h>
#include <tesseract_qt/scene_graph/polygon_mesh_standard_item.h>
#include <tesseract_qt/scene_graph/octree_standard_item.h>
#include <tesseract_qt/common/transform_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
VisualStandardItem::VisualStandardItem(tesseract_scene_graph::Visual::Ptr visual)
  : QStandardItem(icons::getVisualIcon(), "Visual"), visual(std::move(visual))
{
  ctor();
}

VisualStandardItem::VisualStandardItem(const QString& text, tesseract_scene_graph::Visual::Ptr visual)
  : QStandardItem(icons::getVisualIcon(), text), visual(std::move(visual))
{
  ctor();
}

VisualStandardItem::VisualStandardItem(const QIcon& icon,
                                       const QString& text,
                                       tesseract_scene_graph::Visual::Ptr visual)
  : QStandardItem(icon, text), visual(std::move(visual))
{
  ctor();
}

int VisualStandardItem::type() const { return static_cast<int>(StandardItemType::SG_VISUAL); }

void VisualStandardItem::ctor()
{
  appendRow(createStandardItemString("name", visual->name));
  appendRow(new TransformStandardItem(visual->origin));
  appendRow(new MaterialStandardItem(visual->material));

  QStandardItem* geometry_item;
  switch (visual->geometry->getType())
  {
    case tesseract_geometry::GeometryType::BOX:
    {
      geometry_item = new BoxStandardItem(std::static_pointer_cast<tesseract_geometry::Box>(visual->geometry));
      break;
    }
    case tesseract_geometry::GeometryType::CAPSULE:
    {
      geometry_item = new CapsuleStandardItem(std::static_pointer_cast<tesseract_geometry::Capsule>(visual->geometry));
      break;
    }
    case tesseract_geometry::GeometryType::CONE:
    {
      geometry_item = new ConeStandardItem(std::static_pointer_cast<tesseract_geometry::Cone>(visual->geometry));
      break;
    }
    case tesseract_geometry::GeometryType::CYLINDER:
    {
      geometry_item =
          new CylinderStandardItem(std::static_pointer_cast<tesseract_geometry::Cylinder>(visual->geometry));
      break;
    }
    case tesseract_geometry::GeometryType::PLANE:
    {
      geometry_item = new PlaneStandardItem(std::static_pointer_cast<tesseract_geometry::Plane>(visual->geometry));
      break;
    }
    case tesseract_geometry::GeometryType::SPHERE:
    {
      geometry_item = new SphereStandardItem(std::static_pointer_cast<tesseract_geometry::Sphere>(visual->geometry));
      break;
    }
    case tesseract_geometry::GeometryType::OCTREE:
    {
      geometry_item = new OctreeStandardItem(std::static_pointer_cast<tesseract_geometry::Octree>(visual->geometry));
      break;
    }
    case tesseract_geometry::GeometryType::CONVEX_MESH:
    case tesseract_geometry::GeometryType::MESH:
    case tesseract_geometry::GeometryType::SDF_MESH:
    {
      geometry_item =
          new PolygonMeshStandardItem(std::static_pointer_cast<tesseract_geometry::PolygonMesh>(visual->geometry));
      break;
    }
  }
  setIcon(geometry_item->icon());
  appendRow(geometry_item);
}
}  // namespace tesseract_gui
