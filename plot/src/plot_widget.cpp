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
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDebug>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QPushButton>
#include <QWheelEvent>
#include <QSettings>
#include <QSvgGenerator>
#include <QClipboard>
#include <iostream>
#include <limits>
#include <set>
#include <memory>
#include <QtXml/QDomElement>

#include <qwt/qwt_scale_widget.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_glcanvas.h>
#include <qwt/qwt_scale_engine.h>
#include <qwt/qwt_scale_map.h>
#include <qwt/qwt_plot_layout.h>
#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_text.h>
#include <qwt/qwt_plot_renderer.h>
#include <qwt/qwt_series_data.h>
#include <qwt/qwt_date_scale_draw.h>

#include <tesseract_qt/plot/plot_widget.h>
#include <tesseract_qt/plot/plot_widget_editor.h>
#include <tesseract_qt/plot/plot_widget_transforms.h>
//#include "suggest_dialog.h"
//#include "transforms/custom_function.h"
//#include "transforms/custom_timeseries.h"

#include <tesseract_qt/plot/plot_zoomer.h>
#include <tesseract_qt/plot/plot_magnifier.h>
#include <tesseract_qt/plot/plot_legend.h>
#include <tesseract_qt/plot/point_series_xy.h>
#include <tesseract_qt/common/svg_util.h>

class TimeScaleDraw : public QwtScaleDraw
{
  virtual QwtText label(double v) const
  {
    QDateTime dt = QDateTime::fromMSecsSinceEpoch((qint64)(v * 1000));
    if (dt.date().year() == 1970 && dt.date().month() == 1 && dt.date().day() == 1)
    {
      return dt.toString("hh:mm:ss.z");
    }
    return dt.toString("hh:mm:ss.z\nyyyy MMM dd");
  }
};

const double MAX_DOUBLE = std::numeric_limits<double>::max() / 2;

static bool if_xy_plot_failed_show_dialog = true;

