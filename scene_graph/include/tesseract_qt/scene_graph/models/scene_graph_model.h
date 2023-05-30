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
#ifndef TESSERACT_QT_SCENE_GRAPH_SCENE_GRAPH_MODEL_H
#define TESSERACT_QT_SCENE_GRAPH_SCENE_GRAPH_MODEL_H

#include <memory>
#include <QStandardItemModel>
#include <QObject>

namespace tesseract_scene_graph
{
class SceneGraph;
class Link;
class Joint;
}  // namespace tesseract_scene_graph

class QModelIndex;

namespace tesseract_gui
{
struct ComponentInfo;
class SceneGraphModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit SceneGraphModel(QObject* parent = nullptr);
  explicit SceneGraphModel(std::shared_ptr<const ComponentInfo> component_inf, QObject* parent = nullptr);
  ~SceneGraphModel() override;
  SceneGraphModel(const SceneGraphModel& other);
  SceneGraphModel& operator=(const SceneGraphModel& other);

  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;

  void setSceneGraph(const tesseract_scene_graph::SceneGraph& scene_graph);
  void setName(const std::string& name);
  const std::string& getName() const;
  void addLink(const tesseract_scene_graph::Link& link);
  void addJoint(const tesseract_scene_graph::Joint& joint);
  void removeLink(const std::string& link_name);
  void removeJoint(const std::string& joint_name);
  void clear();

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_SCENE_GRAPH_SCENE_GRAPH_MODEL_H
