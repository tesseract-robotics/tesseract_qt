/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 * @author David Spielman <david.spielman7@gmail.com>
 *
 * @copyright Copyright (C) 2025 Levi Armstrong <levi.armstrong@gmail.com>
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

#include <qdatetime.h>
#include <tesseract_qt/common/models/status_log_standard_item.h>
#include <tesseract_qt/common/events/log_events.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
StatusLogStandardItem::StatusLogStandardItem(const events::StatusLogInfoEvent& message) { ctor(message); }

StatusLogStandardItem::StatusLogStandardItem(const events::StatusLogWarnEvent& message) { ctor(message); }

StatusLogStandardItem::StatusLogStandardItem(const events::StatusLogErrorEvent& message) { ctor(message); }

int StatusLogStandardItem::type() const { return static_cast<int>(type_); }

void StatusLogStandardItem::ctor(const events::StatusLogInfoEvent& message)
{
  type_ = StandardItemType::STATUS_LOG_INFO;
  const QDateTime time;
  time_stamp_ = new QStandardItem(time.currentDateTime().toString("dd MMMM yyyy hh:mm:ss.zzz"));
  severity_ = new QStandardItem(icons::getInfoMsgIcon(), "Info");
  text_ = new QStandardItem(message.getString());
  text_->setForeground(Qt::black);
}

void StatusLogStandardItem::ctor(const events::StatusLogWarnEvent& message)
{
  type_ = StandardItemType::STATUS_LOG_WARN;
  const QDateTime time;
  time_stamp_ = new QStandardItem(time.currentDateTime().toString("dd MMMM yyyy hh:mm:ss.zzz"));
  severity_ = new QStandardItem(icons::getWarnMsgIcon(), "Warning");
  text_ = new QStandardItem(message.getString());
  const QColor orange("orange");
  text_->setForeground(orange);
}

void StatusLogStandardItem::ctor(const events::StatusLogErrorEvent& message)
{
  type_ = StandardItemType::STATUS_LOG_ERROR;
  const QDateTime time;
  time_stamp_ = new QStandardItem(time.currentDateTime().toString("dd MMMM yyyy hh:mm:ss.zzz"));
  severity_ = new QStandardItem(icons::getErrorMsgIcon(), "Error");
  text_ = new QStandardItem(message.getString());
  text_->setForeground(Qt::red);
}

QStandardItem* StatusLogStandardItem::getTimeStamp() const { return time_stamp_; }

QStandardItem* StatusLogStandardItem::getSeverity() const { return severity_; }

QStandardItem* StatusLogStandardItem::getText() const { return text_; }

}  // namespace tesseract_gui