namespace tesseract_gui
{
PlotWidget::PlotWidget(PlotDataMapRef& datamap, QWidget* parent)
  : PlotWidgetBase(parent)
  , _mapped_data(datamap)
  , _tracker(nullptr)
  , _use_date_time_scale(false)
  , _dragging({ DragInfo::NONE, {}, nullptr })
  , _time_offset(0.0)
  //  , _transform_select_dialog(nullptr)
  , _context_menu_enabled(true)
{
  connect(this, &PlotWidget::curveListChanged, this, [this]() { this->updateMaximumZoomArea(); });

  qwtPlot()->setAcceptDrops(true);

  //--------------------------
  _tracker = (new CurveTracker(qwtPlot()));

  _grid = new QwtPlotGrid();
  _grid->setPen(QPen(Qt::gray, 0.0, Qt::DotLine));

  connect(this, &PlotWidgetBase::viewResized, this, &PlotWidget::on_externallyResized);

  connect(this, &PlotWidgetBase::dragEnterSignal, this, &PlotWidget::onDragEnterEvent);

  connect(this, &PlotWidgetBase::dropSignal, this, &PlotWidget::onDropEvent);

  //-------------------------

  buildActions();

  _custom_Y_limits.min = (-MAX_DOUBLE);
  _custom_Y_limits.max = (MAX_DOUBLE);

  //  QwtScaleWidget* bottomAxis = qwtPlot()->axisWidget( QwtPlot::xBottom );
  //  QwtScaleWidget* leftAxis = qwtPlot()->axisWidget( QwtPlot::yLeft );

  //  bottomAxis->installEventFilter(this);
  //  leftAxis->installEventFilter(this);
  //  qwtPlot()->canvas()->installEventFilter(this);
}

PlotWidget::~PlotWidget() = default;

void PlotWidget::setContextMenuEnabled(bool enabled) { _context_menu_enabled = enabled; }

void PlotWidget::buildActions()
{
  _action_edit = std::make_unique<QAction>("&Edit curves...", this);
  connect(_action_edit.get(), &QAction::triggered, this, [=]() {
    auto editor_dialog = new PlotWidgetEditor(this);
    editor_dialog->exec();
    editor_dialog->deleteLater();
    editor_dialog = nullptr;
  });

  _action_formula = std::make_unique<QAction>("&Apply filter to data...", this);
  connect(_action_formula.get(), &QAction::triggered, this, [=]() {
    auto editor_dialog = new DialogTransformEditor(this);
    int res = editor_dialog->exec();
    editor_dialog->deleteLater();
    editor_dialog = nullptr;
    if (res == QDialog::Accepted)
    {
      emit undoableChange();
    }
  });

  _action_split_horizontal = std::make_unique<QAction>("&Split Horizontally", this);
  connect(_action_split_horizontal.get(), &QAction::triggered, this, &PlotWidget::splitHorizontal);

  _action_split_vertical = std::make_unique<QAction>("&Split Vertically", this);
  connect(_action_split_vertical.get(), &QAction::triggered, this, &PlotWidget::splitVertical);

  _action_removeAllCurves = std::make_unique<QAction>("&Remove ALL curves", this);
  connect(_action_removeAllCurves.get(), &QAction::triggered, this, &PlotWidget::removeAllCurves);
  connect(_action_removeAllCurves.get(), &QAction::triggered, this, &PlotWidget::undoableChange);

  _action_zoomOutMaximum = std::make_unique<QAction>("&Zoom Out", this);
  connect(_action_zoomOutMaximum.get(), &QAction::triggered, this, [this]() {
    zoomOut(true);
    replot();
    emit undoableChange();
  });

  _action_zoomOutHorizontally = std::make_unique<QAction>("&Zoom Out Horizontally", this);
  connect(_action_zoomOutHorizontally.get(), &QAction::triggered, this, [this]() {
    on_zoomOutHorizontal_triggered(true);
    replot();
    emit undoableChange();
  });

  _action_zoomOutVertically = std::make_unique<QAction>("&Zoom Out Vertically", this);
  connect(_action_zoomOutVertically.get(), &QAction::triggered, this, [this]() {
    on_zoomOutVertical_triggered(true);
    replot();
    emit undoableChange();
  });

  QFont font;
  font.setPointSize(10);

  _action_saveToFile = std::make_unique<QAction>("&Save plot to file", this);
  connect(_action_saveToFile.get(), &QAction::triggered, this, &PlotWidget::on_savePlotToFile);

  _action_copy = std::make_unique<QAction>("&Copy", this);
  connect(_action_copy.get(), &QAction::triggered, this, &PlotWidget::on_copyAction_triggered);

  _action_paste = std::make_unique<QAction>("&Paste", this);
  connect(_action_paste.get(), &QAction::triggered, this, &PlotWidget::on_pasteAction_triggered);

  _action_image_to_clipboard = std::make_unique<QAction>("&Copy image to clipboard", this);
  connect(_action_image_to_clipboard.get(), &QAction::triggered, this, &PlotWidget::on_copyToClipboard);
}

void PlotWidget::canvasContextMenuTriggered(const QPoint& pos)
{
  if (_context_menu_enabled == false)
  {
    return;
  }

  QSettings settings;
  QString theme = settings.value("StyleSheet::theme", "light").toString();

  _action_removeAllCurves->setIcon(LoadSvg(":/tesseract_qt/svg/trash.svg", theme));
  _action_edit->setIcon(LoadSvg(":/tesseract_qt/svg/pencil-edit.svg", theme));
  _action_formula->setIcon(LoadSvg(":/tesseract_qt/svg/Fx.svg", theme));
  _action_split_horizontal->setIcon(LoadSvg(":/tesseract_qt/svg/add_column.svg", theme));
  _action_split_vertical->setIcon(LoadSvg(":/tesseract_qt/svg/add_row.svg", theme));
  _action_zoomOutMaximum->setIcon(LoadSvg(":/tesseract_qt/svg/zoom_max.svg", theme));
  _action_zoomOutHorizontally->setIcon(LoadSvg(":/tesseract_qt/svg/zoom_horizontal.svg", theme));
  _action_zoomOutVertically->setIcon(LoadSvg(":/tesseract_qt/svg/zoom_vertical.svg", theme));
  _action_copy->setIcon(LoadSvg(":/tesseract_qt/svg/copy.svg", theme));
  _action_paste->setIcon(LoadSvg(":/tesseract_qt/svg/paste.svg", theme));
  _action_saveToFile->setIcon(LoadSvg(":/tesseract_qt/svg/save.svg", theme));
  _action_image_to_clipboard->setIcon(LoadSvg(":/tesseract_qt/svg/plot_image.svg", theme));

  QMenu menu(qwtPlot());

  menu.addAction(_action_edit.get());
  menu.addAction(_action_formula.get());
  menu.addSeparator();
  menu.addAction(_action_split_horizontal.get());
  menu.addAction(_action_split_vertical.get());
  menu.addSeparator();
  menu.addAction(_action_zoomOutMaximum.get());
  menu.addAction(_action_zoomOutHorizontally.get());
  menu.addAction(_action_zoomOutVertically.get());
  menu.addSeparator();
  menu.addAction(_action_removeAllCurves.get());
  menu.addSeparator();
  menu.addAction(_action_copy.get());
  menu.addAction(_action_paste.get());
  menu.addAction(_action_image_to_clipboard.get());
  menu.addAction(_action_saveToFile.get());

  // check the clipboard
  QClipboard* clipboard = QGuiApplication::clipboard();
  QString clipboard_text = clipboard->text();
  QDomDocument doc;
  bool valid_clipbaord = (!clipboard_text.isEmpty() &&       // not empty
                          doc.setContent(clipboard_text) &&  // valid xml
                          doc.firstChildElement().tagName() == "PlotWidgetClipBoard");

  _action_paste->setEnabled(valid_clipbaord);

  _action_removeAllCurves->setEnabled(!curveList().empty());
  _action_formula->setEnabled(!curveList().empty() && !isXYPlot());

  menu.exec(qwtPlot()->canvas()->mapToGlobal(pos));
}

PlotWidget::CurveInfo* PlotWidget::addCurveXY(std::string name_x, std::string name_y, QString curve_name)
{
  if (curve_name.isEmpty())
    throw std::runtime_error("PlotWidget, curve_name cannot be empty!");

  if (name_x.empty())
    throw std::runtime_error("PlotWidget, name_x cannot be empty!");

  if (name_y.empty())
    throw std::runtime_error("PlotWidget, name_y cannot be empty!");

  std::string name = curve_name.toStdString();

  auto curve_it = curveFromTitle(curve_name);

  if (name.empty() || curve_it)
  {
    int ret = QMessageBox::warning(qwtPlot(),
                                   "Missing name",
                                   "The name of the curve is missing or exist already. "
                                   "Try again or abort.",
                                   QMessageBox::Abort | QMessageBox::Retry,
                                   QMessageBox::NoButton);
    if (ret == QMessageBox::Abort || ret == QMessageBox::NoButton)
    {
      return nullptr;
    }
    name.clear();
  }

  auto it = _mapped_data.numeric.find(name_x);
  if (it == _mapped_data.numeric.end())
  {
    throw std::runtime_error("Creation of XY plot failed");
  }
  PlotData& data_x = it->second;

  it = _mapped_data.numeric.find(name_y);
  if (it == _mapped_data.numeric.end())
  {
    throw std::runtime_error("Creation of XY plot failed");
  }
  PlotData& data_y = it->second;

  curve_it = curveFromTitle(curve_name);
  if (curve_it)
    return nullptr;

  const auto qname = QString::fromStdString(name);
  auto curve = new QwtPlotCurve(qname);

  try
  {
    auto plot_qwt = createCurveXY(&data_x, &data_y);

    curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, true);
    curve->setPaintAttribute(QwtPlotCurve::FilterPoints, true);
    //    curve->setPaintAttribute(QwtPlotCurve::FilterPointsAggressive, true);
    curve->setData(plot_qwt);
  }
  catch (std::exception& ex)
  {
    QMessageBox::warning(qwtPlot(), "Exception!", ex.what());
    return nullptr;
  }

