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
#include <tesseract_qt/scene_graph/models/polygon_mesh_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_geometry/impl/convex_mesh.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
PolygonMeshStandardItem::PolygonMeshStandardItem(tesseract_geometry::PolygonMesh::Ptr mesh)
  : QStandardItem(icons::getConvexMeshIcon(), "PolygonMesh"), mesh(std::move(mesh))
{
  ctor();
}

PolygonMeshStandardItem::PolygonMeshStandardItem(const QString& text, tesseract_geometry::PolygonMesh::Ptr mesh)
  : QStandardItem(icons::getConvexMeshIcon(), text), mesh(std::move(mesh))
{
  ctor();
}

PolygonMeshStandardItem::PolygonMeshStandardItem(const QIcon& icon,
                                                 const QString& text,
                                                 tesseract_geometry::PolygonMesh::Ptr mesh)
  : QStandardItem(icon, text), mesh(std::move(mesh))
{
  ctor();
}

int PolygonMeshStandardItem::type() const { return static_cast<int>(StandardItemType::SG_POLYGON_MESH); }

void PolygonMeshStandardItem::ctor()
{
  if (mesh->getType() == tesseract_geometry::GeometryType::CONVEX_MESH)
  {
    setIcon(icons::getConvexMeshIcon());
    setText("Convex Mesh");

    std::string method{ "Default" };
    auto convex_mesh = std::static_pointer_cast<tesseract_geometry::ConvexMesh>(mesh);
    if (convex_mesh->getCreationMethod() == tesseract_geometry::ConvexMesh::MESH)
      method = "Mesh";
    else if (convex_mesh->getCreationMethod() == tesseract_geometry::ConvexMesh::CONVERTED)
      method = "Converted";

    appendRow(createStandardItemString("creation method", method));
  }
  else if (mesh->getType() == tesseract_geometry::GeometryType::MESH)
  {
    setIcon(icons::getMeshIcon());
    setText("Mesh");
  }
  else if (mesh->getType() == tesseract_geometry::GeometryType::SDF_MESH)
  {
    setIcon(icons::getMeshIcon());
    setText("SDF Mesh");
  }

  std::string url;
  if (mesh->getResource() != nullptr)
    url = mesh->getResource()->getUrl();

  appendRow(createStandardItemURL("resource", url));
  appendRow(createStandardItemInt("vertex count", mesh->getVertexCount()));
  appendRow(createStandardItemInt("face count", mesh->getFaceCount()));
}
}  // namespace tesseract_gui
