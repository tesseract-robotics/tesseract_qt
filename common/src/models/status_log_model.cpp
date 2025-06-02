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
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/models/status_log_model.h>
#include <tesseract_qt/common/models/status_log_standard_item.h>

#include <QApplication>

namespace tesseract_gui
{
struct StatusLogModel::Implementation
{
};

StatusLogModel::StatusLogModel(QObject* parent) : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  clear();

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

StatusLogModel::~StatusLogModel() = default;

void StatusLogModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(3);
  setHorizontalHeaderLabels({ "Timestamp", "Severity", "Message" });
}

bool StatusLogModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == QEvent::Type(events::EventType::LOG_INFO))
  {
    auto* e = dynamic_cast<events::LogInfoEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    auto* log_standard_item = new StatusLogStandardItem(*e);
    QList<QStandardItem*> items;
    items.append(log_standard_item->getTimeStamp());
    items.append(log_standard_item->getSeverity());
    items.append(log_standard_item->getText());
    appendRow(items);
  }
  else if (event->type() == events::EventType::LOG_WARN)
  {
    auto* e = dynamic_cast<events::LogWarnEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    auto* log_standard_item = new StatusLogStandardItem(*e);
    QList<QStandardItem*> items;
    items.append(log_standard_item->getTimeStamp());
    items.append(log_standard_item->getSeverity());
    items.append(log_standard_item->getText());
    appendRow(items);
  }
  else if (event->type() == events::EventType::LOG_ERROR)
  {
    auto* e = dynamic_cast<events::LogErrorEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    auto* log_standard_item = new StatusLogStandardItem(*e);
    QList<QStandardItem*> items;
    items.append(log_standard_item->getTimeStamp());
    items.append(log_standard_item->getSeverity());
    items.append(log_standard_item->getText());
    appendRow(items);
  }
  else if (event->type() == events::EventType::STATUS_CLEAR)
  {
    auto* e = dynamic_cast<events::ClearStatusLogEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    clear();
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