  QColor color = getColorHint(nullptr);
  curve->setPen(color);
  setStyle(curve, curveStyle());

  curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

  curve->attach(qwtPlot());

  auto marker = new QwtPlotMarker;
  marker->attach(qwtPlot());
  marker->setVisible(isXYPlot());
  QwtSymbol* sym = new QwtSymbol(QwtSymbol::Ellipse, color, QPen(Qt::black), QSize(8, 8));
  marker->setSymbol(sym);

  CurveInfo curve_info;
  curve_info.curve = curve;
  curve_info.marker = marker;
  curve_info.src_name = name;
  curveList().push_back(curve_info);

  return &(curveList().back());
}

PlotWidgetBase::CurveInfo* PlotWidget::addCurve(const std::string& name, QColor color)
{
  auto it = _mapped_data.numeric.find(name);
  if (it == _mapped_data.numeric.end())
  {
    return nullptr;
  }
  return PlotWidgetBase::addCurve(name, it->second, color);
}

void PlotWidget::removeCurve(const QString& title)
{
  PlotWidgetBase::removeCurve(title);
  _tracker->redraw();
}

void PlotWidget::onDataSourceRemoved(const std::string& src_name)
{
  bool deleted = false;

  for (auto it = curveList().begin(); it != curveList().end();)
  {
    PointSeriesXY* curve_xy = dynamic_cast<PointSeriesXY*>(it->curve->data());
    bool remove_curve_xy =
        curve_xy && (curve_xy->dataX()->plotName() == src_name || curve_xy->dataY()->plotName() == src_name);

    if (it->src_name == src_name || remove_curve_xy)
    {
      deleted = true;

      it->curve->detach();
      it->marker->detach();

      it = curveList().erase(it);
    }
    else
    {
      it++;
    }
  }

  if (deleted)
  {
    _tracker->redraw();
    emit curveListChanged();
  }
}

void PlotWidget::removeAllCurves()
{
  PlotWidgetBase::removeAllCurves();
  setModeXY(false);
  _tracker->redraw();
}

void PlotWidget::onDragEnterEvent(QDragEnterEvent* event)
{
  const QMimeData* mimeData = event->mimeData();
  QStringList mimeFormats = mimeData->formats();
  _dragging.curves.clear();
  _dragging.source = event->source();

  for (const QString& format : mimeFormats)
  {
    QByteArray encoded = mimeData->data(format);
    QDataStream stream(&encoded, QIODevice::ReadOnly);

    while (!stream.atEnd())
    {
      QString curve_name;
      stream >> curve_name;
      if (!curve_name.isEmpty())
      {
        _dragging.curves.push_back(curve_name);
      }
    }

    if (format == "curveslist/add_curve")
    {
      _dragging.mode = DragInfo::CURVES;
      event->acceptProposedAction();
    }
    if (format == "curveslist/new_XY_axis")
    {
      if (_dragging.curves.size() != 2)
      {
        qDebug() << "FATAL: Dragging " << _dragging.curves.size() << " curves";
        return;
      }
      if (curveList().empty() || isXYPlot())
      {
        _dragging.mode = DragInfo::NEW_XY;
        event->acceptProposedAction();
      }
      else
      {
        event->ignore();
      }
    }
  }
}

