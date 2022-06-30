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
#ifndef TESSERACT_QT_PLOT_PLOT_WIDGET_BASE_H
#define TESSERACT_QT_PLOT_PLOT_WIDGET_BASE_H

#include <QWidget>
#include <tesseract_qt/plot/plot_data.h>
#include <tesseract_qt/plot/timeseries_qwt.h>

class QwtPlot;
class QwtPlotCurve;
class QwtPlotPanner;
class QwtPlotMarker;

namespace tesseract_gui
{
class QwtPlotPimpl;
class PlotZoomer;
class PlotMagnifier;
class PlotLegend;

class PlotWidgetBase : public QObject
{
  Q_OBJECT

public:
  enum CurveStyle
  {
    LINES,
    DOTS,
    //    LINES_AND_DOTS,
    STICKS
  };

  struct CurveInfo
  {
    std::string src_name;
    QwtPlotCurve* curve;
    QwtPlotMarker* marker;
  };

  PlotWidgetBase(QWidget* parent);

  ~PlotWidgetBase();

  virtual CurveInfo* addCurve(const std::string& name, PlotData& src_data, QColor color = Qt::transparent);

  virtual void removeCurve(const QString& title);

  const std::list<CurveInfo>& curveList() const;

  bool isEmpty() const;

  QColor getColorHint(PlotData* data);

  std::map<QString, QColor> getCurveColors() const;

  CurveInfo* curveFromTitle(const QString& title);

  virtual QwtSeriesWrapper* createTimeSeries(const QString& transform_ID, const PlotData* data);

  virtual void resetZoom();

  virtual Range getVisualizationRangeX() const;

  virtual Range getVisualizationRangeY(Range range_X) const;

  virtual void setModeXY(bool enable);

  void setTitle(const QString& title);

  void setXAxisTitle(const QString& title);

  void setYAxisTitle(const QString& title);

  void setLegendSize(int size);

  void setLegendAlignment(Qt::Alignment alignment);

  void setZoomEnabled(bool enabled);

  bool isZoomEnabled() const;

  void changeCurvesStyle(CurveStyle style);

  bool isXYPlot() const;

  QRectF canvasBoundingRect() const;

  QRectF maxZoomRect() const;

  QWidget* widget();

  const QWidget* widget() const;

  CurveStyle curveStyle() const;

  bool keepRatioXY() const;

  void setKeepRatioXY(bool active);

  void setAcceptDrops(bool accept);

public Q_SLOTS:

  void replot();

  virtual void removeAllCurves();

Q_SIGNALS:

  void curveListChanged();

  void viewResized(const QRectF&);

  void dragEnterSignal(QDragEnterEvent* event);

  void dropSignal(QDropEvent* event);

  void legendSizeChanged(int new_size);

protected:
  QwtPlotPimpl* p = nullptr;

  static void setStyle(QwtPlotCurve* curve, CurveStyle style);

  QwtPlot* qwtPlot();
  const QwtPlot* qwtPlot() const;

  std::list<CurveInfo>& curveList();

  PlotLegend* legend();
  PlotZoomer* zoomer();
  PlotMagnifier* magnifier();

  void updateMaximumZoomArea();

  bool eventFilter(QObject* obj, QEvent* event);

private:
  bool _xy_mode;

  QRectF _max_zoom_rect;

  bool _keep_aspect_ratio;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_PLOT_PLOT_WIDGET_BASE_H
