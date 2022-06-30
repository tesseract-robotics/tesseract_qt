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
#ifndef TESSERACT_QT_PLOT_PLOT_WIDGET_H
#define TESSERACT_QT_PLOT_PLOT_WIDGET_H

#include <map>
#include <deque>
#include <QObject>
#include <QTextEdit>
#include <QDomDocument>
#include <QMessageBox>
#include <QTime>

#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_symbol.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_plot_rescaler.h>
#include <qwt/qwt_plot_panner.h>
#include <qwt/qwt_plot_legenditem.h>

#include <tesseract_qt/plot/plot_widget_base.h>
#include <tesseract_qt/plot/custom_tracker.h>

//#include "transforms/transform_selector.h"
//#include "transforms/custom_function.h"

namespace tesseract_gui
{
class PlotWidget : public PlotWidgetBase
{
  Q_OBJECT

public:
  PlotWidget(PlotDataMapRef& datamap, QWidget* parent = nullptr);

  void setContextMenuEnabled(bool enabled);

  virtual ~PlotWidget() override;

  QDomElement xmlSaveState(QDomDocument& doc) const;

  bool xmlLoadState(QDomElement& element);

  Range getVisualizationRangeY(Range range_X) const override;

  void setZoomRectangle(QRectF rect, bool emit_signal);

  void reloadPlotData();

  void changeBackgroundColor(QColor color);

  double timeOffset() const { return _time_offset; }

  PlotDataMapRef& datamap() { return _mapped_data; }

  CurveInfo* addCurveXY(std::string name_x, std::string name_y, QString curve_name = "");

  CurveInfo* addCurve(const std::string& name, QColor color = Qt::transparent);

  void setCustomAxisLimits(Range range);

  Range customAxisLimit() const;

  void removeCurve(const QString& title) override;

  bool isZoomLinkEnabled() const;

protected:
  PlotDataMapRef& _mapped_data;

  bool eventFilter(QObject* obj, QEvent* event) override;
  void onDragEnterEvent(QDragEnterEvent* event);
  void onDropEvent(QDropEvent* event);

  bool canvasEventFilter(QEvent* event);

Q_SIGNALS:
  void swapWidgetsRequested(PlotWidget* source, PlotWidget* destination);
  void rectChanged(PlotWidget* self, QRectF rect);
  void undoableChange();
  void trackerMoved(QPointF pos);
  void curveListChanged();
  void curvesDropped();
  void splitHorizontal();
  void splitVertical();

public Q_SLOTS:

  void updateCurves(bool reset_older_data);

  void onDataSourceRemoved(const std::string& src_name);

  void removeAllCurves() override;

  void on_panned(int dx, int dy);

  void zoomOut(bool emit_signal);

  void on_zoomOutHorizontal_triggered(bool emit_signal = true);

  void on_zoomOutVertical_triggered(bool emit_signal = true);

  void activateLegend(bool activate);

  void activateGrid(bool activate);

  void configureTracker(CurveTracker::Parameter val);

  void enableTracker(bool enable);

  bool isTrackerEnabled() const;

  void setTrackerPosition(double abs_time);

  void on_changeTimeOffset(double offset);

  void on_changeDateTimeScale(bool enable);

  void on_changeCurveColor(const QString& curve_name, QColor new_color);

private Q_SLOTS:

  // void on_changeToBuiltinTransforms(QString new_transform);

  void setModeXY(bool enable) override;

  void on_savePlotToFile();

  void on_copyToClipboard();

  void on_copyAction_triggered();

  void on_pasteAction_triggered();

private Q_SLOTS:

  void canvasContextMenuTriggered(const QPoint& pos);

  void on_externallyResized(const QRectF& new_rect);

private:
  std::unique_ptr<QAction> _action_removeAllCurves;
  std::unique_ptr<QAction> _action_edit;
  std::unique_ptr<QAction> _action_formula;
  std::unique_ptr<QAction> _action_split_horizontal;
  std::unique_ptr<QAction> _action_split_vertical;
  std::unique_ptr<QAction> _action_zoomOutMaximum;
  std::unique_ptr<QAction> _action_zoomOutHorizontally;
  std::unique_ptr<QAction> _action_zoomOutVertically;
  std::unique_ptr<QAction> _action_saveToFile;
  std::unique_ptr<QAction> _action_copy;
  std::unique_ptr<QAction> _action_paste;
  std::unique_ptr<QAction> _action_image_to_clipboard;

  CurveTracker* _tracker;
  QwtPlotGrid* _grid;

  bool _use_date_time_scale;

  struct DragInfo
  {
    enum
    {
      NONE,
      CURVES,
      NEW_XY
    } mode;
    std::vector<QString> curves;
    QObject* source;
  };

  DragInfo _dragging;

  void buildActions();

  void updateAvailableTransformers();

  void setDefaultRangeX();

  QwtSeriesWrapper* createCurveXY(const PlotData* data_x, const PlotData* data_y);

  QwtSeriesWrapper* createTimeSeries(const QString& transform_ID, const PlotData* data) override;

  double _time_offset;

  Range _custom_Y_limits;

  //  TransformSelector* _transform_select_dialog;

  //  SnippetsMap _snippets;

  bool _context_menu_enabled;

  // void updateMaximumZoomArea();
  void rescaleEqualAxisScaling();
  void overrideCursonMove();
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_PLOT_PLOT_WIDGET_H
