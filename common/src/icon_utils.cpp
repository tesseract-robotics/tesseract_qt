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

#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/svg_util.h>
namespace tesseract_gui::icons
{
QIcon getAnchorIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/anchor.png");
  return icon;
}

QIcon getAxisIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/axis.png");
  return icon;
}

QIcon getCalibrationIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/calibration.png");
  return icon;
}

QIcon getCapsuleIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/capsule.png");
  return icon;
}

QIcon getColorIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/color.png");
  return icon;
}

QIcon getConeIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/cone.png");
  return icon;
}

QIcon getContinuousIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/continuous.png");
  return icon;
}

QIcon getCubeIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/cube.png");
  return icon;
}

QIcon getCylinderIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/cylinder.png");
  return icon;
}

QIcon getDynamicsIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/dynamics.png");
  return icon;
}

QIcon getTrajectoryIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/trajectory.png");
  return icon;
}

QIcon getLimitsIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/limits.png");
  return icon;
}

QIcon getCommandEntryIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/merge.png");
  return icon;
}

QIcon getConvexMeshIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/mesh.png");
  return icon;
}

QIcon getNumericIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/numeric.png");
  return icon;
}

QIcon getOctreeIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/octree.png");
  return icon;
}

QIcon getOrientationIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/orientation.png");
  return icon;
}

QIcon getOriginIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/origin.png");
  return icon;
}

QIcon getPlaneIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/plane.png");
  return icon;
}

QIcon getPluginIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/plugin.png");
  return icon;
}

QIcon getPositionIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/position.png");
  return icon;
}

QIcon getPrismaticIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/prismatic.png");
  return icon;
}

QIcon getSetIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/programming.png");
  return icon;
}

QIcon getRevoluteIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/revolute.png");
  return icon;
}

QIcon getRGBIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/rgb.png");
  return icon;
}

QIcon getRobotArmIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/robotic-arm.png");
  return icon;
}

QIcon getSafetyIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/safety.png");
  return icon;
}

QIcon getShapesIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/shapes.png");
  return icon;
}

QIcon getSphereIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/sphere.png");
  return icon;
}

QIcon getMeshIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/surface.png");
  return icon;
}

QIcon getTextIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/text.png");
  return icon;
}

QIcon getURLIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/url.png");
  return icon;
}

QIcon getLogIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/log.png");
  return icon;
}

QIcon getTrashIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/trash.svg"));
  return icon;
}

QIcon getPlotIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/plot_image.svg"));
  return icon;
}

QIcon getImportIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/import.svg"));
  return icon;
}

QIcon getSaveIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/save.svg"));
  return icon;
}

QIcon getModelIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/model.png");
  return icon;
}
QIcon getJointIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/joint.png");
  return icon;
}
QIcon getLinkIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/link.png");
  return icon;
}
QIcon getJointVectorIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/joint_vector.png");
  return icon;
}
QIcon getLinkVectorIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/link_vector.png");
  return icon;
}
QIcon getVisualIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/visual.png");
  return icon;
}
QIcon getCollisionIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/collision.png");
  return icon;
}
QIcon getVisualVectorIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/visual_vector.png");
  return icon;
}
QIcon getCollisionVectorIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/collision_vector.png");
  return icon;
}

QIcon getUnknownIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/unknown.png");
  return icon;
}
QIcon getJointFixedIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/joint_fixed.png");
  return icon;
}
QIcon getJointRevoluteIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/joint_revolute.png");
  return icon;
}
QIcon getJointContinuousIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/joint_continuous.png");
  return icon;
}
QIcon getJointPrismaticIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/joint_prismatic.png");
  return icon;
}
QIcon getJointUnknownIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/joint_unknown.png");
  return icon;
}

QIcon getShowAllLinksIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/show-link.png");
  return icon;
}

QIcon getHideAllLinksIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/hide-link.png");
  return icon;
}

QIcon getShowVisualAllLinksIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/show-visual.png");
  return icon;
}

QIcon getHideVisualAllLinksIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/hide-visual.png");
  return icon;
}

QIcon getShowCollisionAllLinksIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/show-collision.png");
  return icon;
}

QIcon getHideCollisionAllLinksIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/hide-collision.png");
  return icon;
}

QIcon getShowAxisAllLinksIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/show-origin.png");
  return icon;
}

QIcon getHideAxisAllLinksIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/hide-origin.png");
  return icon;
}

QIcon getSelectAllLinksIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/select-link.png");
  return icon;
}

QIcon getDeselectAllLinksIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/ignition/deselect-link.png");
  return icon;
}

QIcon getTesseractIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/tesseract_logo.png");
  return icon;
}

QIcon getRestoreIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/restore.png");
  return icon;
}

QIcon getLayoutIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/png/layout.png");
  return icon;
}

QIcon getDateTimeIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/datetime.svg"));
  return icon;
}

QIcon getGreenCircleIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/svg/green_circle.svg");
  return icon;
}

QIcon getRedCircleIcon()
{
  static QIcon icon = QIcon(":/tesseract_qt/svg/red_circle.svg");
  return icon;
}

QIcon getClearIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/clear.svg"));
  return icon;
}

QIcon getZoomInIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/zoom_in.svg"));
  return icon;
}

QIcon getZoomOutIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/zoom_out.svg"));
  return icon;
}

QIcon getRatioIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/ratio.svg"));
  return icon;
}

QIcon getFullscreenIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/fullscreen.svg"));
  return icon;
}

QIcon getDataIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/data.svg"));
  return icon;
}

QIcon getShowIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/show.svg"));
  return icon;
}

QIcon getHideIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/hide.svg"));
  return icon;
}

QIcon getToolPathIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/tool_path.svg"));
  return icon;
}

QIcon getToolPathShowIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/tool_path_show.svg"));
  return icon;
}

QIcon getToolPathHideIcon()
{
  static QIcon icon = QIcon(LoadSvg(":/tesseract_qt/svg/tool_path_hide.svg"));
  return icon;
}

}  // namespace tesseract_gui::icons
