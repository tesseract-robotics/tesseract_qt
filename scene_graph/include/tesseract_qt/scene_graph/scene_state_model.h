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
#ifndef TESSERACT_QT_SCENE_GRAPH_SCENE_STATE_MODEL_H
#define TESSERACT_QT_SCENE_GRAPH_SCENE_STATE_MODEL_H

#include <memory>
#include <QStandardItemModel>

namespace tesseract_scene_graph
{
class SceneState;
}

namespace tesseract_gui
{
class SceneStateModelPrivate;
class SceneStateModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit SceneStateModel(QObject* parent = nullptr);
  ~SceneStateModel() override;
  SceneStateModel(const SceneStateModel& other);
  SceneStateModel& operator=(const SceneStateModel& other);

  void setState(const tesseract_scene_graph::SceneState& scene_state);

  void clear();

public Q_SLOTS:
  virtual void onLinkAxisCheckedStateChanged(const QString& link_name, bool checked);

private:
  std::unique_ptr<SceneStateModelPrivate> data_;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_SCENE_GRAPH_SCENE_STATE_MODEL_H
