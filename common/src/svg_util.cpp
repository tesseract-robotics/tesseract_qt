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
#ifdef QT_NO_SVGRENDERER
#error "QT_NO_SVGRENDERER defined"
#endif

#include <tesseract_qt/common/svg_util.h>

#include <QPixmap>
#include <QtSvg>
#include <QFile>
#include <QIcon>
#include <QTextStream>
#include <QByteArray>
#include <QPainter>
#include <QDebug>

namespace tesseract_gui
{
QPixmap LoadSvg(QString filename, QString style_name)
{
  QFile file(filename);
  file.open(QFile::ReadOnly | QFile::Text);
  auto svg_data = file.readAll();
  file.close();

  //  if (style_name.contains("light"))
  //  {
  //    svg_data.replace("#000000", "#111111");
  //    svg_data.replace("#ffffff", "#dddddd");
  //  }
  //  else
  //  {
  //    svg_data.replace("#000000", "#dddddd");
  //    svg_data.replace("#ffffff", "#111111");
  //  }

  if (style_name.contains("light"))
  {
    svg_data.replace("#000000", "#2ba5f7");
    svg_data.replace("#ffffff", "#2ba5f7");
  }
  else
  {
    svg_data.replace("#000000", "#2ba5f7");
    svg_data.replace("#ffffff", "#2ba5f7");
  }

  QByteArray content(svg_data);

  QSvgRenderer rr(content);
  QImage image(64, 64, QImage::Format_ARGB32);
  QPainter painter(&image);
  image.fill(Qt::transparent);
  rr.render(&painter);

  return QPixmap::fromImage(image);
}
}  // namespace tesseract_gui
