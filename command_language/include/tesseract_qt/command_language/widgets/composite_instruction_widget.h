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
#ifndef TESSERACT_QT_COMMAND_LANGUAGE_COMPOSITE_INSTRUCTION_WIDGET_H
#define TESSERACT_QT_COMMAND_LANGUAGE_COMPOSITE_INSTRUCTION_WIDGET_H

#include <memory>
#include <QWidget>

class QItemSelectionModel;

namespace tesseract_gui
{
class ComponentInfo;
class CompositeInstructionModel;
class CompositeInstructionWidget : public QWidget
{
  Q_OBJECT

public:
  explicit CompositeInstructionWidget(QWidget* parent = nullptr);
  explicit CompositeInstructionWidget(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent = nullptr);
  ~CompositeInstructionWidget();

  void setComponentInfo(std::shared_ptr<const ComponentInfo> component_info);
  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

  void setModel(std::shared_ptr<CompositeInstructionModel> model);
  std::shared_ptr<CompositeInstructionModel> getModel();
  std::shared_ptr<const CompositeInstructionModel> getModel() const;

  QItemSelectionModel& getSelectionModel();
  const QItemSelectionModel& getSelectionModel() const;

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_COMMAND_LANGUAGE_COMPOSITE_INSTRUCTION_WIDGET_H
