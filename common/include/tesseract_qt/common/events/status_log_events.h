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

#ifndef TESSERACT_QT_COMMON_STATUS_LOG_EVENTS_H
#define TESSERACT_QT_COMMON_STATUS_LOG_EVENTS_H

#include <string>
#include <qcoreevent.h>
#include <qstring.h>

namespace tesseract::gui::events
{
/** @brief Event called to log information */
class StatusLogInfo : public QEvent
{
public:
  explicit StatusLogInfo(QString text);
  explicit StatusLogInfo(const std::string& text);
  explicit StatusLogInfo(const char* text);
  [[nodiscard]] QString getString() const;

private:
  QString text_;
};

/** @brief Event called to log warnings */
class StatusLogWarn : public QEvent
{
public:
  explicit StatusLogWarn(QString text);
  explicit StatusLogWarn(const std::string& text);
  explicit StatusLogWarn(const char* text);
  [[nodiscard]] QString getString() const;

private:
  QString text_;
};

/** @brief Event called to log errors */
class StatusLogError : public QEvent
{
public:
  explicit StatusLogError(QString text);
  explicit StatusLogError(const std::string& text);
  explicit StatusLogError(const char* text);
  [[nodiscard]] QString getString() const;

private:
  QString text_;
};

/** @brief Event called to clear the messages in the status log */
class StatusLogClear : public QEvent
{
public:
  StatusLogClear();
};

/** @brief Event called to toggle viewing log information on*/
class StatusLogInfoToggleOn : public QEvent
{
public:
  StatusLogInfoToggleOn();
};

/** @brief Event called to toggle viewing log warnings on*/
class StatusLogWarnToggleOn : public QEvent
{
public:
  StatusLogWarnToggleOn();
};

/** @brief Event called to toggle viewing log errors on*/
class StatusLogErrorToggleOn : public QEvent
{
public:
  StatusLogErrorToggleOn();
};

/** @brief Event called to toggle viewing log information off*/
class StatusLogInfoToggleOff : public QEvent
{
public:
  StatusLogInfoToggleOff();
};

/** @brief Event called to toggle viewing log warnings off*/
class StatusLogWarnToggleOff : public QEvent
{
public:
  StatusLogWarnToggleOff();
};

/** @brief Event called to toggle viewing log errors off*/
class StatusLogErrorToggleOff : public QEvent
{
public:
  StatusLogErrorToggleOff();
};

}  // namespace tesseract::gui::events

#endif  // TESSERACT_QT_COMMON_STATUS_LOG_EVENTS_H
