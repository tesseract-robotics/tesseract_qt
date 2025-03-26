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

#include <tesseract_qt/joint_trajectory/widgets/joint_trajectory_tool_bar.h>
#include <tesseract_qt/common/events/joint_trajectory_events.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/component_info.h>
#include <QApplication>
#include <QMessageBox>

namespace tesseract_gui
{
struct JointTrajectoryToolBar::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;

  QAction* remove_all_action{ nullptr };
  QAction* remove_action{ nullptr };
  QAction* open_action{ nullptr };
  QAction* save_action{ nullptr };
  QAction* plot_action{ nullptr };

  QAction* component_info_action{ nullptr };
};

JointTrajectoryToolBar::JointTrajectoryToolBar(QWidget* parent) : JointTrajectoryToolBar(nullptr, parent) {}

JointTrajectoryToolBar::JointTrajectoryToolBar(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent)
  : QToolBar(parent), data_(std::make_unique<Implementation>())
{
  data_->component_info = std::move(component_info);
  data_->remove_all_action = addAction(icons::getClearIcon(), "Remove All", [this]() {
    events::JointTrajectoryRemoveAll event(data_->component_info);
    QApplication::sendEvent(qApp, &event);
  });
  data_->remove_action = addAction(icons::getTrashIcon(), "Remove All", [this]() {
    events::JointTrajectoryRemoveSelected event(data_->component_info);
    QApplication::sendEvent(qApp, &event);
  });
  addSeparator();
  data_->open_action = addAction(icons::getImportIcon(), "Open", [this]() {
    events::JointTrajectoryOpen event(data_->component_info);
    QApplication::sendEvent(qApp, &event);
  });
  data_->save_action = addAction(icons::getSaveIcon(), "Save", [this]() {
    events::JointTrajectorySave event(data_->component_info);
    QApplication::sendEvent(qApp, &event);
  });
  addSeparator();
  data_->plot_action = addAction(icons::getPlotIcon(), "Plot Joint Trajectory", [this]() {
    events::JointTrajectoryPlot event(data_->component_info);
    QApplication::sendEvent(qApp, &event);
  });
  addSeparator();
  data_->component_info_action = addAction(QIcon::fromTheme("dialog-information"), "Component Info", [this]() {
    if (data_->component_info != nullptr)
      QMessageBox::information(
          this, "Component Information", QString::fromStdString(data_->component_info->toString()));
    else
      QMessageBox::information(this, "Component Information", "Null");
  });

  data_->save_action->setDisabled(true);
  data_->plot_action->setDisabled(true);
  data_->remove_action->setDisabled(true);

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

JointTrajectoryToolBar::~JointTrajectoryToolBar() = default;

void JointTrajectoryToolBar::setComponentInfo(std::shared_ptr<const ComponentInfo> component_info)
{
  data_->component_info = std::move(component_info);
}
std::shared_ptr<const ComponentInfo> JointTrajectoryToolBar::getComponentInfo() const { return data_->component_info; }

bool JointTrajectoryToolBar::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::JointTrajectoryToolbarState::kType)
  {
    assert(dynamic_cast<events::JointTrajectoryToolbarState*>(event) != nullptr);
    auto* e = static_cast<events::JointTrajectoryToolbarState*>(event);
    if (e->getComponentInfo() == data_->component_info)
    {
      data_->remove_all_action->setEnabled(e->remove_all_enabled);
      data_->remove_action->setEnabled(e->remove_enabled);
      data_->open_action->setEnabled(e->open_enabled);
      data_->save_action->setEnabled(e->save_enabled);
      data_->plot_action->setEnabled(e->plot_enabled);
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