void PlotWidget::onDropEvent(QDropEvent*)
{
  bool curves_changed = false;

  if (_dragging.mode == DragInfo::CURVES)
  {
    if (isXYPlot() && !curveList().empty())
    {
      _dragging.mode = DragInfo::NONE;
      _dragging.curves.clear();
      QMessageBox::warning(qwtPlot(),
                           "Warning",
                           tr("This is a XY plot, you can not drop normal time series "
                              "here.\n"
                              "Clear all curves to reset it to normal mode."));
      return;
    }
    else if (isXYPlot() && curveList().empty())
    {
      setModeXY(false);
    }

    for (const auto& curve_name : _dragging.curves)
    {
      bool added = addCurve(curve_name.toStdString()) != nullptr;
      curves_changed = curves_changed || added;
    }
  }
  else if (_dragging.mode == DragInfo::NEW_XY && _dragging.curves.size() == 2)
  {
    if (!curveList().empty() && !isXYPlot())
    {
      _dragging.mode = DragInfo::NONE;
      _dragging.curves.clear();
      QMessageBox::warning(qwtPlot(),
                           "Warning",
                           tr("To convert this widget into a XY plot, "
                              "you must first remove all the time series."));
      return;
    }

    setModeXY(true);
    addCurveXY(_dragging.curves[0].toStdString(), _dragging.curves[1].toStdString());

    curves_changed = true;
  }

  if (curves_changed)
  {
    emit curvesDropped();
    emit curveListChanged();
    zoomOut(true);
  }
  _dragging.mode = DragInfo::NONE;
  _dragging.curves.clear();
}

void PlotWidget::on_panned(int, int) { on_externallyResized(canvasBoundingRect()); }

QDomElement PlotWidget::xmlSaveState(QDomDocument& doc) const
{
  QDomElement plot_el = doc.createElement("plot");

  QDomElement range_el = doc.createElement("range");
  QRectF rect = canvasBoundingRect();
  range_el.setAttribute("bottom", QString::number(rect.bottom(), 'f', 6));
  range_el.setAttribute("top", QString::number(rect.top(), 'f', 6));
  range_el.setAttribute("left", QString::number(rect.left(), 'f', 6));
  range_el.setAttribute("right", QString::number(rect.right(), 'f', 6));
  plot_el.appendChild(range_el);

  QDomElement limitY_el = doc.createElement("limitY");
  if (_custom_Y_limits.min > -MAX_DOUBLE)
  {
    limitY_el.setAttribute("min", QString::number(_custom_Y_limits.min));
  }
  if (_custom_Y_limits.max < MAX_DOUBLE)
  {
    limitY_el.setAttribute("max", QString::number(_custom_Y_limits.max));
  }
  plot_el.appendChild(limitY_el);

  if (curveStyle() == PlotWidgetBase::LINES)
  {
    plot_el.setAttribute("style", "Lines");
  }
  //  else if (curveStyle() == PlotWidgetBase::LINES_AND_DOTS)
  //  {
  //    plot_el.setAttribute("style", "LinesAndDots");
  //  }
  else if (curveStyle() == PlotWidgetBase::DOTS)
  {
    plot_el.setAttribute("style", "Dots");
  }
  else if (curveStyle() == PlotWidgetBase::STICKS)
  {
    plot_el.setAttribute("style", "Sticks");
  }

  for (auto& it : curveList())
  {
    auto& name = it.src_name;
    QwtPlotCurve* curve = it.curve;
    QDomElement curve_el = doc.createElement("curve");
    curve_el.setAttribute("name", QString::fromStdString(name));
    curve_el.setAttribute("color", curve->pen().color().name());

    plot_el.appendChild(curve_el);

    if (isXYPlot())
    {
      PointSeriesXY* curve_xy = dynamic_cast<PointSeriesXY*>(curve->data());
      curve_el.setAttribute("curve_x", QString::fromStdString(curve_xy->dataX()->plotName()));
      curve_el.setAttribute("curve_y", QString::fromStdString(curve_xy->dataY()->plotName()));
    }
    else
    {
      auto ts = dynamic_cast<TransformedTimeseries*>(curve->data());
      if (ts && ts->transform())
      {
        QDomElement transform_el = doc.createElement("transform");
        transform_el.setAttribute("name", ts->transformName());
        transform_el.setAttribute("alias", ts->alias());
        //        ts->transform()->xmlSaveState(doc, transform_el);
        curve_el.appendChild(transform_el);
      }
    }
  }

  plot_el.setAttribute("mode", isXYPlot() ? "XYPlot" : "TimeSeries");

  return plot_el;
}

