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
#include <tesseract_qt/plot/plot_widget_base.h>
#include <tesseract_qt/plot/timeseries_qwt.h>
#include <tesseract_qt/plot/plot_magnifier.h>
#include <tesseract_qt/plot/plot_zoomer.h>
#include <tesseract_qt/plot/plot_legend.h>

#include <qwt/qwt_legend.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_glcanvas.h>
#include <qwt/qwt_plot_rescaler.h>
#include <qwt/qwt_plot_panner.h>
#include <qwt/qwt_plot_legenditem.h>
#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_plot_layout.h>
#include <qwt/qwt_scale_engine.h>
#include <qwt/qwt_scale_map.h>
#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_scale_widget.h>
#include <qwt/qwt_symbol.h>
#include <qwt/qwt_text.h>

#include <QBoxLayout>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>

static int _global_color_index_ = 0;

namespace tesseract_gui
{
class QwtPlotPimpl : public QwtPlot
{
public:
  PlotLegend* legend;
  PlotMagnifier* magnifier;
  QwtPlotPanner* panner1;
  QwtPlotPanner* panner2;
  PlotZoomer* zoomer;
  std::function<void(const QRectF&)> resized_callback;
  std::function<void(QEvent*)> event_callback;
  PlotWidgetBase* parent;

  QwtPlotPimpl(PlotWidgetBase* parentObject,
               QWidget* canvas,
               std::function<void(const QRectF&)> resizedViewCallback,
               std::function<void(QEvent*)> eventCallback)
    : QwtPlot(nullptr), resized_callback(resizedViewCallback), event_callback(eventCallback), parent(parentObject)
  {
    this->setCanvas(canvas);

    legend = new PlotLegend(this);
    magnifier = new PlotMagnifier(this->canvas());
    panner1 = new QwtPlotPanner(this->canvas());
    panner2 = new QwtPlotPanner(this->canvas());
    zoomer = new PlotZoomer(this->canvas());

    zoomer->setRubberBandPen(QColor(Qt::red, 1, Qt::DotLine));
    zoomer->setTrackerPen(QColor(Qt::green, 1, Qt::DotLine));
    zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::NoModifier);

    magnifier->setAxisEnabled(QwtPlot::xTop, false);
    magnifier->setAxisEnabled(QwtPlot::yRight, false);

    magnifier->setZoomInKey(Qt::Key_Plus, Qt::ControlModifier);
    magnifier->setZoomOutKey(Qt::Key_Minus, Qt::ControlModifier);

    // disable right button. keep mouse wheel
    magnifier->setMouseButton(Qt::NoButton);

    panner1->setMouseButton(Qt::LeftButton, Qt::ControlModifier);
    panner2->setMouseButton(Qt::MiddleButton, Qt::NoModifier);

    connect(zoomer, &PlotZoomer::zoomed, this, [this](const QRectF& r) { resized_callback(r); });

    connect(magnifier, &PlotMagnifier::rescaled, this, [this](const QRectF& r) {
      resized_callback(r);
      replot();
    });

    connect(panner1, &QwtPlotPanner::panned, this, [this]() { resized_callback(canvasBoundingRect()); });

    connect(panner2, &QwtPlotPanner::panned, this, [this]() { resized_callback(canvasBoundingRect()); });

    QwtScaleWidget* bottomAxis = axisWidget(QwtPlot::xBottom);
    QwtScaleWidget* leftAxis = axisWidget(QwtPlot::yLeft);

    bottomAxis->installEventFilter(parent);
    leftAxis->installEventFilter(parent);
    this->canvas()->installEventFilter(parent);
  }

  ~QwtPlotPimpl() override
  {
    QwtScaleWidget* bottomAxis = axisWidget(QwtPlot::xBottom);
    QwtScaleWidget* leftAxis = axisWidget(QwtPlot::yLeft);

    bottomAxis->installEventFilter(parent);
    leftAxis->removeEventFilter(parent);
    canvas()->removeEventFilter(parent);

    setCanvas(nullptr);
  }

  QRectF canvasBoundingRect() const
  {
    QRectF rect;
    rect.setBottom(canvasMap(QwtPlot::yLeft).s1());
    rect.setTop(canvasMap(QwtPlot::yLeft).s2());
    rect.setLeft(canvasMap(QwtPlot::xBottom).s1());
    rect.setRight(canvasMap(QwtPlot::xBottom).s2());
    return rect;
  }

  virtual void resizeEvent(QResizeEvent* ev) override
  {
    QwtPlot::resizeEvent(ev);
    resized_callback(canvasBoundingRect());
  }

