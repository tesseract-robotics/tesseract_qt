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

#include <tesseract_qt/kinematic_groups/joint_names_combo_box.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_environment/environment.h>

#include <QStringListModel>

namespace tesseract_gui
{
struct JointNamesComboBox::Implementation
{
  ComponentInfo component_info;
  QStringListModel model;
};

JointNamesComboBox::JointNamesComboBox(QWidget* parent) : JointNamesComboBox(ComponentInfo(), parent) {}
JointNamesComboBox::JointNamesComboBox(ComponentInfo component_info, QWidget* parent)
  : QComboBox(parent), data_(std::make_unique<Implementation>())
{
  data_->component_info = std::move(component_info);
  setModel(&data_->model);
}
JointNamesComboBox::~JointNamesComboBox() = default;

void JointNamesComboBox::setComponentInfo(ComponentInfo component_info)
{
  data_->component_info = std::move(component_info);
  updateModel();
}
const ComponentInfo& JointNamesComboBox::getComponentInfo() const { return data_->component_info; }

void JointNamesComboBox::updateModel()
{
  QStringList list;
  static_cast<QStringListModel*>(model())->setStringList(list);

  auto env_wrapper = tesseract_gui::EnvironmentManager::instance()->get(tesseract_gui::ComponentInfo());
  if (env_wrapper == nullptr)
    return;

  auto env = env_wrapper->getEnvironment();
  if (env == nullptr || !env->isInitialized())
    return;

  for (const auto& link_name : env->getActiveJointNames())
    list.append(QString::fromStdString(link_name));

  static_cast<QStringListModel*>(model())->setStringList(list);
}

void JointNamesComboBox::showPopup()
{
  updateModel();
  QComboBox::showPopup();
}

}  // namespace tesseract_gui