bool PlotWidget::xmlLoadState(QDomElement& plot_widget)
{
  std::set<std::string> added_curve_names;

  QString mode = plot_widget.attribute("mode");
  setModeXY(mode == "XYPlot");

  QDomElement limitY_el = plot_widget.firstChildElement("limitY");

  _custom_Y_limits.min = -MAX_DOUBLE;
  _custom_Y_limits.max = +MAX_DOUBLE;

  if (!limitY_el.isNull())
  {
    if (limitY_el.hasAttribute("min"))
    {
      _custom_Y_limits.min = limitY_el.attribute("min").toDouble();
    }
    if (limitY_el.hasAttribute("max"))
    {
      _custom_Y_limits.max = limitY_el.attribute("max").toDouble();
    }
  }

  static bool warning_message_shown = false;

  // removeAllCurves simplified
  for (auto& it : curveList())
  {
    it.curve->detach();
    it.marker->detach();
  }
  curveList().clear();

  // insert curves
  for (QDomElement curve_element = plot_widget.firstChildElement("curve"); !curve_element.isNull();
       curve_element = curve_element.nextSiblingElement("curve"))
  {
    QString curve_name = curve_element.attribute("name");
    std::string curve_name_std = curve_name.toStdString();
    QColor color(curve_element.attribute("color"));

    bool error = false;
    if (!isXYPlot())
    {
      if (_mapped_data.numeric.find(curve_name_std) == _mapped_data.numeric.end())
      {
        error = true;
      }
      else
      {
        auto curve_it = addCurve(curve_name_std, color);
        if (!curve_it)
        {
          continue;
        }
        auto& curve = curve_it->curve;
        curve->setPen(color, 1.3);
        added_curve_names.insert(curve_name_std);

        auto ts = dynamic_cast<TransformedTimeseries*>(curve->data());
        QDomElement transform_el = curve_element.firstChildElement("transform");
        if (transform_el.isNull() == false)
        {
          ts->setTransform(transform_el.attribute("name"));
          //          ts->transform()->xmlLoadState(transform_el);
          ts->updateCache(true);
          auto alias = transform_el.attribute("alias");
          ts->setAlias(alias);
          curve->setTitle(alias);
        }
      }
    }
    else
    {
      std::string curve_x = curve_element.attribute("curve_x").toStdString();
      std::string curve_y = curve_element.attribute("curve_y").toStdString();

      if (_mapped_data.numeric.find(curve_x) == _mapped_data.numeric.end() ||
          _mapped_data.numeric.find(curve_y) == _mapped_data.numeric.end())
      {
        error = true;
      }
      else
      {
        auto curve_it = addCurveXY(curve_x, curve_y, curve_name);
        if (!curve_it)
        {
          continue;
        }
        curve_it->curve->setPen(color, 1.3);
        curve_it->marker->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, color, QPen(Qt::black), QSize(8, 8)));
        added_curve_names.insert(curve_name_std);
      }
    }

    if (error && !warning_message_shown)
    {
      QMessageBox::warning(qwtPlot(),
                           "Warning",
                           tr("Can't find one or more curves.\n"
                              "This message will be shown only once."));
      warning_message_shown = true;
    }
  }

  emit curveListChanged();

  //-----------------------------------------

  QDomElement rectangle = plot_widget.firstChildElement("range");

  if (!rectangle.isNull())
  {
    QRectF rect;
    rect.setBottom(rectangle.attribute("bottom").toDouble());
    rect.setTop(rectangle.attribute("top").toDouble());
    rect.setLeft(rectangle.attribute("left").toDouble());
    rect.setRight(rectangle.attribute("right").toDouble());
    this->setZoomRectangle(rect, false);
  }

  if (plot_widget.hasAttribute("style"))
  {
    QString style = plot_widget.attribute("style");
    if (style == "Lines")
    {
      changeCurvesStyle(PlotWidgetBase::LINES);
    }
    //    else if (style == "LinesAndDots")
    //    {
    //      changeCurvesStyle(PlotWidgetBase::LINES_AND_DOTS);
    //    }
    else if (style == "Dots")
    {
      changeCurvesStyle(PlotWidgetBase::DOTS);
    }
    else if (style == "Sticks")
    {
      changeCurvesStyle(PlotWidgetBase::STICKS);
    }
  }

  updateMaximumZoomArea();
  replot();
  return true;
}

void PlotWidget::rescaleEqualAxisScaling()
{
  const QwtScaleMap xMap = qwtPlot()->canvasMap(QwtPlot::xBottom);
  const QwtScaleMap yMap = qwtPlot()->canvasMap(QwtPlot::yLeft);

  QRectF canvas_rect = qwtPlot()->canvas()->contentsRect();
  canvas_rect = canvas_rect.normalized();
  const double x1 = xMap.invTransform(canvas_rect.left());
  const double x2 = xMap.invTransform(canvas_rect.right());
  const double y1 = yMap.invTransform(canvas_rect.bottom());
  const double y2 = yMap.invTransform(canvas_rect.top());

  const double data_ratio = (x2 - x1) / (y2 - y1);
  const double canvas_ratio = canvas_rect.width() / canvas_rect.height();

  QRectF rect(QPointF(x1, y2), QPointF(x2, y1));

  if (data_ratio < canvas_ratio)
  {
    double new_width = fabs(rect.height() * canvas_ratio);
    double increment = new_width - rect.width();
    rect.setWidth(new_width);
    rect.moveLeft(rect.left() - 0.5 * increment);
  }
  else
  {
    double new_height = -(rect.width() / canvas_ratio);
    double increment = fabs(new_height - rect.height());
    rect.setHeight(new_height);
    rect.moveTop(rect.top() + 0.5 * increment);
  }
  if (rect.contains(maxZoomRect()))
  {
    rect = maxZoomRect();
  }

  qwtPlot()->setAxisScale(QwtPlot::yLeft, std::min(rect.bottom(), rect.top()), std::max(rect.bottom(), rect.top()));
  qwtPlot()->setAxisScale(QwtPlot::xBottom, std::min(rect.left(), rect.right()), std::max(rect.left(), rect.right()));
  qwtPlot()->updateAxes();
}

void PlotWidget::setZoomRectangle(QRectF rect, bool emit_signal)
{
  QRectF current_rect = canvasBoundingRect();
  if (current_rect == rect)
  {
    return;
  }
  qwtPlot()->setAxisScale(QwtPlot::yLeft, std::min(rect.bottom(), rect.top()), std::max(rect.bottom(), rect.top()));
  qwtPlot()->setAxisScale(QwtPlot::xBottom, std::min(rect.left(), rect.right()), std::max(rect.left(), rect.right()));
  qwtPlot()->updateAxes();

  if (isXYPlot() && keepRatioXY())
  {
    rescaleEqualAxisScaling();
  }

  if (emit_signal)
  {
    if (isXYPlot())
    {
      emit undoableChange();
    }
    else
    {
      emit rectChanged(this, rect);
    }
  }
}