  std::list<PlotWidgetBase::CurveInfo> curve_list;

  PlotWidgetBase::CurveStyle curve_style = PlotWidgetBase::LINES;

  bool zoom_enabled = true;

  void dragEnterEvent(QDragEnterEvent* event) override { event_callback(event); }

  void dropEvent(QDropEvent* event) override { event_callback(event); }
};

QwtPlot* PlotWidgetBase::qwtPlot() { return p; }

const QwtPlot* PlotWidgetBase::qwtPlot() const { return p; }

QWidget* PlotWidgetBase::widget() { return qwtPlot(); }

const QWidget* PlotWidgetBase::widget() const { return qwtPlot(); }

void PlotWidgetBase::resetZoom()
{
  updateMaximumZoomArea();
  QRectF rect = maxZoomRect();

  qwtPlot()->setAxisScale(QwtPlot::yLeft, std::min(rect.bottom(), rect.top()), std::max(rect.bottom(), rect.top()));
  qwtPlot()->setAxisScale(QwtPlot::xBottom, std::min(rect.left(), rect.right()), std::max(rect.left(), rect.right()));
  qwtPlot()->updateAxes();

  replot();
}

Range PlotWidgetBase::getVisualizationRangeX() const
{
  double left = std::numeric_limits<double>::max();
  double right = -std::numeric_limits<double>::max();

  for (auto& it : curveList())
  {
    if (!it.curve->isVisible())
      continue;

    auto series = dynamic_cast<QwtSeriesWrapper*>(it.curve->data());
    const auto max_range_X = series->getVisualizationRangeX();
    if (!max_range_X)
    {
      continue;
    }

    left = std::min(max_range_X->min, left);
    right = std::max(max_range_X->max, right);
  }

  if (left > right)
  {
    left = 0;
    right = 0;
  }

  double margin = 0.0;
  if (fabs(right - left) > std::numeric_limits<double>::epsilon())
  {
    margin = isXYPlot() ? ((right - left) * 0.025) : 0.0;
  }
  right = right + margin;
  left = left - margin;

  return Range({ left, right });
}

Range PlotWidgetBase::getVisualizationRangeY(Range range_X) const
{
  double top = -std::numeric_limits<double>::max();
  double bottom = std::numeric_limits<double>::max();

  for (auto& it : curveList())
  {
    if (!it.curve->isVisible())
      continue;

    auto series = dynamic_cast<QwtSeriesWrapper*>(it.curve->data());

    const auto max_range_X = series->plotData()->rangeX();
    if (!max_range_X)
    {
      continue;
    }

    double left = std::max(max_range_X->min, range_X.min);
    double right = std::min(max_range_X->max, range_X.max);
    left = std::nextafter(left, right);
    right = std::nextafter(right, left);

    auto range_Y = series->getVisualizationRangeY({ left, right });
    if (!range_Y)
    {
      qDebug() << " invalid range_Y in PlotWidget::maximumRangeY";
      continue;
    }
    if (top < range_Y->max)
    {
      top = range_Y->max;
    }
    if (bottom > range_Y->min)
    {
      bottom = range_Y->min;
    }
  }

  double margin = 0.1;

  if (bottom > top)
  {
    bottom = 0;
    top = 0;
  }

  if (top - bottom > std::numeric_limits<double>::epsilon())
  {
    margin = (top - bottom) * 0.025;
  }

  top += margin;
  bottom -= margin;

  return Range({ bottom, top });
}

bool PlotWidgetBase::isXYPlot() const { return _xy_mode; }

QRectF PlotWidgetBase::canvasBoundingRect() const { return p->canvasBoundingRect(); }

QRectF PlotWidgetBase::maxZoomRect() const { return _max_zoom_rect; }

void PlotWidgetBase::setModeXY(bool enable) { _xy_mode = enable; }

