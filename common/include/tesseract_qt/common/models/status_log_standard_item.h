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

#ifndef TESSERACT_QT_COMMON_STATUS_LOG_STANDARD_ITEM_H
#define TESSERACT_QT_COMMON_STATUS_LOG_STANDARD_ITEM_H

#include <boost/uuid/uuid.hpp>
#include <tesseract_common/eigen_types.h>
#include <tesseract_qt/common/events/log_events.h>
#include <tesseract_qt/common/models/standard_item_type.h>

#include <QStandardItem>

namespace tesseract_gui
{
class StatusLogStandardItem : public QStandardItem
{
public:
  explicit StatusLogStandardItem(const events::LogInfoEvent& message);
  explicit StatusLogStandardItem(const events::LogWarnEvent& message);
  explicit StatusLogStandardItem(const events::LogErrorEvent& message);

  QStandardItem* getTimeStamp() const;
  QStandardItem* getSeverity() const;
  QStandardItem* getText() const;

  int type() const override;

private:
  void ctor(const events::LogInfoEvent& message);
  void ctor(const events::LogWarnEvent& message);
  void ctor(const events::LogErrorEvent& message);
  StandardItemType type_;
  QStandardItem* time_stamp_;
  QStandardItem* severity_;
  QStandardItem* text_;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_COMMON_STATUS_LOG_STANDARD_ITEM_H