void PlotWidget::reloadPlotData()
{
  // TODO: this needs MUCH more testing

  int visible = 0;

  for (auto& it : curveList())
  {
    if (it.curve->isVisible())
    {
      visible++;
    }

    const auto& curve_name = it.src_name;

    auto data_it = _mapped_data.numeric.find(curve_name);
    if (data_it != _mapped_data.numeric.end())
    {
      auto ts = dynamic_cast<TransformedTimeseries*>(it.curve->data());
      if (ts)
      {
        ts->updateCache(true);
      }
    }
  }

  if (curveList().size() == 0 || visible == 0)
  {
    setDefaultRangeX();
  }
}

void PlotWidget::activateLegend(bool activate) { legend()->setVisible(activate); }

void PlotWidget::activateGrid(bool activate)
{
  _grid->enableX(activate);
  _grid->enableXMin(activate);
  _grid->enableY(activate);
  _grid->enableYMin(activate);
  _grid->attach(qwtPlot());
}

void PlotWidget::configureTracker(CurveTracker::Parameter val) { _tracker->setParameter(val); }

void PlotWidget::enableTracker(bool enable) { _tracker->setEnabled(enable && !isXYPlot()); }

bool PlotWidget::isTrackerEnabled() const { return _tracker->isEnabled(); }

void PlotWidget::setTrackerPosition(double abs_time)
{
  if (isXYPlot())
  {
    for (auto& it : curveList())
    {
      auto series = dynamic_cast<QwtSeriesWrapper*>(it.curve->data());
      auto pointXY = series->sampleFromTime(abs_time);
      if (pointXY)
      {
        it.marker->setValue(pointXY.value());
      }
    }
  }
  else
  {
    double relative_time = abs_time - _time_offset;
    _tracker->setPosition(QPointF(relative_time, 0.0));
  }
}

void PlotWidget::on_changeTimeOffset(double offset)
{
  auto prev_offset = _time_offset;
  _time_offset = offset;

  if (fabs(prev_offset - offset) > std::numeric_limits<double>::epsilon())
  {
    for (auto& it : curveList())
    {
      auto series = dynamic_cast<QwtSeriesWrapper*>(it.curve->data());
      series->setTimeOffset(_time_offset);
    }
    if (!isXYPlot() && !curveList().empty())
    {
      QRectF rect = canvasBoundingRect();
      double delta = prev_offset - offset;
      rect.moveLeft(rect.left() + delta);
      setZoomRectangle(rect, false);
    }
  }
}

void PlotWidget::on_changeDateTimeScale(bool enable)
{
  _use_date_time_scale = enable;
  bool is_timescale = dynamic_cast<TimeScaleDraw*>(qwtPlot()->axisScaleDraw(QwtPlot::xBottom)) != nullptr;

  if (enable && !isXYPlot())
  {
    if (!is_timescale)
    {
      qwtPlot()->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
    }
  }
  else
  {
    if (is_timescale)
    {
      qwtPlot()->setAxisScaleDraw(QwtPlot::xBottom, new QwtScaleDraw);
    }
  }
}

// TODO report failure for empty dataset
Range PlotWidget::getVisualizationRangeY(Range range_X) const
{
  range_X.min += _time_offset;
  range_X.max += _time_offset;
  auto [bottom, top] = PlotWidgetBase::getVisualizationRangeY(range_X);

  const bool lower_limit = _custom_Y_limits.min > -MAX_DOUBLE;
  const bool upper_limit = _custom_Y_limits.max < MAX_DOUBLE;

  if (lower_limit)
  {
    bottom = _custom_Y_limits.min;
    if (top < bottom)
    {
      top = bottom;
    }
  }

  if (upper_limit)
  {
    top = _custom_Y_limits.max;
    if (top < bottom)
    {
      bottom = top;
    }
  }

  return Range({ bottom, top });
}

void PlotWidget::updateCurves(bool reset_older_data)
{
  for (auto& it : curveList())
  {
    auto series = dynamic_cast<QwtSeriesWrapper*>(it.curve->data());
    series->updateCache(reset_older_data);
    // TODO check res and do something if false.
  }
  updateMaximumZoomArea();
}

void PlotWidget::on_changeCurveColor(const QString& curve_name, QColor new_color)
{
  for (auto& it : curveList())
  {
    if (it.curve->title() == curve_name)
    {
      auto& curve = it.curve;
      if (curve->pen().color() != new_color)
      {
        curve->setPen(new_color, 1.3);
      }
      replot();
      break;
    }
  }
}

void PlotWidget::on_externallyResized(const QRectF& rect)
{
  QRectF current_rect = canvasBoundingRect();
  if (current_rect == rect)
  {
    return;
  }

  if (isXYPlot())
  {
    if (keepRatioXY())
    {
      rescaleEqualAxisScaling();
    }
    emit undoableChange();
  }
  else if (isZoomLinkEnabled())
  {
    emit rectChanged(this, rect);
  }
}

void PlotWidget::zoomOut(bool emit_signal)
{
  if (curveList().size() == 0)
  {
    QRectF rect(0, 1, 1, -1);
    this->setZoomRectangle(rect, false);
    return;
  }
  updateMaximumZoomArea();
  setZoomRectangle(maxZoomRect(), emit_signal);
  replot();
}

