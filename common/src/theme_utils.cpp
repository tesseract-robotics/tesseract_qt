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

#include <tesseract_qt/common/theme_utils.h>

#include <QFile>

namespace tesseract_gui::themes
{
QString getTheme(const QString& resource)
{
  QFile file(resource);
  file.open(QFile::ReadOnly);

  return QLatin1String(file.readAll());
}

QString getDarkTheme()
{
  static QString styleSheet = getTheme(":/qdarkstyle/dark/qdarkstyle_dark.qss");
  return styleSheet;
}

QString getLightTheme()
{
  static QString styleSheet = getTheme(":/qdarkstyle/light/qdarkstyle_light.qss");
  return styleSheet;
}
}  // namespace tesseract_gui::themes
