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

#include <utility>
#include <string>

#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/status_log_events.h>

namespace tesseract_gui::events
{
StatusLogInfoEvent::StatusLogInfoEvent(QString text)
  : QEvent(QEvent::Type(EventType::STATUS_LOG_INFO)), text_(std::move(text))
{
}

StatusLogInfoEvent::StatusLogInfoEvent(const std::string& text) : StatusLogInfoEvent(QString::fromStdString(text)) {}

StatusLogInfoEvent::StatusLogInfoEvent(const char* text) : StatusLogInfoEvent(QString(text)) {}

QString StatusLogInfoEvent::getString() const { return text_; }

StatusLogWarnEvent::StatusLogWarnEvent(QString text)
  : QEvent(QEvent::Type(EventType::STATUS_LOG_WARN)), text_(std::move(text))
{
}

StatusLogWarnEvent::StatusLogWarnEvent(const std::string& text) : StatusLogWarnEvent(QString::fromStdString(text)) {}

StatusLogWarnEvent::StatusLogWarnEvent(const char* text) : StatusLogWarnEvent(QString(text)) {}

QString StatusLogWarnEvent::getString() const { return text_; }

StatusLogErrorEvent::StatusLogErrorEvent(QString text)
  : QEvent(QEvent::Type(EventType::STATUS_LOG_ERROR)), text_(std::move(text))
{
}

StatusLogErrorEvent::StatusLogErrorEvent(const std::string& text) : StatusLogErrorEvent(QString::fromStdString(text)) {}

StatusLogErrorEvent::StatusLogErrorEvent(const char* text) : StatusLogErrorEvent(QString(text)) {}

QString StatusLogErrorEvent::getString() const { return text_; }

StatusLogClearEvent::StatusLogClearEvent() : QEvent(QEvent::Type(EventType::STATUS_LOG_CLEAR)) {}

StatusLogInfoToggleOnEvent::StatusLogInfoToggleOnEvent() : QEvent(QEvent::Type(EventType::STATUS_LOG_INFO_TOGGLE_ON)) {}

StatusLogWarnToggleOnEvent::StatusLogWarnToggleOnEvent() : QEvent(QEvent::Type(EventType::STATUS_LOG_WARN_TOGGLE_ON)) {}

StatusLogErrorToggleOnEvent::StatusLogErrorToggleOnEvent() : QEvent(QEvent::Type(EventType::STATUS_LOG_ERROR_TOGGLE_ON))
{
}

StatusLogInfoToggleOffEvent::StatusLogInfoToggleOffEvent() : QEvent(QEvent::Type(EventType::STATUS_LOG_INFO_TOGGLE_OFF))
{
}

StatusLogWarnToggleOffEvent::StatusLogWarnToggleOffEvent() : QEvent(QEvent::Type(EventType::STATUS_LOG_WARN_TOGGLE_OFF))
{
}

StatusLogErrorToggleOffEvent::StatusLogErrorToggleOffEvent()
  : QEvent(QEvent::Type(EventType::STATUS_LOG_ERROR_TOGGLE_OFF))
{
}

}  // namespace tesseract_gui::events
