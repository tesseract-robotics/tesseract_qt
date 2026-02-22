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

#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/status_log_events.h>

#include <utility>
#include <string>

namespace tesseract::gui::events
{
StatusLogInfo::StatusLogInfo(QString text) : QEvent(QEvent::Type(EventType::STATUS_LOG_INFO)), text_(std::move(text)) {}

StatusLogInfo::StatusLogInfo(const std::string& text) : StatusLogInfo(QString::fromStdString(text)) {}

StatusLogInfo::StatusLogInfo(const char* text) : StatusLogInfo(QString(text)) {}

QString StatusLogInfo::getString() const { return text_; }

StatusLogWarn::StatusLogWarn(QString text) : QEvent(QEvent::Type(EventType::STATUS_LOG_WARN)), text_(std::move(text)) {}

StatusLogWarn::StatusLogWarn(const std::string& text) : StatusLogWarn(QString::fromStdString(text)) {}

StatusLogWarn::StatusLogWarn(const char* text) : StatusLogWarn(QString(text)) {}

QString StatusLogWarn::getString() const { return text_; }

StatusLogError::StatusLogError(QString text) : QEvent(QEvent::Type(EventType::STATUS_LOG_ERROR)), text_(std::move(text))
{
}

StatusLogError::StatusLogError(const std::string& text) : StatusLogError(QString::fromStdString(text)) {}

StatusLogError::StatusLogError(const char* text) : StatusLogError(QString(text)) {}

QString StatusLogError::getString() const { return text_; }

StatusLogClear::StatusLogClear() : QEvent(QEvent::Type(EventType::STATUS_LOG_CLEAR)) {}

StatusLogInfoToggleOn::StatusLogInfoToggleOn() : QEvent(QEvent::Type(EventType::STATUS_LOG_INFO_TOGGLE_ON)) {}

StatusLogWarnToggleOn::StatusLogWarnToggleOn() : QEvent(QEvent::Type(EventType::STATUS_LOG_WARN_TOGGLE_ON)) {}

StatusLogErrorToggleOn::StatusLogErrorToggleOn() : QEvent(QEvent::Type(EventType::STATUS_LOG_ERROR_TOGGLE_ON)) {}

StatusLogInfoToggleOff::StatusLogInfoToggleOff() : QEvent(QEvent::Type(EventType::STATUS_LOG_INFO_TOGGLE_OFF)) {}

StatusLogWarnToggleOff::StatusLogWarnToggleOff() : QEvent(QEvent::Type(EventType::STATUS_LOG_WARN_TOGGLE_OFF)) {}

StatusLogErrorToggleOff::StatusLogErrorToggleOff() : QEvent(QEvent::Type(EventType::STATUS_LOG_ERROR_TOGGLE_OFF)) {}

}  // namespace tesseract::gui::events