PlotWidgetBase::PlotWidgetBase(QWidget* parent) : _xy_mode(false), _keep_aspect_ratio(false)
{
  auto onViewResized = [this](const QRectF& r) { emit viewResized(r); };

  auto onEvent = [this](QEvent* event) {
    if (auto ev = dynamic_cast<QDragEnterEvent*>(event))
    {
      emit dragEnterSignal(ev);
    }
    else if (auto ev = dynamic_cast<QDropEvent*>(event))
    {
      emit dropSignal(ev);
    }
  };

  QSettings settings;
  bool use_opengl = settings.value("Preferences::use_opengl", false).toBool();

  QWidget* abs_canvas;
  if (use_opengl)
  {
    auto canvas = new QwtPlotGLCanvas();
    canvas->setFrameStyle(QFrame::NoFrame);
    canvas->setFrameStyle(QFrame::Box | QFrame::Plain);
    canvas->setLineWidth(1);
    canvas->setPalette(Qt::white);
    abs_canvas = canvas;
  }
  else
  {
    auto canvas = new QwtPlotCanvas();
    canvas->setFrameStyle(QFrame::NoFrame);
    canvas->setFrameStyle(QFrame::Box | QFrame::Plain);
    canvas->setLineWidth(1);
    canvas->setPalette(Qt::white);
    canvas->setPaintAttribute(QwtPlotCanvas::BackingStore, true);
    abs_canvas = canvas;
  }
  abs_canvas->setObjectName("qwtCanvas");

  p = new QwtPlotPimpl(this, abs_canvas, onViewResized, onEvent);

  qwtPlot()->setMinimumWidth(100);
  qwtPlot()->setMinimumHeight(100);

  qwtPlot()->sizePolicy().setHorizontalPolicy(QSizePolicy::Expanding);
  qwtPlot()->sizePolicy().setVerticalPolicy(QSizePolicy::Expanding);

  qwtPlot()->canvas()->setMouseTracking(true);

  qwtPlot()->setCanvasBackground(Qt::white);
  qwtPlot()->setAxisAutoScale(QwtPlot::yLeft, true);
  qwtPlot()->setAxisAutoScale(QwtPlot::xBottom, true);

  qwtPlot()->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating, true);
  qwtPlot()->plotLayout()->setAlignCanvasToScales(true);

  qwtPlot()->setAxisScale(QwtPlot::xBottom, 0.0, 1.0);
  qwtPlot()->setAxisScale(QwtPlot::yLeft, 0.0, 1.0);
}

PlotWidgetBase::~PlotWidgetBase()
{
  if (p)
  {
    p->deleteLater();
    p = nullptr;
  }
}

PlotWidgetBase::CurveInfo* PlotWidgetBase::addCurve(const std::string& name, PlotData& data, QColor color)
{
  const auto qname = QString::fromStdString(name);

  // title is the same of src_name, unless a transform was applied
  auto curve_it = curveFromTitle(qname);
  if (curve_it)
  {
    return nullptr;  // TODO FIXME
  }

  auto curve = new QwtPlotCurve(qname);
  try
  {
    auto plot_qwt = createTimeSeries("", &data);

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

  if (color == Qt::transparent)
  {
    color = getColorHint(&data);
  }

  curve->setPen(color);
  setStyle(curve, p->curve_style);

  curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

  curve->attach(qwtPlot());

  auto marker = new QwtPlotMarker;
  marker->attach(qwtPlot());
  marker->setVisible(false);

  QwtSymbol* sym = new QwtSymbol(QwtSymbol::Ellipse, Qt::red, QPen(Qt::black), QSize(8, 8));
  marker->setSymbol(sym);

  CurveInfo curve_info;
  curve_info.curve = curve;
  curve_info.marker = marker;
  curve_info.src_name = name;

  p->curve_list.push_back(curve_info);

  return &(p->curve_list.back());
}

bool PlotWidgetBase::isEmpty() const { return p->curve_list.empty(); }

void PlotWidgetBase::removeCurve(const QString& title)
{
  auto it = std::find_if(p->curve_list.begin(), p->curve_list.end(), [&title](const PlotWidgetBase::CurveInfo& info) {
    return info.curve->title() == title;
  });

  if (it != p->curve_list.end())
  {
    it->curve->detach();
    it->marker->detach();
    p->curve_list.erase(it);

    emit curveListChanged();
  }
}

const std::list<PlotWidgetBase::CurveInfo>& PlotWidgetBase::curveList() const { return p->curve_list; }

std::list<PlotWidgetBase::CurveInfo>& PlotWidgetBase::curveList() { return p->curve_list; }

QwtSeriesWrapper* PlotWidgetBase::createTimeSeries(const QString& transform_ID, const PlotData* data)
{
  TransformedTimeseries* output = new TransformedTimeseries(data);
  output->setTransform(transform_ID);
  output->updateCache(true);
  return output;
}

PlotWidgetBase::CurveStyle PlotWidgetBase::curveStyle() const { return p->curve_style; }

bool PlotWidgetBase::keepRatioXY() const { return _keep_aspect_ratio; }

void PlotWidgetBase::setKeepRatioXY(bool active)
{
  _keep_aspect_ratio = active;
  if (isXYPlot() && active)
  {
    zoomer()->keepAspectRatio(true);
  }
  else
  {
    zoomer()->keepAspectRatio(false);
  }
}

void PlotWidgetBase::setAcceptDrops(bool accept) { qwtPlot()->setAcceptDrops(accept); }

bool PlotWidgetBase::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == QEvent::Destroy)
  {
    return false;
  }

  QwtPlot* plot = qwtPlot();
  QwtScaleWidget* bottomAxis = plot->axisWidget(QwtPlot::xBottom);
  QwtScaleWidget* leftAxis = plot->axisWidget(QwtPlot::yLeft);

  if (magnifier() && (obj == bottomAxis || obj == leftAxis) && !(isXYPlot() && keepRatioXY()))
  {
    if (event->type() == QEvent::Wheel)
    {
      auto wheel_event = dynamic_cast<QWheelEvent*>(event);
      if (obj == bottomAxis)
      {
        magnifier()->setDefaultMode(PlotMagnifier::X_AXIS);
      }
      else
      {
        magnifier()->setDefaultMode(PlotMagnifier::Y_AXIS);
      }
      magnifier()->widgetWheelEvent(wheel_event);
    }
  }
  if (obj == plot->canvas())
  {
    if (magnifier())
    {
      magnifier()->setDefaultMode(PlotMagnifier::BOTH_AXES);
    }
    switch (event->type())
    {
      case QEvent::Wheel:
      {
        auto mouse_event = dynamic_cast<QWheelEvent*>(event);

        bool ctrl_modifier = mouse_event->modifiers() == Qt::ControlModifier;
        auto legend_rect = legend()->geometry(plot->canvas()->rect());

        if (ctrl_modifier)
        {
          if (legend_rect.contains(mouse_event->pos()) && legend()->isVisible())
          {
            int prev_size = legend()->font().pointSize();
            int new_size = prev_size;
            if (mouse_event->angleDelta().y() > 0)
            {
              new_size = std::min(13, prev_size + 1);
            }
            if (mouse_event->angleDelta().y() < 0)
            {
              new_size = std::max(7, prev_size - 1);
            }
            if (new_size != prev_size)
            {
              setLegendSize(new_size);
              emit legendSizeChanged(new_size);
            }
            return true;
          }
        }
        return false;
      }
      //-------------------
      case QEvent::MouseButtonPress:
      {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton && mouse_event->modifiers() == Qt::NoModifier)
        {
          auto clicked_item = legend()->processMousePressEvent(mouse_event);
          if (clicked_item)
          {
            for (auto& it : curveList())
            {
              if (clicked_item == it.curve)
              {
                it.curve->setVisible(!it.curve->isVisible());
                //_tracker->redraw();
                resetZoom();
                replot();
                return true;
              }
            }
          }
        }
      }
    }
  }
  return false;
}

