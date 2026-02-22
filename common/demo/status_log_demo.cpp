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

#include <QApplication>
#include <QTableView>
#include <QVBoxLayout>

#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/status_log_events.h>
#include <tesseract_qt/common/models/status_log_model.h>
#include <tesseract_qt/common/widgets/status_log_tool_bar.h>
#include <tesseract_qt/common/widgets/status_log_widget.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);

  QWidget widget;
  auto layout = new QVBoxLayout();
  layout->setMargin(0);
  layout->setSpacing(0);
  layout->addWidget(new tesseract::gui::StatusLogToolBar());
  layout->addWidget(new tesseract::gui::StatusLogWidget());
  widget.setLayout(layout);
  widget.show();

  tesseract::gui::events::StatusLogInfo event("Info Test 1, This is a long message to show that text wrapping is "
                                              "working!");
  QApplication::sendEvent(qApp, &event);

  tesseract::gui::events::StatusLogWarn event1("Warn Test 1");
  QApplication::sendEvent(qApp, &event1);

  tesseract::gui::events::StatusLogError event2("Error Test 1");
  QApplication::sendEvent(qApp, &event2);

  tesseract::gui::events::StatusLogInfo event3("Info Test 2");
  QApplication::sendEvent(qApp, &event3);

  tesseract::gui::events::StatusLogError event4("Error Test 2");
  QApplication::sendEvent(qApp, &event4);

  tesseract::gui::events::StatusLogWarn event5("Warn Test 2");
  QApplication::sendEvent(qApp, &event5);

  return QApplication::exec();
}