void PlotWidget::on_zoomOutHorizontal_triggered(bool emit_signal)
{
  updateMaximumZoomArea();
  QRectF act = canvasBoundingRect();
  auto rangeX = getVisualizationRangeX();

  act.setLeft(rangeX.min);
  act.setRight(rangeX.max);
  this->setZoomRectangle(act, emit_signal);
}

void PlotWidget::on_zoomOutVertical_triggered(bool emit_signal)
{
  updateMaximumZoomArea();
  QRectF rect = canvasBoundingRect();
  auto rangeY = getVisualizationRangeY({ rect.left(), rect.right() });

  rect.setBottom(rangeY.min);
  rect.setTop(rangeY.max);
  this->setZoomRectangle(rect, emit_signal);
}

void PlotWidget::setModeXY(bool enable)
{
  if (enable == isXYPlot())
  {
    return;
  }
  PlotWidgetBase::setModeXY(enable);

  enableTracker(!enable);

  if (enable)
  {
    QFont font_footer;
    font_footer.setPointSize(10);
    QwtText text("XY Plot");
    text.setFont(font_footer);
    qwtPlot()->setFooter(text);
  }
  else
  {
    qwtPlot()->setFooter("");
  }

  zoomOut(true);
  on_changeDateTimeScale(_use_date_time_scale);
  replot();
}

void PlotWidget::updateAvailableTransformers()
{
  QSettings settings;
  QByteArray xml_text = settings.value("AddCustomPlotDialog.savedXML", QByteArray()).toByteArray();
  if (!xml_text.isEmpty())
  {
    //    _snippets = GetSnippetsFromXML(xml_text);
  }
}

void PlotWidget::on_savePlotToFile()
{
  QString fileName;

  QFileDialog saveDialog(qwtPlot());
  saveDialog.setAcceptMode(QFileDialog::AcceptSave);

  QStringList filters;
  filters << "png (*.png)"
          << "jpg (*.jpg *.jpeg)"
          << "svg (*.svg)";

  saveDialog.setNameFilters(filters);
  saveDialog.exec();

  if (saveDialog.result() == QDialog::Accepted && !saveDialog.selectedFiles().empty())
  {
    fileName = saveDialog.selectedFiles().first();

    if (fileName.isEmpty())
    {
      return;
    }

    bool is_svg = false;
    QFileInfo fileinfo(fileName);
    if (fileinfo.suffix().isEmpty())
    {
      auto filter = saveDialog.selectedNameFilter();
      if (filter == filters[0])
      {
        fileName.append(".png");
      }
      else if (filter == filters[1])
      {
        fileName.append(".jpg");
      }
      else if (filter == filters[2])
      {
        fileName.append(".svg");
        is_svg = true;
      }
    }

    bool tracker_enabled = _tracker->isEnabled();
    if (tracker_enabled)
    {
      this->enableTracker(false);
      replot();
    }

    QRect documentRect(0, 0, 1200, 900);
    QwtPlotRenderer rend;

    if (is_svg)
    {
      QSvgGenerator generator;
      generator.setFileName(fileName);
      generator.setResolution(80);
      generator.setViewBox(documentRect);
      QPainter painter(&generator);
      rend.render(qwtPlot(), &painter, documentRect);
    }
    else
    {
      QPixmap pixmap(1200, 900);
      QPainter painter(&pixmap);
      rend.render(qwtPlot(), &painter, documentRect);
      pixmap.save(fileName);
    }

    if (tracker_enabled)
    {
      this->enableTracker(true);
      replot();
    }
  }
}

void PlotWidget::setCustomAxisLimits(Range range)
{
  _custom_Y_limits = range;
  on_zoomOutVertical_triggered(false);
  replot();
}

Range PlotWidget::customAxisLimit() const { return _custom_Y_limits; }

void PlotWidget::on_copyToClipboard()
{
  bool tracker_enabled = _tracker->isEnabled();
  if (tracker_enabled)
  {
    this->enableTracker(false);
    replot();
  }

  auto documentRect = qwtPlot()->canvas()->rect();
  qDebug() << documentRect;

  QwtPlotRenderer rend;
  QPixmap pixmap(documentRect.width(), documentRect.height());
  QPainter painter(&pixmap);
  rend.render(qwtPlot(), &painter, documentRect);

  QClipboard* clipboard = QGuiApplication::clipboard();
  clipboard->setPixmap(pixmap);

  if (tracker_enabled)
  {
    this->enableTracker(true);
    replot();
  }
}

void PlotWidget::on_copyAction_triggered()
{
  QDomDocument doc;
  auto root = doc.createElement("PlotWidgetClipBoard");
  auto el = xmlSaveState(doc);
  doc.appendChild(root);
  root.appendChild(el);

  QClipboard* clipboard = QGuiApplication::clipboard();
  clipboard->setText(doc.toString());
}

void PlotWidget::on_pasteAction_triggered()
{
  QClipboard* clipboard = QGuiApplication::clipboard();
  QString clipboard_text = clipboard->text();

  QDomDocument doc;
  bool valid = doc.setContent(clipboard_text);
  if (!valid)
  {
    return;
  }
  auto root = doc.firstChildElement();
  if (root.tagName() != "PlotWidgetClipBoard")
  {
    return;
  }
  else
  {
    auto el = root.firstChildElement();
    xmlLoadState(el);
    clipboard->setText("");
    emit undoableChange();
  }
}

