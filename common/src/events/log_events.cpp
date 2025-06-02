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
#include <tesseract_qt/common/events/log_events.h>

namespace tesseract_gui::events
{
LogInfoEvent::LogInfoEvent(QString text) : QEvent(QEvent::Type(EventType::LOG_INFO)), text_(std::move(text)) {}

LogInfoEvent::LogInfoEvent(const std::string& text) : LogInfoEvent(QString::fromStdString(text)) {}

LogInfoEvent::LogInfoEvent(const char* text) : LogInfoEvent(QString(text)) {}

QString LogInfoEvent::getString() const { return text_; }

LogWarnEvent::LogWarnEvent(QString text) : QEvent(QEvent::Type(EventType::LOG_WARN)), text_(std::move(text)) {}

LogWarnEvent::LogWarnEvent(const std::string& text) : LogWarnEvent(QString::fromStdString(text)) {}

LogWarnEvent::LogWarnEvent(const char* text) : LogWarnEvent(QString(text)) {}

QString LogWarnEvent::getString() const { return text_; }

LogErrorEvent::LogErrorEvent(QString text) : QEvent(QEvent::Type(EventType::LOG_ERROR)), text_(std::move(text)) {}

LogErrorEvent::LogErrorEvent(const std::string& text) : LogErrorEvent(QString::fromStdString(text)) {}

LogErrorEvent::LogErrorEvent(const char* text) : LogErrorEvent(QString(text)) {}

QString LogErrorEvent::getString() const { return text_; }

ClearStatusLogEvent::ClearStatusLogEvent() : QEvent(QEvent::Type(EventType::STATUS_CLEAR)) {}

ToggleLogInfoOnEvent::ToggleLogInfoOnEvent() : QEvent(QEvent::Type(EventType::TOGGLE_LOG_INFO_ON)) {}

ToggleLogWarnOnEvent::ToggleLogWarnOnEvent() : QEvent(QEvent::Type(EventType::TOGGLE_LOG_WARN_ON)) {}

ToggleLogErrorOnEvent::ToggleLogErrorOnEvent() : QEvent(QEvent::Type(EventType::TOGGLE_LOG_ERROR_ON)) {}

ToggleLogInfoOffEvent::ToggleLogInfoOffEvent() : QEvent(QEvent::Type(EventType::TOGGLE_LOG_INFO_OFF)) {}

ToggleLogWarnOffEvent::ToggleLogWarnOffEvent() : QEvent(QEvent::Type(EventType::TOGGLE_LOG_WARN_OFF)) {}

ToggleLogErrorOffEvent::ToggleLogErrorOffEvent() : QEvent(QEvent::Type(EventType::TOGGLE_LOG_ERROR_OFF)) {}

}  // namespace tesseract_gui::events
