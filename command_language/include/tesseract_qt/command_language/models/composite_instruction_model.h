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
#ifndef TESSERACT_QT_COMMAND_LANGUAGE_COMPOSITE_INSTRUCTION_MODEL_H
#define TESSERACT_QT_COMMAND_LANGUAGE_COMPOSITE_INSTRUCTION_MODEL_H

#include <QStandardItemModel>
#include <memory>

namespace tesseract_planning
{
class CompositeInstruction;
}

namespace tesseract_gui
{
struct ComponentInfo;
class CompositeInstructionModel : public QStandardItemModel
{
  Q_OBJECT
public:
  CompositeInstructionModel(QObject* parent = nullptr);
  CompositeInstructionModel(std::shared_ptr<const ComponentInfo> component_info, QObject* parent = nullptr);
  ~CompositeInstructionModel() override;

  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

  tesseract_planning::CompositeInstruction getCompositeInstruction(const QModelIndex& row) const;

  void clear();

protected:
  struct Implementation;
  std::unique_ptr<Implementation> data_;

  void setCompositeInstruction(const std::string& ns, const tesseract_planning::CompositeInstruction& ci);
  void removeNamespace(const std::string& ns);

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_COMMAND_LANGUAGE_COMPOSITE_INSTRUCTION_MODEL_H
