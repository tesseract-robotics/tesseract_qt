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
  if (event->type() == QEvent::Type(events::EventType::STATUS_LOG_INFO))
  {
    auto* e = dynamic_cast<events::StatusLogInfoEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    QList<QStandardItem*> items;
    items.append(new QStandardItem(QDateTime::currentDateTime().toString("dd MMMM yyyy hh:mm:ss.zzz")));
    items.append(new QStandardItem(icons::getInfoMsgIcon(), "Info"));
    items.append(new QStandardItem(e->getString()));
    items.back()->setForeground(Qt::black);
    appendRow(items);
  }
  else if (event->type() == events::EventType::STATUS_LOG_WARN)
  {
    auto* e = dynamic_cast<events::StatusLogWarnEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    QList<QStandardItem*> items;
    items.append(new QStandardItem(QDateTime::currentDateTime().toString("dd MMMM yyyy hh:mm:ss.zzz")));
    items.append(new QStandardItem(icons::getWarnMsgIcon(), "Warn"));
    items.append(new QStandardItem(e->getString()));
    items.back()->setForeground(QColor("orange"));
    appendRow(items);
  }
  else if (event->type() == events::EventType::STATUS_LOG_ERROR)
  {
    auto* e = dynamic_cast<events::StatusLogErrorEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    QList<QStandardItem*> items;
    items.append(new QStandardItem(QDateTime::currentDateTime().toString("dd MMMM yyyy hh:mm:ss.zzz")));
    items.append(new QStandardItem(icons::getErrorMsgIcon(), "Error"));
    items.append(new QStandardItem(e->getString()));
    items.back()->setForeground(Qt::red);
    appendRow(items);
  }
  else if (event->type() == events::EventType::STATUS_LOG_CLEAR)
  {
    auto* e = dynamic_cast<events::StatusLogClearEvent*>(event);
    if (e == nullptr)
      return true;  // Filter out
    clear();
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
