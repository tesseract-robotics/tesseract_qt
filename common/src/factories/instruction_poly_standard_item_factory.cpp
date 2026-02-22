/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2023 Levi Armstrong <levi.armstrong@gmail.com>
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

#include <tesseract_qt/common/factories/instruction_poly_standard_item_factory.h>
#include <tesseract_command_language/poly/instruction_poly.h>

namespace tesseract::gui
{
std::shared_ptr<InstructionPolyStandardItemManager> InstructionPolyStandardItemManager::singleton = nullptr;
std::once_flag InstructionPolyStandardItemManager::init_instance_flag;
void InstructionPolyStandardItemManager::initSingleton()
{
  singleton = std::make_shared<InstructionPolyStandardItemManager>();
}

std::shared_ptr<InstructionPolyStandardItemManager> InstructionPolyStandardItemManager::instance()
{
  std::call_once(init_instance_flag, &InstructionPolyStandardItemManager::initSingleton);
  return singleton;
}

QList<QStandardItem*>
InstructionPolyStandardItemManager::create(const command_language::InstructionPoly& instruction_poly)
{
  return instance()->createHelper(instruction_poly);
}

QList<QStandardItem*>
InstructionPolyStandardItemManager::createHelper(const tesseract::command_language::InstructionPoly& instruction_poly)
{
  std::shared_lock lock(mutex_);
  auto it = factories_.find(instruction_poly.getType());
  if (it == factories_.end())
    return {};

  return it->second(instruction_poly);
}

void InstructionPolyStandardItemManager::registerFactoryHelper(const InstructionPolyStandardItemFactory& factory)
{
  std::unique_lock lock(mutex_);
  auto fm = factory.create();
  factories_.insert(fm.begin(), fm.end());
}

}  // namespace tesseract::gui
