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

#include <QApplication>
#include <QDebug>
#include <sstream>

#include <tesseract_qt/acm/allowed_collision_matrix_model.h>
#include <tesseract_qt/acm/allowed_collision_matrix_widget.h>

void onEntrySelected(const tesseract_common::AllowedCollisionEntries& selection)
{
  std::stringstream ss;
  ss << "Selected Rows:" << std::endl;
  for (const auto& s : selection)
    ss << "     (" << s.first.first << ", " << s.first.second << ", " << s.second << ")" << std::endl;

  qDebug() << QString::fromStdString(ss.str());
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  std::vector<std::string> links{ "link_1", "link_2", "link_3", "link_4" };

  tesseract_gui::AllowedCollisionMatrixModel model;
  model.add("link_1", "link_2", "Adjacent");
  model.add("link_2", "link_3", "Adjacent");
  model.add("link_3", "link_4", "Adjacent");

  tesseract_gui::AllowedCollisionMatrixWidget widget;
  QObject::connect(&widget, &tesseract_gui::AllowedCollisionMatrixWidget::entrySelected, &onEntrySelected);

  widget.setModel(&model);
  widget.show();

  return app.exec();
}
