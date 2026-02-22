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

#ifndef TESSERACT_QT_COMMON_STATUS_LOG_MODEL_H
#define TESSERACT_QT_COMMON_STATUS_LOG_MODEL_H

#ifndef Q_MOC_RUN
#include <QStandardItemModel>
#include <Eigen/Eigen>
#endif

namespace tesseract::gui
{
class StatusLogModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit StatusLogModel(QObject* parent = nullptr);

private:
  void clear();

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};

}  // namespace tesseract::gui

#endif  // TESSERACT_QT_COMMON_STATUS_LOG_MODEL_H
