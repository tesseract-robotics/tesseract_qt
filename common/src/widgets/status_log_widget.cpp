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
#include <tesseract_qt/common/events/log_events.h>
#include <tesseract_qt/common/models/status_log_model.h>
#include <tesseract_qt/common/models/standard_item_type.h>
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
  QVBoxLayout* layout;
  QTableView* table_view;
  QSortFilterProxyModel* proxy_model;
  QString regex_pattern;

  // Store the selected item
  QStandardItem* selected_item{ nullptr };
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
  if (event->type() == events::EventType::TOGGLE_LOG_INFO_ON)
  {
    auto* e = dynamic_cast<events::ToggleLogInfoOnEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    if (data_->regex_pattern.isEmpty())
    {
      data_->regex_pattern.append("Info");
    }
    else
    {
      data_->regex_pattern.append("|Info");
    }
    data_->proxy_model->setFilterRegularExpression(data_->regex_pattern);
    data_->proxy_model->setFilterKeyColumn(1);
  }
  else if (event->type() == events::EventType::TOGGLE_LOG_INFO_OFF)
  {
    auto* e = dynamic_cast<events::ToggleLogInfoOffEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    if (data_->regex_pattern.contains(QString("|Info")))
    {
      data_->regex_pattern.remove("|Info");
    }
    else if (data_->regex_pattern.contains(QString("Info")))
    {
      data_->regex_pattern.remove("Info");
    }
    data_->proxy_model->setFilterRegularExpression(data_->regex_pattern);
  }
  else if (event->type() == events::EventType::TOGGLE_LOG_WARN_ON)
  {
    auto* e = dynamic_cast<events::ToggleLogWarnOnEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    if (data_->regex_pattern.isEmpty())
    {
      data_->regex_pattern.append("Warn");
    }
    else
    {
      data_->regex_pattern.append("|Warn");
    }
    data_->proxy_model->setFilterRegularExpression(data_->regex_pattern);
    data_->proxy_model->setFilterKeyColumn(1);
  }
  else if (event->type() == events::EventType::TOGGLE_LOG_WARN_OFF)
  {
    auto* e = dynamic_cast<events::ToggleLogWarnOffEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    if (data_->regex_pattern.contains(QString("|Warn")))
    {
      data_->regex_pattern.remove("|Warn");
    }
    else if (data_->regex_pattern.contains(QString("Warn")))
    {
      data_->regex_pattern.remove("Warn");
    }
    data_->proxy_model->setFilterRegularExpression(data_->regex_pattern);
  }
  else if (event->type() == events::EventType::TOGGLE_LOG_ERROR_ON)
  {
    auto* e = dynamic_cast<events::ToggleLogErrorOnEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    if (data_->regex_pattern.isEmpty())
    {
      data_->regex_pattern.append("Error");
    }
    else
    {
      data_->regex_pattern.append("|Error");
    }
    data_->proxy_model->setFilterRegularExpression(data_->regex_pattern);
    data_->proxy_model->setFilterKeyColumn(1);
  }
  else if (event->type() == events::EventType::TOGGLE_LOG_ERROR_OFF)
  {
    auto* e = dynamic_cast<events::ToggleLogErrorOffEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    if (data_->regex_pattern.contains(QString("|Error")))
    {
      data_->regex_pattern.remove("|Error");
    }
    else if (data_->regex_pattern.contains(QString("Error")))
    {
      data_->regex_pattern.remove("Error");
    }
    data_->proxy_model->setFilterRegularExpression(data_->regex_pattern);
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
