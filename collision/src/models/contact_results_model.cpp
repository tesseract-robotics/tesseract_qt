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

#include <tesseract_qt/collision/models/contact_results_model.h>
#include <tesseract_qt/collision/models/contact_result_standard_item.h>
#include <tesseract_qt/collision/models/contact_result_vector_standard_item.h>
#include <tesseract_qt/common/events/contact_results_events.h>
#include <tesseract_qt/common/models/namespace_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/component_info.h>

#include <QApplication>

namespace tesseract::gui
{
struct ContactResultsModel::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;
  std::unordered_map<std::string, QStandardItem*> namespace_map;
};

ContactResultsModel::ContactResultsModel(QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  clear();

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

ContactResultsModel::ContactResultsModel(std::shared_ptr<const ComponentInfo> component_info, QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  clear();

  data_->component_info = std::move(component_info);

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

ContactResultsModel::~ContactResultsModel() = default;

std::shared_ptr<const ComponentInfo> ContactResultsModel::getComponentInfo() const { return data_->component_info; }

bool ContactResultsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // Need emit application event to change visible
  if (role == Qt::CheckStateRole)
  {
    QStandardItem* item = itemFromIndex(index);
    if (item->type() == static_cast<int>(StandardItemType::COLLISION_CONTACT_RESULT))
    {
      assert(dynamic_cast<ContactResultStandardItem*>(item) != nullptr);
      assert(dynamic_cast<ContactResultVectorStandardItem*>(item->parent()) != nullptr);
      auto* derived_item = static_cast<ContactResultStandardItem*>(item);
      auto* parent_item = static_cast<ContactResultVectorStandardItem*>(item->parent());

      events::ContactResultsVisbility event(data_->component_info,
                                            parent_item->getUUID(),
                                            derived_item->contact_result.getUUID(),
                                            value.value<Qt::CheckState>() == Qt::Checked);
      QApplication::sendEvent(qApp, &event);
    }
    else if (item->type() == static_cast<int>(StandardItemType::COLLISION_CONTACT_RESULT_VECTOR))
    {
      assert(dynamic_cast<ContactResultVectorStandardItem*>(item) != nullptr);
      auto* derived_item = static_cast<ContactResultVectorStandardItem*>(item);

      events::ContactResultsVisbility event(
          data_->component_info, derived_item->getUUID(), value.value<Qt::CheckState>() == Qt::Checked);
      QApplication::sendEvent(qApp, &event);
    }
  }
  return QStandardItemModel::setData(index, value, role);
}

void ContactResultsModel::setContactResults(const QString& ns, const ContactResultVector& contact_results)
{
  removeNamespace(ns);

  auto* crv_item = new ContactResultVectorStandardItem(ns, contact_results);
  appendRow(crv_item);

  data_->namespace_map[ns.toStdString()] = crv_item;
}

void ContactResultsModel::setContactResults(const QString& ns, const ContactResultMap& contact_results)
{
  removeNamespace(ns);

  QStandardItem* ns_item = new NamespaceStandardItem(ns);
  for (const auto& pair : contact_results)
  {
    auto text =
        QString("%1::%2").arg(QString::fromStdString(pair.first.first), QString::fromStdString(pair.first.second));
    auto* link_pair_item = new ContactResultVectorStandardItem(text, pair.second);
    ns_item->appendRow(link_pair_item);
  }
  appendRow(ns_item);
  data_->namespace_map[ns.toStdString()] = ns_item;
}

void ContactResultsModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  data_->namespace_map.clear();
}

void ContactResultsModel::removeNamespace(const QString& ns)
{
  auto it_vector = data_->namespace_map.find(ns.toStdString());
  if (it_vector != data_->namespace_map.end())
  {
    QModelIndex idx = indexFromItem(it_vector->second);
    removeRow(idx.row(), idx.parent());
    data_->namespace_map.erase(ns.toStdString());
  }
}

ContactResultVectorStandardItem* findContactResultsItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::COLLISION_CONTACT_RESULT_VECTOR))
    return dynamic_cast<ContactResultVectorStandardItem*>(item);

  return findContactResultsItem(item->parent());
}

bool ContactResultsModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::EventType::CONTACT_RESULTS_SET)
  {
    assert(dynamic_cast<events::ContactResultsSet*>(event) != nullptr);
    auto* e = static_cast<events::ContactResultsSet*>(event);
    if (e->getComponentInfo() == data_->component_info)
    {
      if (e->getContactResults().index() == 0)
        setContactResults(e->getNamespace().c_str(), std::get<ContactResultVector>(e->getContactResults()));
      else
        setContactResults(e->getNamespace().c_str(), std::get<ContactResultMap>(e->getContactResults()));
    }
  }
  else if (event->type() == events::EventType::CONTACT_RESULTS_CLEAR)
  {
    assert(dynamic_cast<events::ContactResultsClear*>(event) != nullptr);
    auto* e = static_cast<events::ContactResultsClear*>(event);
    if (e->getComponentInfo() == data_->component_info)
      clear();
  }
  else if (event->type() == events::EventType::CONTACT_RESULTS_REMOVE)
  {
    assert(dynamic_cast<events::ContactResultsRemove*>(event) != nullptr);
    auto* e = static_cast<events::ContactResultsRemove*>(event);
    if (e->getComponentInfo() == data_->component_info)
    {
      throw std::runtime_error("ContactResultsModel, events::ContactResultsRemove not implemented yet!");
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

}  // namespace tesseract::gui
