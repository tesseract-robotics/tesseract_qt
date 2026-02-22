#ifndef TESSERACT_QT_COMMON_ANY_POLY_STANDARD_ITEM_FACTORY_H
#define TESSERACT_QT_COMMON_ANY_POLY_STANDARD_ITEM_FACTORY_H

#include <tesseract_common/fwd.h>

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
using AnyPolyStandardItemFactoryFn = std::function<QList<QStandardItem*>(const tesseract::common::AnyPoly& any_poly)>;

class AnyPolyStandardItemFactory
{
public:
  virtual ~AnyPolyStandardItemFactory() = default;
  virtual std::map<std::type_index, AnyPolyStandardItemFactoryFn> create() const = 0;
};

class AnyPolyStandardItemManager
{
public:
  AnyPolyStandardItemManager();
  ~AnyPolyStandardItemManager() = default;
  AnyPolyStandardItemManager(const AnyPolyStandardItemManager&) = delete;
  AnyPolyStandardItemManager& operator=(const AnyPolyStandardItemManager&) = delete;
  AnyPolyStandardItemManager(AnyPolyStandardItemManager&&) = delete;
  AnyPolyStandardItemManager& operator=(AnyPolyStandardItemManager&&) = delete;

  static QList<QStandardItem*> create(const tesseract::common::AnyPoly& any_poly);

  static void registerFactory(const AnyPolyStandardItemFactory& factory);

  template <typename InstructionType>
  static void registerFactory(const AnyPolyStandardItemFactoryFn& factory)
  {
    auto manager = instance();
    manager->registerFactoryFnHelper<InstructionType>(factory);
  }

private:
  std::shared_mutex mutex_;
  std::map<std::type_index, AnyPolyStandardItemFactoryFn> factories_;

  static std::shared_ptr<AnyPolyStandardItemManager> singleton;
  static std::once_flag init_instance_flag;
  static void initSingleton();
  static std::shared_ptr<AnyPolyStandardItemManager> instance();

  QList<QStandardItem*> createHelper(const tesseract::common::AnyPoly& any_poly);

  void registerFactoryHelper(const AnyPolyStandardItemFactory& factory);

  template <typename InstructionType>
  void registerFactoryFnHelper(const AnyPolyStandardItemFactoryFn& factory)
  {
    std::unique_lock lock(mutex_);
    factories_[std::type_index(typeid(InstructionType))] = factory;
  }
};
}  // namespace tesseract::gui

#endif  // TESSERACT_QT_COMMON_ANY_POLY_STANDARD_ITEM_FACTORY_H
