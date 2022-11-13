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
#ifndef TESSERACT_QT_SRDF_EDITOR_WIDGET_H
#define TESSERACT_QT_SRDF_EDITOR_WIDGET_H

#include <QWidget>
#include <memory>

namespace tesseract_common
{
class ResourceLocator;
}

namespace tesseract_scene_graph
{
class Joint;
}

namespace tesseract_environment
{
class Environment;
}

namespace Ui
{
class SRDFEditorWidget;
}

namespace tesseract_gui
{
struct SRDFEditorWidgetImpl;
class SRDFEditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit SRDFEditorWidget(std::shared_ptr<tesseract_common::ResourceLocator> locator, QWidget* parent = nullptr);
  ~SRDFEditorWidget();

Q_SIGNALS:
  void showStatusMessage(const QString& message, int timeout);
  void environmentSet(const std::shared_ptr<tesseract_environment::Environment>& env);

public Q_SLOTS:
  /**
   * @brief Callback trigged when the load button is pressed.
   * @param[in] urdf_filepath The urdf file path.
   * @param[in] srdf_filepath The srdf file path.
   */
  void onLoad(const QString& urdf_filepath, const QString& srdf_filepath);
  void onSave(const QString& srdf_filepath);

  void onBrowseURDFLoadClicked();
  void onBrowseSRDFLoadClicked();
  void onBrowseSRDFSaveClicked();

  void onAddChainGroup(const QString& group_name, const QString& base_link, const QString& tip_link);
  void onAddJointGroup(const QString& group_name);
  void onAddJointGroupJoint(const QString& joint_name);
  void onRemoveJointGroupJoint(int index);

  void onAddLinkGroup(const QString& group_name);
  void onAddLinkGroupLink(const QString& link_name);
  void onRemoveLinkGroupLink(int index);

  void onRemoveKinematicGroup(int index);

  void onGenerateACM(long resolution);
  void onRemoveACMEntry(int index);
  void onClickedACMEntry(int index);

  void onLoadJointGroup(const QString& group_name);
  void onJointValue(const QString& joint_name, double joint_value);

  void onAddUserDefinedJointState(const QString& group_name, const QString& state_name);
  void onRemoveUserDefinedJointState(int index);

  void onAddUserDefinedTCP(const QString& group_name,
                           const QString& tcp_name,
                           const QVector3D& position,
                           const QVector3D& orientation);
  void onRemoveUserDefinedTCP(int index);

  void onAddGroupOPWKinematics(const QString& group_name,
                               double a1,
                               double a2,
                               double b,
                               double c1,
                               double c2,
                               double c3,
                               double c4,
                               double o1,
                               double o2,
                               double o3,
                               double o4,
                               double o5,
                               double o6,
                               int sc1,
                               int sc2,
                               int sc3,
                               int sc4,
                               int sc5,
                               int sc6);

  void onRemoveGroupOPWKinematics(int index);

protected Q_SLOTS:
  void onLoad();
  void onSave();

private:
  std::unique_ptr<Ui::SRDFEditorWidget> ui_;
  std::unique_ptr<SRDFEditorWidgetImpl> data_;

  void removeGroupStates(const QString& group_name);
  void removeGroupTCPs(const QString& group_name);
  void removeGroupOPWKinematics(const QString& group_name);

  void enablePages(bool enable);

  std::vector<std::shared_ptr<const tesseract_scene_graph::Joint>> groupJointsRetrevier(QString group_name);
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_SRDF_EDITOR_WIDGET_H
