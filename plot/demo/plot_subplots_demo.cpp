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
#include <QObject>
#include <QDebug>

#include <tesseract_qt/plot/plot_subplots.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  tesseract_gui::PlotDataMapRef plot_data_map;
  tesseract_gui::PlotData& cosine_data = plot_data_map.getOrCreateNumeric("cosine");
  tesseract_gui::PlotData& sine_data = plot_data_map.getOrCreateNumeric("sine");
  for (int i = 0; i < int((2 * M_PI) / 0.01); ++i)
  {
    cosine_data.pushBack(tesseract_gui::PlotDataXY::Point(i * 0.01, std::cos(i * 0.01)));
    sine_data.pushBack(tesseract_gui::PlotDataXY::Point(i * 0.01, std::sin(i * 0.01)));
  }

  tesseract_gui::PlotSubplots subplots(plot_data_map, 2, 1);
  subplots.resize(320, 240);
  subplots.setWindowTitle(QApplication::translate("subplots", "Subplots"));

  {  // Subplot [0, 0]
    tesseract_gui::PlotWidget* widget = subplots.getSubplot(0, 0);
    widget->addCurve("cosine");
    widget->setTitle("Cosine");
    widget->setXAxisTitle("Time (seconds)");
    widget->setYAxisTitle("Amplitude");
    widget->enableTracker(true);
    widget->zoomOut(true);
    widget->replot();

    QObject::connect(widget, &tesseract_gui::PlotWidget::trackerMoved, [widget](QPointF pos) {
      widget->setTrackerPosition(pos.x());
      widget->replot();
    });
  }

  {  // Subplot [1, 0]
    tesseract_gui::PlotWidget* widget = subplots.getSubplot(1, 0);
    widget->addCurve("sine");
    widget->setTitle("Sine");
    widget->setXAxisTitle("Time (seconds)");
    widget->setYAxisTitle("Amplitude");
    widget->enableTracker(true);
    widget->zoomOut(true);
    widget->replot();

    QObject::connect(widget, &tesseract_gui::PlotWidget::trackerMoved, [widget](QPointF pos) {
      widget->setTrackerPosition(pos.x());
      widget->replot();
    });
  }

  subplots.show();

  return app.exec();
}
