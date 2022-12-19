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
  explicit SceneStateModel(std::string scene_name = "", QObject* parent = nullptr);
  ~SceneStateModel() override;
  SceneStateModel(const SceneStateModel& other);
  SceneStateModel& operator=(const SceneStateModel& other);

  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

  // Caution when using methods below. In most cases you should use application events.
  void setState(const tesseract_scene_graph::SceneState& scene_state);
  void clear();

private:
  std::string scene_name_;
  std::unique_ptr<SceneStateModelPrivate> data_;

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_SCENE_GRAPH_SCENE_STATE_MODEL_H
