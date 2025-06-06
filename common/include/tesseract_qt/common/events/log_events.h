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

#ifndef TESSERACT_QT_COMMON_LOG_EVENTS_H
#define TESSERACT_QT_COMMON_LOG_EVENTS_H

#include <string>
#include <qcoreevent.h>
#include <qstring.h>

namespace tesseract_gui::events
{
/** @brief Event called to log information */
class StatusLogInfoEvent : public QEvent
{
public:
  explicit StatusLogInfoEvent(QString text);
  explicit StatusLogInfoEvent(const std::string& text);
  explicit StatusLogInfoEvent(const char* text);
  [[nodiscard]] QString getString() const;

private:
  QString text_;
};

/** @brief Event called to log warnings */
class StatusLogWarnEvent : public QEvent
{
public:
  explicit StatusLogWarnEvent(QString text);
  explicit StatusLogWarnEvent(const std::string& text);
  explicit StatusLogWarnEvent(const char* text);
  [[nodiscard]] QString getString() const;

private:
  QString text_;
};

/** @brief Event called to log errors */
class StatusLogErrorEvent : public QEvent
{
public:
  explicit StatusLogErrorEvent(QString text);
  explicit StatusLogErrorEvent(const std::string& text);
  explicit StatusLogErrorEvent(const char* text);
  [[nodiscard]] QString getString() const;

private:
  QString text_;
};

/** @brief Event called to clear the messages in the status log */
class StatusLogClearEvent : public QEvent
{
public:
  StatusLogClearEvent();
};

/** @brief Event called to toggle viewing log information on*/
class StatusLogInfoToggleOnEvent : public QEvent
{
public:
  StatusLogInfoToggleOnEvent();
};

/** @brief Event called to toggle viewing log warnings on*/
class StatusLogWarnToggleOnEvent : public QEvent
{
public:
  StatusLogWarnToggleOnEvent();
};

/** @brief Event called to toggle viewing log errors on*/
class StatusLogErrorToggleOnEvent : public QEvent
{
public:
  StatusLogErrorToggleOnEvent();
};

/** @brief Event called to toggle viewing log information off*/
class StatusLogInfoToggleOffEvent : public QEvent
{
public:
  StatusLogInfoToggleOffEvent();
};

/** @brief Event called to toggle viewing log warnings off*/
class StatusLogWarnToggleOffEvent : public QEvent
{
public:
  StatusLogWarnToggleOffEvent();
};

/** @brief Event called to toggle viewing log errors off*/
class StatusLogErrorToggleOffEvent : public QEvent
{
public:
  StatusLogErrorToggleOffEvent();
};

}  // namespace tesseract_gui::events

#endif  // TESSERACT_QT_COMMON_LOG_EVENTS_H
