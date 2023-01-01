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
#ifndef TESSERACT_QT_JOINT_TRAJECTORY_JOINT_TRAJECTORY_PLOT_DIALOG_H
#define TESSERACT_QT_JOINT_TRAJECTORY_JOINT_TRAJECTORY_PLOT_DIALOG_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
//#include <tesseract_common/joint_trajectory_set.h>
#include <tesseract_qt/common/joint_trajectory_set.h>
#include <memory>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <QDialog>
#include <tesseract_qt/plot/plot_data.h>

namespace Ui
{
class JointTrajectoryPlotDialog;
}

namespace tesseract_gui
{
class PlotSubplots;

class JointTrajectoryPlotDialog : public QDialog
{
  Q_OBJECT

public:
  explicit JointTrajectoryPlotDialog(tesseract_common::JointTrajectoryInfo current_trajectory,
                                     QWidget* parent = nullptr);
  ~JointTrajectoryPlotDialog();

private Q_SLOTS:
  void onSliderMoved(int value);
  void onTabIndexChanged(int value);

private:
  std::unique_ptr<Ui::JointTrajectoryPlotDialog> ui_;
  tesseract_gui::PlotDataMapRef plot_data_map_;
  tesseract_gui::PlotSubplots* position_subplots_;
  tesseract_gui::PlotSubplots* velocity_subplots_;
  tesseract_gui::PlotSubplots* acceleration_subplots_;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_JOINT_TRAJECTORY_JOINT_TRAJECTORY_PLOT_DIALOG_H
