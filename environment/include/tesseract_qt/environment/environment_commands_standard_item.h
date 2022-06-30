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
#ifndef TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_COMMANDS_STANDARD_ITEM_H
#define TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_COMMANDS_STANDARD_ITEM_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_environment/command.h>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <QStandardItem>

namespace tesseract_gui
{
class EnvironmentCommandsStandardItem : public QStandardItem
{
public:
  EnvironmentCommandsStandardItem();
  explicit EnvironmentCommandsStandardItem(tesseract_environment::Commands commands);
  explicit EnvironmentCommandsStandardItem(const QString& text, tesseract_environment::Commands commands);
  explicit EnvironmentCommandsStandardItem(const QIcon& icon,
                                           const QString& text,
                                           tesseract_environment::Commands commands);
  int type() const override;

  void appendCommand(const tesseract_environment::Command::ConstPtr& command);
  const tesseract_environment::Commands& getCommands() const;

private:
  tesseract_environment::Commands commands_;

  void addCommand(const QString& text, const tesseract_environment::Command::ConstPtr& command);
  void ctor();
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_COMMANDS_STANDARD_ITEM_H