QColor PlotWidgetBase::getColorHint(PlotData* data)
{
  QSettings settings;
  bool remember_color = settings.value("Preferences::remember_color", true).toBool();

  if (data)
  {
    auto colorHint = data->attribute("ColorHint");
    if (remember_color && colorHint.isValid())
    {
      return colorHint.value<QColor>();
    }
  }
  QColor color;
  bool use_plot_color_index = settings.value("Preferences::use_plot_color_index", false).toBool();
  int index = p->curve_list.size();

  if (!use_plot_color_index)
  {
    index = (_global_color_index_++);
  }

  // https://matplotlib.org/3.1.1/users/dflt_style_changes.html
  switch (index % 8)
  {
    case 0:
      color = QColor("#1f77b4");
      break;
    case 1:
      color = QColor("#d62728");
      break;
    case 2:
      color = QColor("#1ac938");
      break;
    case 3:
      color = QColor("#ff7f0e");
      break;

    case 4:
      color = QColor("#f14cc1");
      break;
    case 5:
      color = QColor("#9467bd");
      break;
    case 6:
      color = QColor("#17becf");
      break;
    case 7:
      color = QColor("#bcbd22");
      break;
  }
  if (data)
  {
    data->setAttribute("ColorHint", color);
  }

  return color;
}

std::map<QString, QColor> PlotWidgetBase::getCurveColors() const
{
  std::map<QString, QColor> color_by_name;

  for (auto& it : p->curve_list)
  {
    color_by_name.insert({ it.curve->title().text(), it.curve->pen().color() });
  }
  return color_by_name;
}

void PlotWidgetBase::setStyle(QwtPlotCurve* curve, CurveStyle style)
{
  curve->setPen(curve->pen().color(), (style == DOTS) ? 4.0 : 1.3);

  switch (style)
  {
    case LINES:
      curve->setStyle(QwtPlotCurve::Lines);
      break;
      //    case LINES_AND_DOTS:
      //      curve->setStyle(QwtPlotCurve::LinesAndDots);
      //      break;
    case DOTS:
      curve->setStyle(QwtPlotCurve::Dots);
      break;
    case STICKS:
      curve->setStyle(QwtPlotCurve::Sticks);
  }
}

