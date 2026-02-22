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
#ifndef TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_COMMANDS_MODEL_H
#define TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_COMMANDS_MODEL_H

#ifndef Q_MOC_RUN
#include <memory>
#include <tesseract_environment/fwd.h>
#include <QStandardItemModel>
#endif

namespace tesseract::gui
{
class ComponentInfo;
class EnvironmentCommandsStandardItem;
class EnvironmentCommandsModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit EnvironmentCommandsModel(QObject* parent = nullptr);
  explicit EnvironmentCommandsModel(std::shared_ptr<const ComponentInfo> component_info, QObject* parent = nullptr);
  EnvironmentCommandsModel(const EnvironmentCommandsModel& other);
  EnvironmentCommandsModel& operator=(const EnvironmentCommandsModel& other);
  ~EnvironmentCommandsModel() override;

  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

  const std::vector<std::shared_ptr<const tesseract::environment::Command>>& getCommands() const;

private:
  std::shared_ptr<const ComponentInfo> component_info_;

  EnvironmentCommandsStandardItem* getRoot();
  const EnvironmentCommandsStandardItem* getRoot() const;

  void set(const std::vector<std::shared_ptr<const tesseract::environment::Command>>& commands);
  void appendCommand(const std::shared_ptr<const tesseract::environment::Command>& command);
  void clear();

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};

}  // namespace tesseract::gui

#endif  // TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_COMMANDS_MODEL_H
