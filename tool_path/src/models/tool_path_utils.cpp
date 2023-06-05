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

#include <tesseract_qt/tool_path/models/tool_path_utils.h>
#include <tesseract_qt/common/models/transform_standard_item.h>
#include <tesseract_qt/common/models/tool_path_segment_standard_item.h>
#include <tesseract_qt/common/models/tool_path_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/events/tool_path_events.h>

#include <tesseract_common/yaml_utils.h>

#include <tesseract_common/eigen_serialization.h>
#include <tesseract_common/serialization.h>
#include <boost/serialization/vector.hpp>

#include <QStandardItem>
#include <QApplication>
#include <QString>
#include <QDir>

namespace tesseract_gui
{
TransformStandardItem* findTransformItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::COMMON_TRANSFORM))
    return dynamic_cast<TransformStandardItem*>(item);

  return findTransformItem(item->parent());
}

ToolPathSegmentStandardItem* findToolPathSegmentItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH_SEGMENT))
    return dynamic_cast<ToolPathSegmentStandardItem*>(item);

  return findToolPathSegmentItem(item->parent());
}

ToolPathStandardItem* findToolPathItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH))
    return dynamic_cast<ToolPathStandardItem*>(item);

  return findToolPathItem(item->parent());
}

void setCheckedStateRecursive(QStandardItem* item, Qt::CheckState st)
{
  if (item->isCheckable())
    item->setCheckState(st);

  for (int i = 0; i < item->rowCount(); i++)
    setCheckedStateRecursive(item->child(i), st);
}

bool openToolPath(const std::shared_ptr<const ComponentInfo>& component_info,
                  const QString& filename,
                  const QString& link_name)
{
  QFileInfo file_info(filename);
  if (file_info.suffix() == "tpx")
  {
    auto tool_path =
        tesseract_common::Serialization::fromArchiveFileXML<tesseract_common::Toolpath>(filename.toStdString());
    ToolPath qt_tool_path(tool_path, link_name.toStdString());
    QApplication::sendEvent(qApp, new events::ToolPathAdd(component_info, std::move(qt_tool_path)));
    return true;
  }

  if (file_info.suffix() == "tpb")
  {
    auto tool_path =
        tesseract_common::Serialization::fromArchiveFileBinary<tesseract_common::Toolpath>(filename.toStdString());
    ToolPath qt_tool_path(tool_path, link_name.toStdString());
    QApplication::sendEvent(qApp, new events::ToolPathAdd(component_info, std::move(qt_tool_path)));
    return true;
  }

  if (file_info.suffix() == "yaml" || file_info.suffix() == "json")
  {
    YAML::Node node = YAML::LoadFile(filename.toStdString());
    auto tool_path = node.as<tesseract_common::Toolpath>();
    ToolPath qt_tool_path(tool_path, link_name.toStdString());
    QApplication::sendEvent(qApp, new events::ToolPathAdd(component_info, std::move(qt_tool_path)));
    return true;
  }

  return false;
}

bool saveToolPath(const tesseract_common::Toolpath& tool_path, QString filename)
{
  QFileInfo file_info(filename);
  if (file_info.suffix() == "tpx")
  {
    tesseract_common::Serialization::toArchiveFileXML<tesseract_common::Toolpath>(tool_path, filename.toStdString());
    return true;
  }

  if (file_info.suffix() == "tpb")
  {
    tesseract_common::Serialization::toArchiveFileBinary<tesseract_common::Toolpath>(tool_path, filename.toStdString());
    return true;
  }

  if (file_info.suffix() == "yaml")
  {
    YAML::Node node{ tool_path };
    std::ofstream fout(filename.toStdString());
    fout << node;
    return true;
  }

  return false;
}
}  // namespace tesseract_gui