void PlotWidgetBase::changeCurvesStyle(CurveStyle style)
{
  p->curve_style = style;
  for (auto& it : p->curve_list)
  {
    setStyle(it.curve, style);
  }
  replot();
}

PlotWidgetBase::CurveInfo* PlotWidgetBase::curveFromTitle(const QString& title)
{
  for (auto& info : p->curve_list)
  {
    if (info.curve->title() == title)
    {
      return &info;
    }
    if (info.src_name == title.toStdString())
    {
      return &info;
    }
  }
  return nullptr;
}

void PlotWidgetBase::setTitle(const QString& title) { qwtPlot()->setTitle(title); }

void PlotWidgetBase::setXAxisTitle(const QString& title) { qwtPlot()->setAxisTitle(QwtPlot::xBottom, title); }

void PlotWidgetBase::setYAxisTitle(const QString& title) { qwtPlot()->setAxisTitle(QwtPlot::yLeft, title); }

void PlotWidgetBase::setLegendSize(int size)
{
  auto font = p->legend->font();
  font.setPointSize(size);
  p->legend->setFont(font);
  replot();
}

void PlotWidgetBase::setLegendAlignment(Qt::Alignment alignment)
{
  p->legend->setAlignment(Qt::Alignment(Qt::AlignTop | alignment));
}

void PlotWidgetBase::setZoomEnabled(bool enabled)
{
  p->zoom_enabled = enabled;
  p->zoomer->setEnabled(enabled);
  p->magnifier->setEnabled(enabled);
  p->panner1->setEnabled(enabled);
  p->panner2->setEnabled(enabled);
}

bool PlotWidgetBase::isZoomEnabled() const { return p->zoom_enabled; }

void PlotWidgetBase::replot()
{
  if (p->zoomer)
  {
    p->zoomer->setZoomBase(false);
  }
  qwtPlot()->replot();
}

void PlotWidgetBase::removeAllCurves()
{
  for (auto& it : curveList())
  {
    it.curve->detach();
    it.marker->detach();
  }

  curveList().clear();
  emit curveListChanged();
  replot();
}

PlotLegend* PlotWidgetBase::legend() { return p->legend; }
PlotZoomer* PlotWidgetBase::zoomer() { return p->zoomer; }

PlotMagnifier* PlotWidgetBase::magnifier() { return p->magnifier; }

void PlotWidgetBase::updateMaximumZoomArea()
{
  QRectF max_rect;
  auto rangeX = getVisualizationRangeX();
  max_rect.setLeft(rangeX.min);
  max_rect.setRight(rangeX.max);

  rangeX.min = -std::numeric_limits<double>::max();
  rangeX.max = std::numeric_limits<double>::max();
  auto rangeY = getVisualizationRangeY(rangeX);
  max_rect.setBottom(rangeY.min);
  max_rect.setTop(rangeY.max);

  if (isXYPlot() && _keep_aspect_ratio)
  {
    const QRectF canvas_rect = p->canvas()->contentsRect();
    const double canvas_ratio = fabs(canvas_rect.width() / canvas_rect.height());
    const double data_ratio = fabs(max_rect.width() / max_rect.height());
    if (data_ratio < canvas_ratio)
    {
      // height is negative!!!!
      double new_width = fabs(max_rect.height() * canvas_ratio);
      double increment = new_width - max_rect.width();
      max_rect.setWidth(new_width);
      max_rect.moveLeft(max_rect.left() - 0.5 * increment);
    }
    else
    {
      // height must be negative!!!!
      double new_height = -(max_rect.width() / canvas_ratio);
      double increment = fabs(new_height - max_rect.height());
      max_rect.setHeight(new_height);
      max_rect.moveTop(max_rect.top() + 0.5 * increment);
    }
    magnifier()->setAxisLimits(QwtPlot::xBottom, max_rect.left(), max_rect.right());
    magnifier()->setAxisLimits(QwtPlot::yLeft, max_rect.bottom(), max_rect.top());
    zoomer()->keepAspectRatio(true);
  }
  else
  {
    magnifier()->setAxisLimits(QwtPlot::xBottom, max_rect.left(), max_rect.right());
    magnifier()->setAxisLimits(QwtPlot::yLeft, max_rect.bottom(), max_rect.top());
    zoomer()->keepAspectRatio(false);
  }
  _max_zoom_rect = max_rect;
}

}  // namespace tesseract_gui
