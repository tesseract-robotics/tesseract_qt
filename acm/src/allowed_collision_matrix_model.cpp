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

#include <tesseract_qt/acm/allowed_collision_matrix_model.h>
#include <tesseract_qt/acm/allowed_collision_matrix_events.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_common/allowed_collision_matrix.h>
#include <tesseract_qt/common/icon_utils.h>

#include <QApplication>

namespace tesseract_gui
{
struct AllowedCollisionMatrixModel::Implementation
{
  std::string scene_name;
  std::unordered_map<std::string, QStandardItem*> items;
};

AllowedCollisionMatrixModel::AllowedCollisionMatrixModel(std::string scene_name, QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  clear();
  data_->scene_name = std::move(scene_name);

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

AllowedCollisionMatrixModel::AllowedCollisionMatrixModel(const AllowedCollisionMatrixModel& other)
  : QStandardItemModel(other.d_ptr->parent), data_(std::make_unique<Implementation>())
{
  data_->scene_name = other.getSceneName();

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

AllowedCollisionMatrixModel::~AllowedCollisionMatrixModel() = default;

AllowedCollisionMatrixModel& AllowedCollisionMatrixModel::operator=(const AllowedCollisionMatrixModel& other)
{
  data_->scene_name = other.getSceneName();
  return *this;
}

const std::string& AllowedCollisionMatrixModel::getSceneName() const { return data_->scene_name; }

void AllowedCollisionMatrixModel::set(const tesseract_common::AllowedCollisionMatrix& acm)
{
  clear();

  for (const auto& ac : acm.getAllAllowedCollisions())
    add(ac.first.first, ac.first.second, ac.second);

  sort(0);
}

void AllowedCollisionMatrixModel::add(const std::string& link1_name,
                                      const std::string& link2_name,
                                      const std::string& reason)
{
  auto it1 = data_->items.find(link1_name);
  if (it1 == data_->items.end())
  {
    auto* item = new QStandardItem(QString::fromStdString(link1_name));
    item->setCheckable(true);
    data_->items[link1_name] = item;

    auto item_value = createStandardItemString(link2_name, reason);
    item_value.first()->setCheckable(true);

    item->appendRow(item_value);
    appendRow(item);
  }
  else
  {
    auto item_value = createStandardItemString(link2_name, reason);
    item_value.first()->setCheckable(true);

    it1->second->appendRow(item_value);
  }

  auto it2 = data_->items.find(link2_name);
  if (it2 == data_->items.end())
  {
    auto* item = new QStandardItem(QString::fromStdString(link2_name));
    item->setCheckable(true);
    data_->items[link2_name] = item;

    auto item_value = createStandardItemString(link1_name, reason);
    item_value.first()->setCheckable(true);

    item->appendRow(item_value);
    appendRow(item);
  }
  else
  {
    auto item_value = createStandardItemString(link1_name, reason);
    item_value.first()->setCheckable(true);

    it2->second->appendRow(item_value);
  }
}

void AllowedCollisionMatrixModel::remove(const std::string& link1_name, const std::string& link2_name)
{
  auto it1 = data_->items.find(link1_name);
  if (it1 != data_->items.end())
  {
    for (int row = 0; row < it1->second->rowCount(); ++row)
    {
      QStandardItem* child = it1->second->child(row);
      if (child->text().toStdString() == link2_name)
      {
        QModelIndex idx = indexFromItem(child);
        QStandardItemModel::removeRows(row, 1, idx.parent());
        break;
      }
    }
    if (it1->second->rowCount() == 0)
    {
      QModelIndex idx = indexFromItem(it1->second);
      QStandardItemModel::removeRows(idx.row(), 1, idx.parent());
      data_->items.erase(link1_name);
    }
  }

  auto it2 = data_->items.find(link2_name);
  if (it2 != data_->items.end())
  {
    for (int row = 0; row < it2->second->rowCount(); ++row)
    {
      QStandardItem* child = it2->second->child(row);
      if (child->text().toStdString() == link1_name)
      {
        QModelIndex idx = indexFromItem(child);
        QStandardItemModel::removeRows(row, 1, idx.parent());
        break;
      }
    }
    if (it2->second->rowCount() == 0)
    {
      QModelIndex idx = indexFromItem(it2->second);
      QStandardItemModel::removeRows(idx.row(), 1, idx.parent());
      data_->items.erase(link2_name);
    }
  }
}

void AllowedCollisionMatrixModel::remove(const std::string& link_name)
{
  auto it1 = data_->items.find(link_name);
  if (it1 != data_->items.end())
  {
    std::vector<std::array<std::string, 2>> link_pairs;
    for (int row = 0; row < it1->second->rowCount(); ++row)
      link_pairs.push_back({ link_name, it1->second->child(row)->text().toStdString() });

    for (const auto& link_pair : link_pairs)
      remove(link_pair[0], link_pair[1]);
  }
}

void AllowedCollisionMatrixModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  data_->items.clear();
}

tesseract_common::AllowedCollisionMatrix AllowedCollisionMatrixModel::getAllowedCollisionMatrix() const
{
  tesseract_common::AllowedCollisionMatrix acm;
  for (int parent_rows = 0; parent_rows < invisibleRootItem()->rowCount(); ++parent_rows)
  {
    QStandardItem* parent = invisibleRootItem()->child(parent_rows);
    if (parent->checkState() == Qt::Checked)
    {
      for (int child_row = 0; child_row < parent->rowCount(); ++child_row)
      {
        QStandardItem* child = parent->child(child_row);
        QStandardItem* reason = parent->child(child_row, 1);
        acm.addAllowedCollision(
            parent->text().toStdString(), child->text().toStdString(), reason->text().toStdString());
      }
    }
  }

  return acm;
}

bool AllowedCollisionMatrixModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // Need emit application event to change visible
  if (role == Qt::CheckStateRole)
  {
    std::vector<std::string> links;
    for (int parent_rows = 0; parent_rows < invisibleRootItem()->rowCount(); ++parent_rows)
    {
      QStandardItem* parent = invisibleRootItem()->child(parent_rows);
      if (parent->checkState() == Qt::Checked)
      {
        std::vector<std::string> sub_links;
        sub_links.push_back(parent->text().toStdString());
        for (int child_row = 0; child_row < parent->rowCount(); ++child_row)
        {
          QStandardItem* child = parent->child(child_row);
          if (child->checkState() == Qt::Checked)
            sub_links.push_back(child->text().toStdString());
        }
        if (sub_links.size() > 1)
          links.insert(links.end(), sub_links.begin(), sub_links.end());
      }
    }

    if (!links.empty())
      QApplication::sendEvent(qApp, new events::AllowedCollisionMatrixShow(data_->scene_name, links));
  }
  return QStandardItemModel::setData(index, value, role);
}

bool AllowedCollisionMatrixModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::AllowedCollisionMatrixSet::kType)
  {
    assert(dynamic_cast<events::AllowedCollisionMatrixSet*>(event) != nullptr);
    auto* e = static_cast<events::AllowedCollisionMatrixSet*>(event);
    if (e->getSceneName() == data_->scene_name)
      set(e->getACM());
  }
  else if (event->type() == events::AllowedCollisionMatrixAdd::kType)
  {
    assert(dynamic_cast<events::AllowedCollisionMatrixAdd*>(event) != nullptr);
    auto* e = static_cast<events::AllowedCollisionMatrixAdd*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      for (const auto& entry : e->getEntries())
        add(entry[0], entry[1], entry[2]);
    }
  }
  else if (event->type() == events::AllowedCollisionMatrixClear::kType)
  {
    assert(dynamic_cast<events::AllowedCollisionMatrixClear*>(event) != nullptr);
    auto* e = static_cast<events::AllowedCollisionMatrixClear*>(event);
    if (e->getSceneName() == data_->scene_name)
      clear();
  }
  else if (event->type() == events::AllowedCollisionMatrixRemove::kType)
  {
    assert(dynamic_cast<events::AllowedCollisionMatrixRemove*>(event) != nullptr);
    auto* e = static_cast<events::AllowedCollisionMatrixRemove*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      for (const auto& entry : e->getEntries())
        remove(entry[0], entry[1]);
    }
  }
  else if (event->type() == events::AllowedCollisionMatrixRemoveLink::kType)
  {
    assert(dynamic_cast<events::AllowedCollisionMatrixRemoveLink*>(event) != nullptr);
    auto* e = static_cast<events::AllowedCollisionMatrixRemoveLink*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      for (const auto& link_name : e->getLinkNames())
        remove(link_name);
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
