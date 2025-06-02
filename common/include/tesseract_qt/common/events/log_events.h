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
class LogInfoEvent : public QEvent
{
public:
  explicit LogInfoEvent(QString text);
  explicit LogInfoEvent(const std::string& text);
  explicit LogInfoEvent(const char* text);
  [[nodiscard]] QString getString() const;

private:
  QString text_;
};

/** @brief Event called to log warnings */
class LogWarnEvent : public QEvent
{
public:
  explicit LogWarnEvent(QString text);
  explicit LogWarnEvent(const std::string& text);
  explicit LogWarnEvent(const char* text);
  [[nodiscard]] QString getString() const;

private:
  QString text_;
};

/** @brief Event called to log errors */
class LogErrorEvent : public QEvent
{
public:
  explicit LogErrorEvent(QString text);
  explicit LogErrorEvent(const std::string& text);
  explicit LogErrorEvent(const char* text);
  [[nodiscard]] QString getString() const;

private:
  QString text_;
};

/** @brief Event called to clear the messages in the status log */
class ClearStatusLogEvent : public QEvent
{
public:
  ClearStatusLogEvent();
};

/** @brief Event called to toggle viewing log information on*/
class ToggleLogInfoOnEvent : public QEvent
{
public:
  ToggleLogInfoOnEvent();
};

/** @brief Event called to toggle viewing log warnings on*/
class ToggleLogWarnOnEvent : public QEvent
{
public:
  ToggleLogWarnOnEvent();
};

/** @brief Event called to toggle viewing log errors on*/
class ToggleLogErrorOnEvent : public QEvent
{
public:
  ToggleLogErrorOnEvent();
};

/** @brief Event called to toggle viewing log information off*/
class ToggleLogInfoOffEvent : public QEvent
{
public:
  ToggleLogInfoOffEvent();
};

/** @brief Event called to toggle viewing log warnings off*/
class ToggleLogWarnOffEvent : public QEvent
{
public:
  ToggleLogWarnOffEvent();
};

/** @brief Event called to toggle viewing log errors off*/
class ToggleLogErrorOffEvent : public QEvent
{
public:
  ToggleLogErrorOffEvent();
};

}  // namespace tesseract_gui::events

#endif  // TESSERACT_QT_COMMON_LOG_EVENTS_H
