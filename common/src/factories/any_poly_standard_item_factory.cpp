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

#include <tesseract_qt/common/factories/any_poly_standard_item_factory.h>
#include <tesseract_qt/common/factories/instruction_poly_standard_item_factory.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/transform_standard_item.h>
#include <tesseract_qt/common/models/position_standard_item.h>
#include <tesseract_qt/common/models/manipulator_info_standard_item.h>
#include <tesseract_qt/common/models/tool_path_standard_item.h>
#include <tesseract_qt/common/tool_path.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_common/any_poly.h>
#include <tesseract_common/eigen_types.h>
#include <tesseract_common/manipulator_info.h>
#include <tesseract_command_language/poly/instruction_poly.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace tesseract::gui
{
AnyPolyStandardItemManager::AnyPolyStandardItemManager()
{
  {  // tesseract::command_language::InstructionPoly
    using Type = tesseract::command_language::InstructionPoly;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      const auto& data = any_poly.as<Type>();
      QList<QStandardItem*> item = InstructionPolyStandardItemManager::create(data);
      if (item.empty())
        return { new QStandardItem(icons::getUnknownIcon(), "Unknown Instruction") };

      return item;
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // bool
    using Type = bool;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      return createStandardItemBool("bool", any_poly.as<Type>());
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // int
    using Type = int;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      return createStandardItemInt("int", any_poly.as<Type>());
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // unsigned
    using Type = unsigned;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      return createStandardItemInt("unsigned", any_poly.as<Type>());  // NOLINT
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::size_t
    using Type = std::size_t;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      return createStandardItemInt("size_t", any_poly.as<Type>());  // NOLINT
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // float
    using Type = float;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      return createStandardItemFloat("float", any_poly.as<Type>());
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // double
    using Type = double;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      return createStandardItemFloat("double", any_poly.as<Type>());
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::string
    using Type = std::string;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      return createStandardItemString("string", any_poly.as<Type>());
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::unordered_map<std::string, bool>
    using Type = std::unordered_map<std::string, bool>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("unordered_map<std::string, bool>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemBool(pair.first, pair.second));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::unordered_map<std::string, int>
    using Type = std::unordered_map<std::string, int>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("unordered_map<std::string, int>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemInt(pair.first, pair.second));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::unordered_map<std::string, unsigned>
    using Type = std::unordered_map<std::string, unsigned>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("unordered_map<std::string, unsigned>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemInt(pair.first, pair.second));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::unordered_map<std::string, std::size_t>
    using Type = std::unordered_map<std::string, std::size_t>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("unordered_map<std::string, std::size_t>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemInt(pair.first, pair.second));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::unordered_map<std::string, float>
    using Type = std::unordered_map<std::string, float>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("unordered_map<std::string, float>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemFloat(pair.first, pair.second));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::unordered_map<std::string, double>
    using Type = std::unordered_map<std::string, double>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("unordered_map<std::string, double>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemFloat(pair.first, pair.second));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::unordered_map<std::string, std::string>
    using Type = std::unordered_map<std::string, std::string>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("unordered_map<std::string, std::string>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemString(pair.first, pair.second));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::map<std::string, bool>
    using Type = std::map<std::string, bool>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("map<std::string, bool>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemBool(pair.first, pair.second));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::map<std::string, int>
    using Type = std::map<std::string, int>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("map<std::string, int>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemInt(pair.first, pair.second));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::map<std::string, unsigned>
    using Type = std::map<std::string, unsigned>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("map<std::string, unsigned>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemInt(pair.first, pair.second));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::map<std::string, std::size_t>
    using Type = std::map<std::string, float>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("map<std::string, std::size_t>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemInt(pair.first, pair.second));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::map<std::string, float>
    using Type = std::map<std::string, float>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("map<std::string, float>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemFloat(pair.first, pair.second));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::map<std::string, double>
    using Type = std::map<std::string, double>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("map<std::string, double>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemFloat(pair.first, pair.second));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::map<std::string, std::string>
    using Type = std::map<std::string, std::string>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("map<std::string, std::string>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(createStandardItemString(pair.first, pair.second));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::vector<bool>
    using Type = std::vector<bool>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("vector<bool>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(createStandardItemBool(QString("[%1]").arg(i).toStdString(), data[i]));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::vector<int>
    using Type = std::vector<int>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("vector<int>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(createStandardItemInt(QString("[%1]").arg(i).toStdString(), data[i]));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::vector<unsigned>
    using Type = std::vector<unsigned>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("vector<unsigned>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(createStandardItemInt(QString("[%1]").arg(i).toStdString(), data[i]));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::vector<std::size_t>
    using Type = std::vector<std::size_t>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("vector<std::size_t>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(createStandardItemInt(QString("[%1]").arg(i).toStdString(), data[i]));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::vector<float>
    using Type = std::vector<float>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("vector<float>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(createStandardItemFloat(QString("[%1]").arg(i).toStdString(), data[i]));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::vector<double>
    using Type = std::vector<double>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("vector<double>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(createStandardItemFloat(QString("[%1]").arg(i).toStdString(), data[i]));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::vector<std::string>
    using Type = std::vector<std::string>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("vector<std::string>");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(createStandardItemString(QString("[%1]").arg(i).toStdString(), data[i]));

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // std::vector<Eigen::Vector3d>
    using Type = std::vector<Eigen::Vector3d>;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem(icons::getArrayIcon(), "positons");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(new PositionStandardItem(QString("[%1]").arg(i), data[i]));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // Eigen::Vector3d
    using Type = Eigen::Vector3d;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      const auto& data = any_poly.as<Type>();
      return { new PositionStandardItem(data) };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // Eigen::VectorXd
    using Type = Eigen::VectorXd;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("Eigen::VectorXd");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(createStandardItemFloat(QString("[%1]").arg(i).toStdString(), data[i]));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // Eigen::VectorXf
    using Type = Eigen::VectorXf;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("Eigen::VectorXf");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(createStandardItemFloat(QString("[%1]").arg(i).toStdString(), data[i]));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // Eigen::VectorXi
    using Type = Eigen::VectorXi;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("Eigen::VectorXi");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(createStandardItemInt(QString("[%1]").arg(i).toStdString(), data(i)));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // Eigen::Isometry3d
    using Type = Eigen::Isometry3d;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      const auto& data = any_poly.as<Type>();
      return { new TransformStandardItem(data) };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // tesseract::common::VectorVector3d
    using Type = tesseract::common::VectorVector3d;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem(icons::getArrayIcon(), "positons");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(new PositionStandardItem(QString("[%1]").arg(i), data[i]));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // tesseract::common::VectorIsometry3d
    using Type = tesseract::common::VectorIsometry3d;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem(icons::getArrayIcon(), "positons");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (std::size_t i = 0; i < data.size(); ++i)
        value->appendRow(new TransformStandardItem(QString("[%1]").arg(i), data[i]));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // tesseract::common::TransformMap
    using Type = tesseract::common::TransformMap;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      auto* value = new QStandardItem("TransformMap");  // NOLINT
      const auto& data = any_poly.as<Type>();
      for (const auto& pair : data)
        value->appendRow(new TransformStandardItem(QString::fromStdString(pair.first), pair.second));  // NOLINT

      return { value };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // tesseract::common::ManipulatorInfo
    using Type = tesseract::common::ManipulatorInfo;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      const auto& data = any_poly.as<Type>();
      return { new ManipulatorInfoStandardItem(data) };  // NOLINT
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // tesseract::common::Toolpath
    using Type = tesseract::common::Toolpath;
    auto fn = [](const tesseract::common::AnyPoly& any_poly) -> QList<QStandardItem*> {
      const auto& data = any_poly.as<Type>();
      return { new ToolPathStandardItem(data) };  // NOLINT
    };
    registerFactoryFnHelper<Type>(fn);
  }
}
std::shared_ptr<AnyPolyStandardItemManager> AnyPolyStandardItemManager::singleton = nullptr;  // NOLINT
std::once_flag AnyPolyStandardItemManager::init_instance_flag;                                // NOLINT
void AnyPolyStandardItemManager::initSingleton() { singleton = std::make_shared<AnyPolyStandardItemManager>(); }

std::shared_ptr<AnyPolyStandardItemManager> AnyPolyStandardItemManager::instance()
{
  std::call_once(init_instance_flag, &AnyPolyStandardItemManager::initSingleton);
  return singleton;
}

QList<QStandardItem*> AnyPolyStandardItemManager::create(const tesseract::common::AnyPoly& any_poly)
{
  return instance()->createHelper(any_poly);
}

QList<QStandardItem*> AnyPolyStandardItemManager::createHelper(const tesseract::common::AnyPoly& any_poly)
{
  std::shared_lock lock(mutex_);
  auto it = factories_.find(any_poly.getType());
  if (it == factories_.end())
    return {};

  return it->second(any_poly);
}

void AnyPolyStandardItemManager::registerFactoryHelper(const AnyPolyStandardItemFactory& factory)
{
  std::unique_lock lock(mutex_);
  auto fm = factory.create();
  factories_.insert(fm.begin(), fm.end());
}

}  // namespace tesseract::gui
