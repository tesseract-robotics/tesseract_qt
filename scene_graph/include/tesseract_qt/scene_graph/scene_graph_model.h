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
}

class QModelIndex;

namespace tesseract_gui
{
class SceneGraphModelImpl;
class SceneGraphModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit SceneGraphModel(QObject* parent = nullptr);
  ~SceneGraphModel() override;
  SceneGraphModel(const SceneGraphModel& other);
  SceneGraphModel& operator=(const SceneGraphModel& other);

  void setSceneGraph(std::unique_ptr<tesseract_scene_graph::SceneGraph> scene_graph);

  void clear();

public Q_SLOTS:
  virtual void onLinkCheckedStateChanged(const QString& link_name, bool checked);
  virtual void onLinkVisualsCheckedStateChanged(const QString& link_name, bool checked);
  virtual void onLinkCollisionsCheckedStateChanged(const QString& link_name, bool checked);

private:
  std::unique_ptr<SceneGraphModelImpl> data_;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_SCENE_GRAPH_SCENE_GRAPH_MODEL_H
