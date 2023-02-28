/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
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
#ifndef TESSERACT_QT_COMMON_APPLICATION_EVENTS_H
#define TESSERACT_QT_COMMON_APPLICATION_EVENTS_H

#include <tesseract_qt/common/events/event_type.h>

#include <QEvent>
#include <QString>

namespace tesseract_gui::events
{
class StatusMessageShow : public QEvent
{
public:
  StatusMessageShow(QString message, int timeout = 0);
  ~StatusMessageShow() override;

  QString message;
  int timeout;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::APP_STATUS_MESSAGE_SHOW);
};

class StatusMessageClear : public QEvent
{
public:
  StatusMessageClear();
  ~StatusMessageClear() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::APP_STATUS_MESSAGE_CLEAR);
};
}  // namespace tesseract_gui::events
#endif  // TESSERACT_QT_COMMON_APPLICATION_EVENTS_H
