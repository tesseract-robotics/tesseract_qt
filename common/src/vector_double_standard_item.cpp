/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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
#include <tesseract_qt/common/vector_double_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
VectorDoubleStandardItem::VectorDoubleStandardItem(const Eigen::Ref<const Eigen::VectorXd>& vector_double)
  : QStandardItem(icons::getSetIcon(), "VectorXd")
{
  ctor(vector_double);
}

VectorDoubleStandardItem::VectorDoubleStandardItem(const QString& text,
                                                   const Eigen::Ref<const Eigen::VectorXd>& vector_double)
  : QStandardItem(icons::getSetIcon(), text)
{
  ctor(vector_double);
}

VectorDoubleStandardItem::VectorDoubleStandardItem(const QIcon& icon,
                                                   const QString& text,
                                                   const Eigen::Ref<const Eigen::VectorXd>& vector_double)
  : QStandardItem(icon, text)
{
  ctor(vector_double);
}

VectorDoubleStandardItem::VectorDoubleStandardItem(const std::vector<double>& vector_double)
  : QStandardItem(icons::getSetIcon(), "VectorXd")
{
  ctor(Eigen::Map<const Eigen::VectorXd>(vector_double.data(), vector_double.size()));
}

VectorDoubleStandardItem::VectorDoubleStandardItem(const QString& text, const std::vector<double>& vector_double)
  : QStandardItem(icons::getSetIcon(), text)
{
  ctor(Eigen::Map<const Eigen::VectorXd>(vector_double.data(), vector_double.size()));
}

VectorDoubleStandardItem::VectorDoubleStandardItem(const QIcon& icon,
                                                   const QString& text,
                                                   const std::vector<double>& vector_double)
  : QStandardItem(icon, text)
{
  ctor(Eigen::Map<const Eigen::VectorXd>(vector_double.data(), vector_double.size()));
}

int VectorDoubleStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_VECTOR_DOUBLE); }

void VectorDoubleStandardItem::ctor(const Eigen::Ref<const Eigen::VectorXd>& vector_double)
{
  for (Eigen::Index i = 0; i < vector_double.size(); ++i)
    appendRow(createStandardItemFloat(QString("[%1]").arg(i).toStdString(), vector_double[i]));
}
}  // namespace tesseract_gui
