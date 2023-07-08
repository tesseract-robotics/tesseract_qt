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
#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <QApplication>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/common/models/json_model.h>
#include <QTreeView>
#include <QJsonDocument>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);

  std::string str = R"({
                           "firstName": "John",
                           "lastName": "Smith",
                           "age": 25,
                           "address": {
                               "streetAddress": "21 2nd Street",
                               "city": "New York",
                               "state": "NY",
                               "postalCode": "10021"
                           },
                           "phoneNumber": [
                               {
                                   "type": "home",
                                   "number": "212 555-1234"
                               },
                               {
                                   "type": "fax",
                                   "number": "646 555-4567"
                               }
                           ]
                       })";

  QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(str).toUtf8());
  assert(!doc.isNull());
  tesseract_gui::QJsonModel model;
  model.loadJson(doc.toBinaryData());
  QTreeView widget;
  widget.setModel(&model);
  widget.show();

  return QApplication::exec();
}
