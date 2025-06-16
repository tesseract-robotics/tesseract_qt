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
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/models/status_log_model.h>

#include <QApplication>
#include <QDateTime>

namespace tesseract_gui
{
StatusLogModel::StatusLogModel(QObject* parent) : QStandardItemModel(parent)
{
  clear();

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

void StatusLogModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(3);
  setHorizontalHeaderLabels({ "Timestamp", "Severity", "Message" });
}

QList<QStandardItem*> getItems(const QString& severity, const QString& message, const QColor& color)
{
  QList<QStandardItem*> items;
  items.append(new QStandardItem(QDateTime::currentDateTime().toString("dd MMMM yyyy hh:mm:ss.zzz")));
  items.append(new QStandardItem(icons::getInfoMsgIcon(), severity));
  items.append(new QStandardItem(message));
  items.back()->setForeground(color);
  return items;
}

bool StatusLogModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == QEvent::Type(events::EventType::STATUS_LOG_INFO))
  {
    auto* e = dynamic_cast<events::StatusLogInfo*>(event);
    if (e == nullptr)
      return true;  // Filter out

    appendRow(getItems("Info", e->getString(), Qt::black));
  }
  else if (event->type() == events::EventType::STATUS_LOG_WARN)
  {
    auto* e = dynamic_cast<events::StatusLogWarn*>(event);
    if (e == nullptr)
      return true;  // Filter out

    appendRow(getItems("Warn", e->getString(), QColor("orange")));
  }
  else if (event->type() == events::EventType::STATUS_LOG_ERROR)
  {
    auto* e = dynamic_cast<events::StatusLogError*>(event);
    if (e == nullptr)
      return true;  // Filter out

    appendRow(getItems("Error", e->getString(), Qt::red));
  }
  else if (event->type() == events::EventType::STATUS_LOG_CLEAR)
  {
    auto* e = dynamic_cast<events::StatusLogClear*>(event);
    if (e == nullptr)
      return true;  // Filter out
    clear();
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
