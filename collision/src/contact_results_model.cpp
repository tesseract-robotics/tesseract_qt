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

#include <tesseract_qt/collision/contact_results_model.h>
#include <tesseract_qt/collision/contact_result_standard_item.h>
#include <tesseract_qt/common/namespace_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>

namespace tesseract_gui
{
struct ContactResultsModelImpl
{
  std::unordered_map<std::string, std::pair<QStandardItem*, tesseract_collision::ContactResultVector>>
      contact_result_vectors;
  std::unordered_map<std::string, std::pair<QStandardItem*, tesseract_collision::ContactResultMap>> contact_result_maps;
};

ContactResultsModel::ContactResultsModel(QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<ContactResultsModelImpl>())
{
  clear();
}

ContactResultsModel::~ContactResultsModel() = default;

void ContactResultsModel::setContactResults(const QString& ns,
                                            const tesseract_collision::ContactResultVector& contact_results)
{
  removeNamespace(ns);

  QStandardItem* ns_item = new NamespaceStandardItem(ns);
  for (std::size_t i = 0; i < contact_results.size(); ++i)
    ns_item->appendRow(new ContactResultStandardItem(QString("[%1]").arg(i), contact_results[i]));

  appendRow(ns_item);
  data_->contact_result_vectors[ns.toStdString()] = std::make_pair(ns_item, contact_results);
}

void ContactResultsModel::setContactResults(const QString& ns,
                                            const tesseract_collision::ContactResultMap& contact_results)
{
  removeNamespace(ns);

  QStandardItem* ns_item = new NamespaceStandardItem(ns);
  for (const auto& pair : contact_results)
  {
    QStandardItem* link_pair_item = new NamespaceStandardItem(
        QString("%1::%2").arg(QString::fromStdString(pair.first.first), QString::fromStdString(pair.first.second)));
    for (std::size_t i = 0; i < pair.second.size(); ++i)
    {
      auto* cr_item = new ContactResultStandardItem(QString("[%1]").arg(i), pair.second[i]);
      auto* cr_value_item = new QStandardItem(QString("{ distance: %1 }").arg(pair.second[i].distance));
      link_pair_item->appendRow({ cr_item, cr_value_item });
    }

    ns_item->appendRow(link_pair_item);
  }
  appendRow(ns_item);
  data_->contact_result_maps[ns.toStdString()] = std::make_pair(ns_item, contact_results);
}

void ContactResultsModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  data_->contact_result_vectors.clear();
  data_->contact_result_maps.clear();
}

void ContactResultsModel::removeNamespace(const QString& ns)
{
  auto it_vector = data_->contact_result_vectors.find(ns.toStdString());
  auto it_map = data_->contact_result_maps.find(ns.toStdString());
  if (it_vector != data_->contact_result_vectors.end())
  {
    QModelIndex idx = indexFromItem(it_vector->second.first);
    removeRow(idx.row());
    data_->contact_result_vectors.erase(ns.toStdString());
  }
  else if (it_map != data_->contact_result_maps.end())
  {
    QModelIndex idx = indexFromItem(it_map->second.first);
    removeRow(idx.row());
    data_->contact_result_maps.erase(ns.toStdString());
  }
}

NamespaceStandardItem* findContactResultsItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::COMMON_NAMESPACE))
    return dynamic_cast<NamespaceStandardItem*>(item);

  return findContactResultsItem(item->parent());
}

tesseract_collision::ContactResultVector ContactResultsModel::getContactResults(const QModelIndex& row) const
{
  NamespaceStandardItem* ns_item = findContactResultsItem(itemFromIndex(row));
  std::string key = ns_item->text().toStdString();
  {  // Check vector results
    auto it = data_->contact_result_vectors.find(key);
    if (it != data_->contact_result_vectors.end())
      return it->second.second;
  }

  {  // Check map results
    auto it = data_->contact_result_maps.find(key);
    if (it != data_->contact_result_maps.end())
    {
      tesseract_collision::ContactResultVector results;
      tesseract_collision::flattenCopyResults(it->second.second, results);
      return results;
    }
  }

  if (ns_item->parent() != nullptr && ns_item->parent()->type() == static_cast<int>(StandardItemType::COMMON_NAMESPACE))
  {
    std::string ns = ns_item->parent()->text().toStdString();

    auto it = data_->contact_result_maps.find(ns);
    if (it != data_->contact_result_maps.end())
    {
      QStringList links = ns_item->text().split("::");
      if (links.size() == 2 && !links[0].isEmpty() && !links[1].isEmpty())
      {
        std::pair<std::string, std::string> sub_key(links[0].toStdString(), links[1].toStdString());
        auto sub_it = it->second.second.find(sub_key);
        if (sub_it != it->second.second.end())
          return sub_it->second;
      }
    }
  }

  return tesseract_collision::ContactResultVector();
}

}  // namespace tesseract_gui
