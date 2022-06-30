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
#ifndef TESSERACT_QT_JOINT_STATE_SLIDER_WIDGET_H
#define TESSERACT_QT_JOINT_STATE_SLIDER_WIDGET_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_scene_graph/joint.h>
#include <memory>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <QWidget>

namespace Ui
{
class JointStateSliderWidget;
}

namespace tesseract_gui
{
struct JointStateSliderWidgetPrivate;
class JointStateSliderWidget : public QWidget
{
  Q_OBJECT

public:
  explicit JointStateSliderWidget(QWidget* parent = nullptr);
  ~JointStateSliderWidget();

  void setJoints(const std::vector<tesseract_scene_graph::Joint::ConstPtr>& joints);
  std::unordered_map<std::string, double> getJointState() const;

Q_SIGNALS:
  void jointValueChanged(QString name, double value);
  void jointStateChanged(std::unordered_map<std::string, double> state);

public Q_SLOTS:
  void setJointState(const std::unordered_map<std::string, double>& state);

private:
  std::unique_ptr<Ui::JointStateSliderWidget> ui_;
  std::unique_ptr<JointStateSliderWidgetPrivate> data_;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_JOINT_STATE_SLIDER_WIDGET_H
