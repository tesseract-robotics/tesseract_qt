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

#include <tesseract_qt/command_language/models/composite_instruction_model.h>
#include <tesseract_qt/command_language/models/composite_instruction_standard_item.h>
#include <tesseract_qt/common/events/command_language_events.h>
#include <tesseract_qt/common/models/namespace_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/component_info.h>

#include <tesseract_command_language/composite_instruction.h>

#include <QApplication>

namespace tesseract_gui
{
struct CompositeInstructionModel::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;
  std::unordered_map<std::string, std::pair<QStandardItem*, tesseract_planning::CompositeInstruction>>
      composite_instructions;
};

CompositeInstructionModel::CompositeInstructionModel(QObject* parent) : CompositeInstructionModel(nullptr, parent) {}

CompositeInstructionModel::CompositeInstructionModel(std::shared_ptr<const ComponentInfo> component_info,
                                                     QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  clear();

  data_->component_info = std::move(component_info);

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

CompositeInstructionModel::~CompositeInstructionModel() = default;

std::shared_ptr<const ComponentInfo> CompositeInstructionModel::getComponentInfo() const
{
  return data_->component_info;
}

bool CompositeInstructionModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  return QStandardItemModel::setData(index, value, role);
}

void CompositeInstructionModel::setCompositeInstruction(const std::string& ns,
                                                        const tesseract_planning::CompositeInstruction& ci)
{
  removeNamespace(ns);

  QStandardItem* ns_item = new NamespaceStandardItem(QString::fromStdString(ns));
  ns_item->appendRow(new CompositeInstructionStandardItem(ci));

  appendRow(ns_item);
  data_->composite_instructions[ns] = std::make_pair(ns_item, ci);
}

void CompositeInstructionModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  data_->composite_instructions.clear();
}

void CompositeInstructionModel::removeNamespace(const std::string& ns)
{
  auto it = data_->composite_instructions.find(ns);
  if (it != data_->composite_instructions.end())
  {
    QModelIndex idx = indexFromItem(it->second.first);
    removeRow(idx.row(), idx.parent());
  }
}

NamespaceStandardItem* findCompositeInstructionItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::COMMON_NAMESPACE))
    return dynamic_cast<NamespaceStandardItem*>(item);

  return findCompositeInstructionItem(item->parent());
}

tesseract_planning::CompositeInstruction
CompositeInstructionModel::getCompositeInstruction(const QModelIndex& row) const
{
  NamespaceStandardItem* ns_item = findCompositeInstructionItem(itemFromIndex(row));
  std::string key = ns_item->text().toStdString();

  auto it = data_->composite_instructions.find(key);
  if (it != data_->composite_instructions.end())
    return it->second.second;

  return tesseract_planning::CompositeInstruction();
}

bool CompositeInstructionModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::CompositeInstructionSet::kType)
  {
    assert(dynamic_cast<events::CompositeInstructionSet*>(event) != nullptr);
    auto* e = static_cast<events::CompositeInstructionSet*>(event);
    if (e->getComponentInfo() == data_->component_info)
      setCompositeInstruction(e->getNamespace(), e->getCompositeInstruction());
  }
  else if (event->type() == events::CompositeInstructionClear::kType)
  {
    assert(dynamic_cast<events::CompositeInstructionClear*>(event) != nullptr);
    auto* e = static_cast<events::CompositeInstructionClear*>(event);
    if (e->getComponentInfo() == data_->component_info)
      clear();
  }
  else if (event->type() == events::CompositeInstructionRemove::kType)
  {
    assert(dynamic_cast<events::CompositeInstructionRemove*>(event) != nullptr);
    auto* e = static_cast<events::CompositeInstructionRemove*>(event);
    if (e->getComponentInfo() == data_->component_info)
    {
      for (const auto& entry : data_->composite_instructions)
      {
        if (entry.second.second.getUUID() == e->getUUID())
        {
          removeNamespace(entry.first);
          break;
        }
      }
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

}  // namespace tesseract_gui