bool PlotWidget::eventFilter(QObject* obj, QEvent* event)
{
  if (PlotWidgetBase::eventFilter(obj, event))
  {
    return true;
  }

  if (event->type() == QEvent::Destroy)
  {
    return false;
  }

  if (obj == qwtPlot()->canvas())
  {
    return canvasEventFilter(event);
  }
  return false;
}

void PlotWidget::overrideCursonMove()
{
  QSettings settings;
  QString theme = settings.value("Preferences::theme", "light").toString();
  auto pixmap = LoadSvg(":/tesseract_qt/svg/move_view.svg", theme);
  QApplication::setOverrideCursor(QCursor(pixmap.scaled(24, 24)));
}

bool PlotWidget::isZoomLinkEnabled() const
{
  for (const auto& it : curveList())
  {
    auto* series = dynamic_cast<QwtSeriesWrapper*>(it.curve->data());
    if (series->plotData()->attribute(PlotAttribute::DISABLE_LINKED_ZOOM).toBool())
    {
      return false;
    }
  }
  return true;
}

bool PlotWidget::canvasEventFilter(QEvent* event)
{
  switch (event->type())
  {
    case QEvent::MouseButtonPress:
    {
      if (_dragging.mode != DragInfo::NONE)
      {
        return true;  // don't pass to canvas().
      }

      QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

      if (mouse_event->button() == Qt::LeftButton)
      {
        const QPoint press_point = mouse_event->pos();
        if (mouse_event->modifiers() == Qt::ShiftModifier)  // time tracker
        {
          QPointF pointF(qwtPlot()->invTransform(QwtPlot::xBottom, press_point.x()),
                         qwtPlot()->invTransform(QwtPlot::yLeft, press_point.y()));
          emit trackerMoved(pointF);
          return true;  // don't pass to canvas().
        }
        else if (mouse_event->modifiers() == Qt::ControlModifier)  // panner
        {
          overrideCursonMove();
        }
        return false;  // send to canvas()
      }
      else if (mouse_event->buttons() == Qt::MiddleButton && mouse_event->modifiers() == Qt::NoModifier)
      {
        overrideCursonMove();
        return false;
      }
      else if (mouse_event->button() == Qt::RightButton)
      {
        if (mouse_event->modifiers() == Qt::NoModifier)  // show menu
        {
          canvasContextMenuTriggered(mouse_event->pos());
          return true;  // don't pass to canvas().
        }
      }
    }
    break;
      //---------------------------------
    case QEvent::MouseMove:
    {
      if (_dragging.mode != DragInfo::NONE)
      {
        return true;  // don't pass to canvas().
      }

      QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

      if (mouse_event->buttons() == Qt::LeftButton && mouse_event->modifiers() == Qt::ShiftModifier)
      {
        const QPoint point = mouse_event->pos();
        QPointF pointF(qwtPlot()->invTransform(QwtPlot::xBottom, point.x()),
                       qwtPlot()->invTransform(QwtPlot::yLeft, point.y()));
        emit trackerMoved(pointF);
        return true;
      }
    }
    break;

    case QEvent::Leave:
    {
      _dragging.mode = DragInfo::NONE;
      _dragging.curves.clear();
    }
    break;
    case QEvent::MouseButtonRelease:
    {
      if (_dragging.mode == DragInfo::NONE)
      {
        QApplication::restoreOverrideCursor();
        return false;
      }
    }
    break;

  }  // end switch

  return false;
}

void PlotWidget::setDefaultRangeX()
{
  if (!curveList().empty())
  {
    double min = std::numeric_limits<double>::max();
    double max = -std::numeric_limits<double>::max();
    for (auto& it : _mapped_data.numeric)
    {
      const PlotData& data = it.second;
      if (data.size() > 0)
      {
        double A = data.front().x;
        double B = data.back().x;
        min = std::min(A, min);
        max = std::max(B, max);
      }
    }
    qwtPlot()->setAxisScale(QwtPlot::xBottom, min - _time_offset, max - _time_offset);
  }
  else
  {
    qwtPlot()->setAxisScale(QwtPlot::xBottom, 0.0, 1.0);
  }
}

QwtSeriesWrapper* PlotWidget::createCurveXY(const PlotData* data_x, const PlotData* data_y)
{
  QwtSeriesWrapper* output = nullptr;

  try
  {
    output = new PointSeriesXY(data_x, data_y);
  }
  catch (std::runtime_error& ex)
  {
    if (if_xy_plot_failed_show_dialog)
    {
      QMessageBox msgBox(qwtPlot());
      msgBox.setWindowTitle("Warnings");
      msgBox.setText(tr("The creation of the XY plot failed with the following "
                        "message:\n %1")
                         .arg(ex.what()));
      msgBox.addButton("Continue", QMessageBox::AcceptRole);
      msgBox.exec();
    }
    throw std::runtime_error("Creation of XY plot failed");
  }

  output->setTimeOffset(_time_offset);
  return output;
}

QwtSeriesWrapper* PlotWidget::createTimeSeries(const QString& transform_ID, const PlotData* data)
{
  auto* output = new TransformedTimeseries(data);
  output->setTransform(transform_ID);
  output->setTimeOffset(_time_offset);
  output->updateCache(true);
  return output;
}

void PlotWidget::changeBackgroundColor(QColor color)
{
  if (qwtPlot()->canvasBackground().color() != color)
  {
    qwtPlot()->setCanvasBackground(color);
    replot();
  }
}
}  // namespace tesseract_gui
