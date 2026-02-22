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
#ifndef TESSERACT_QT_COMMON_INSTRUCTION_POLY_STANDARD_ITEM_FACTORY_H
#define TESSERACT_QT_COMMON_INSTRUCTION_POLY_STANDARD_ITEM_FACTORY_H

#include <tesseract_command_language/fwd.h>

#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <typeindex>

#include <QStandardItem>

namespace tesseract::gui
{
using InstructionPolyStandardItemFactoryFn =
    std::function<QList<QStandardItem*>(const tesseract::command_language::InstructionPoly& instruction_poly)>;

class InstructionPolyStandardItemFactory
{
public:
  virtual ~InstructionPolyStandardItemFactory() = default;
  virtual std::map<std::type_index, InstructionPolyStandardItemFactoryFn> create() const = 0;
};

class InstructionPolyStandardItemManager
{
public:
  InstructionPolyStandardItemManager() = default;
  ~InstructionPolyStandardItemManager() = default;
  InstructionPolyStandardItemManager(const InstructionPolyStandardItemManager&) = delete;
  InstructionPolyStandardItemManager& operator=(const InstructionPolyStandardItemManager&) = delete;
  InstructionPolyStandardItemManager(InstructionPolyStandardItemManager&&) = delete;
  InstructionPolyStandardItemManager& operator=(InstructionPolyStandardItemManager&&) = delete;

  static QList<QStandardItem*> create(const tesseract::command_language::InstructionPoly& instruction_poly);

  static void registerFactory(const InstructionPolyStandardItemFactory& factory);

  template <typename InstructionType>
  static void registerFactory(const InstructionPolyStandardItemFactoryFn& factory)
  {
    auto manager = instance();
    manager->registerFactoryFnHelper<InstructionType>(factory);
  }

private:
  std::shared_mutex mutex_;
  std::map<std::type_index, InstructionPolyStandardItemFactoryFn> factories_;

  static std::shared_ptr<InstructionPolyStandardItemManager> singleton;
  static std::once_flag init_instance_flag;
  static void initSingleton();
  static std::shared_ptr<InstructionPolyStandardItemManager> instance();

  QList<QStandardItem*> createHelper(const command_language::InstructionPoly& instruction_poly);

  void registerFactoryHelper(const InstructionPolyStandardItemFactory& factory);

  template <typename InstructionType>
  void registerFactoryFnHelper(const InstructionPolyStandardItemFactoryFn& factory)
  {
    std::unique_lock lock(mutex_);
    factories_[std::type_index(typeid(InstructionType))] = factory;
  }
};
}  // namespace tesseract::gui

#endif  // TESSERACT_QT_COMMON_INSTRUCTION_POLY_STANDARD_ITEM_FACTORY_H
