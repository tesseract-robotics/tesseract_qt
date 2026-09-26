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

#include <tesseract/common/logging.h>

#include <QApplication>
#include <QDateTime>
#include <QMetaObject>

#include <chrono>

namespace tesseract::gui
{
namespace
{
QList<QStandardItem*>
getItems(const QDateTime& timestamp, const QString& severity, const QString& message, const QColor& color)
{
  QList<QStandardItem*> items;
  items.append(new QStandardItem(timestamp.toString("dd MMMM yyyy hh:mm:ss.zzz")));
  if (severity == "Info")
  {
    items.append(new QStandardItem(icons::getInfoMsgIcon(), severity));
  }
  else if (severity == "Warn")
  {
    items.append(new QStandardItem(icons::getWarnMsgIcon(), severity));
  }
  else if (severity == "Error")
  {
    items.append(new QStandardItem(icons::getErrorMsgIcon(), severity));
  }
  items.append(new QStandardItem(message));
  items.back()->setForeground(color);
  return items;
}
}  // namespace

StatusLogModel::StatusLogModel(QObject* parent) : QStandardItemModel(parent)
{
  clear();

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);

  log_record_handler_id_ = tesseract::common::addLogRecordHandler([this](const tesseract::common::LogRecord& record) {
    QString severity;
    QColor color;
    switch (record.level)
    {
      case spdlog::level::trace:
      case spdlog::level::debug:
      case spdlog::level::info:
        severity = "Info";
        color = Qt::black;
        break;
      case spdlog::level::warn:
        severity = "Warn";
        color = QColor("orange");
        break;
      case spdlog::level::err:
      case spdlog::level::critical:
        severity = "Error";
        color = Qt::red;
        break;
      case spdlog::level::off:
        return;
    }

    const auto timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(record.timestamp.time_since_epoch()).count();
    const QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(timestamp_ms);
    const QString message = QString::fromStdString(record.message);
    QMetaObject::invokeMethod(
        this,
        [this, timestamp, severity = std::move(severity), message, color]() {
          appendRow(getItems(timestamp, severity, message, color));
        },
        Qt::AutoConnection);
  });
}

StatusLogModel::~StatusLogModel() { tesseract::common::removeLogRecordHandler(log_record_handler_id_); }

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
    auto* e = dynamic_cast<events::StatusLogInfo*>(event);
    if (e == nullptr)
      return true;  // Filter out

    appendRow(getItems(QDateTime::currentDateTime(), "Info", e->getString(), Qt::black));
  }
  else if (event->type() == events::EventType::STATUS_LOG_WARN)
  {
    auto* e = dynamic_cast<events::StatusLogWarn*>(event);
    if (e == nullptr)
      return true;  // Filter out

    appendRow(getItems(QDateTime::currentDateTime(), "Warn", e->getString(), QColor("orange")));
  }
  else if (event->type() == events::EventType::STATUS_LOG_ERROR)
  {
    auto* e = dynamic_cast<events::StatusLogError*>(event);
    if (e == nullptr)
      return true;  // Filter out

    appendRow(getItems(QDateTime::currentDateTime(), "Error", e->getString(), Qt::red));
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
}  // namespace tesseract::gui
