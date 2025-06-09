/**
 * @author David Spielman <david.spielman7@gmail.com>
 *
 * @copyright Copyright (C) 2025 David Spielman <david.spielman7@gmail.com>
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

#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/status_log_events.h>
#include <tesseract_qt/common/models/status_log_model.h>
#include <tesseract_qt/common/widgets/status_log_widget.h>

#include <QApplication>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

namespace tesseract_gui
{
struct StatusLogWidget::Implementation
{
  std::shared_ptr<StatusLogModel> model;
  QVBoxLayout* layout{ nullptr };
  QTableView* table_view{ nullptr };
  QSortFilterProxyModel* proxy_model{ nullptr };
  QString regex_pattern;

  // Store the selected item
  QStandardItem* selected_item{ nullptr };

  void addFilterEntry(const QString& entry)
  {
    if (regex_pattern.isEmpty())
      regex_pattern.append(entry);
    else
      regex_pattern.append(QString("|%1").arg(entry));

    proxy_model->setFilterRegularExpression(regex_pattern);
    proxy_model->setFilterKeyColumn(1);
  }

  void removeFilterEntry(const QString& entry)
  {
    if (regex_pattern.contains(QString("|%1").arg(entry)))
      regex_pattern.remove(QString("|%1").arg(entry));
    else if (regex_pattern.contains(QString("%1|").arg(entry)))
      regex_pattern.remove(QString("%1|").arg(entry));
    else if (regex_pattern.contains(entry))
      regex_pattern.remove(entry);

    proxy_model->setFilterRegularExpression(regex_pattern);
  }
};

StatusLogWidget::StatusLogWidget(QWidget* parent) : data_(std::make_unique<Implementation>())
{
  // Create model
  data_->model = std::make_shared<StatusLogModel>(parent);

  // Create table widget
  data_->table_view = new QTableView();
  data_->table_view->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
  data_->proxy_model = new QSortFilterProxyModel();
  data_->proxy_model->setSourceModel(data_->model.get());
  data_->table_view->setModel(data_->proxy_model);
  data_->table_view->setSortingEnabled(true);
  data_->table_view->setAlternatingRowColors(true);
  data_->table_view->horizontalHeader()->setStretchLastSection(true);
  data_->table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

  // Create layout
  data_->layout = new QVBoxLayout();
  data_->layout->setMargin(0);
  data_->layout->setSpacing(0);
  data_->layout->addWidget(data_->table_view);

  // Set layout
  setLayout(data_->layout);

  connect(data_->table_view->selectionModel(),
          SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
          this,
          SLOT(onCurrentRowChanged(QModelIndex, QModelIndex)));

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

StatusLogWidget::~StatusLogWidget() = default;

std::shared_ptr<StatusLogModel> StatusLogWidget::getModel() { return data_->model; }
std::shared_ptr<const StatusLogModel> StatusLogWidget::getModel() const { return data_->model; }

void StatusLogWidget::setModel(std::shared_ptr<StatusLogModel> model)
{
  data_->model = std::move(model);
  data_->table_view->setModel(data_->model.get());
}

// Documentation inherited
bool StatusLogWidget::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::EventType::STATUS_LOG_INFO_TOGGLE_ON)
  {
    auto* e = dynamic_cast<events::StatusLogInfoToggleOnEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out

    data_->addFilterEntry("Info");
  }
  else if (event->type() == events::EventType::STATUS_LOG_INFO_TOGGLE_OFF)
  {
    auto* e = dynamic_cast<events::StatusLogInfoToggleOffEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    data_->removeFilterEntry("Info");
  }
  else if (event->type() == events::EventType::STATUS_LOG_WARN_TOGGLE_ON)
  {
    auto* e = dynamic_cast<events::StatusLogWarnToggleOnEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out

    data_->addFilterEntry("Warn");
  }
  else if (event->type() == events::EventType::STATUS_LOG_WARN_TOGGLE_OFF)
  {
    auto* e = dynamic_cast<events::StatusLogWarnToggleOffEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    data_->removeFilterEntry("Warn");
  }
  else if (event->type() == events::EventType::STATUS_LOG_ERROR_TOGGLE_ON)
  {
    auto* e = dynamic_cast<events::StatusLogErrorToggleOnEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out

    data_->addFilterEntry("Error");
  }
  else if (event->type() == events::EventType::STATUS_LOG_ERROR_TOGGLE_OFF)
  {
    auto* e = dynamic_cast<events::StatusLogErrorToggleOffEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out

    data_->removeFilterEntry("Error");
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

void StatusLogWidget::onCurrentRowChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
  QModelIndex current_index = current;  // This appears to be changing so copy
  data_->selected_item = data_->model->itemFromIndex(current_index);
}
}  // namespace tesseract_gui
