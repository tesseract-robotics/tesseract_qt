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
#include <QHBoxLayout>
#include <QObject>
#include <QLatin1String>
#include <QDebug>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/plot/plot_widget.h>
#include <tesseract_qt/common/theme_utils.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  // setup stylesheet
  app.setStyleSheet(tesseract_gui::themes::getDarkTheme());

  QWidget window;
  window.resize(320, 240);
  window.setWindowTitle(QApplication::translate("childwidget", "Child widget"));

  QHBoxLayout* layout = new QHBoxLayout(&window);  // NOLINT

  tesseract_gui::PlotDataMapRef plot_data_map;
  tesseract_gui::PlotData& cosine_data = plot_data_map.getOrCreateNumeric("cosine");
  tesseract_gui::PlotData& sine_data = plot_data_map.getOrCreateNumeric("sine");
  for (int i = 0; i < int((2 * M_PI) / 0.01); ++i)
  {
    cosine_data.pushBack(tesseract_gui::PlotDataXY::Point(i * 0.01, std::cos(i * 0.01)));
    sine_data.pushBack(tesseract_gui::PlotDataXY::Point(i * 0.01, std::sin(i * 0.01)));
  }

  auto widget = std::make_unique<tesseract_gui::PlotWidget>(plot_data_map);
  widget->addCurve("cosine");
  widget->addCurve("sine");
  widget->enableTracker(true);
  widget->replot();

  QObject::connect(widget.get(), &tesseract_gui::PlotWidget::trackerMoved, [&](QPointF pos) {
    widget->setTrackerPosition(pos.x());
    widget->replot();
  });

  layout->addWidget(widget->widget(), 0, 0);

  window.show();

  return app.exec();
}
