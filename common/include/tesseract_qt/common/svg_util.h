/**
 * @author Davide Faconti <davide.faconti@gmail.com>
 *
 * @copyright Copyright (C) 2015-2018 Davide Faconti <davide.faconti@gmail.com>
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
#ifndef TESSERACT_QT_COMMON_SVG_UTIL_H
#define TESSERACT_QT_COMMON_SVG_UTIL_H

#include <QPixmap>

namespace tesseract_gui
{
// Useful function to change the color of SVG icons programmatically.
// Useful to switch between dark view and light view.
// To work, the SVG file must use the color #ffffff and #000000 only.
QPixmap LoadSvg(QString filename, QString style_name = "light");
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_COMMON_SVG_UTIL_H
