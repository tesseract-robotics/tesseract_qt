/**
 * @author Davide Faconti <davide.faconti@gmail.com>
 *
 * @copyright Copyright (C) 2015-2018 Davide Faconti <davide.faconti@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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
#include <tesseract_qt/plot/plot_docker.h>
#include <tesseract_qt/plot/plot_widget_editor.h>
#include <tesseract_qt/common/svg_util.h>
#include <DockSplitter.h>
#include <QPushButton>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QSplitter>
#include <QDebug>
#include <QInputDialog>
#include <QLineEdit>
#include "ui_plot_docker_toolbar.h"

namespace tesseract_gui
{
class SplittableComponentsFactory : public ads::CDockComponentsFactory
{
public:
  ads::CDockAreaTitleBar* createDockAreaTitleBar(ads::CDockAreaWidget* dock_area) const override
  {
    auto title_bar = new ads::CDockAreaTitleBar(dock_area);
    title_bar->setVisible(false);
    return title_bar;
  }
};

PlotDocker::PlotDocker(QString name, PlotDataMapRef& datamap, QWidget* parent)
  : ads::CDockManager(parent), _name(name), _datamap(datamap)
{
  ads::CDockComponentsFactory::setFactory(new SplittableComponentsFactory());

  auto CreateFirstWidget = [&]() {
    if (dockAreaCount() == 0)
    {
      DockWidget* widget = new DockWidget(datamap, this);

      auto area = addDockWidget(ads::TopDockWidgetArea, widget);
      area->setAllowedAreas(ads::OuterDockAreas);

      this->plotWidgetAdded(widget->plotWidget());

      connect(widget, &DockWidget::undoableChange, this, &PlotDocker::undoableChange);
    }
  };

  connect(this, &ads::CDockManager::dockWidgetRemoved, this, CreateFirstWidget);

  connect(this, &ads::CDockManager::dockAreasAdded, this, &PlotDocker::undoableChange);

  CreateFirstWidget();
}

QString PlotDocker::name() const { return _name; }

void PlotDocker::setName(QString name) { _name = name; }

QDomElement saveChildNodesState(QDomDocument& doc, QWidget* widget)
{
  QSplitter* splitter = qobject_cast<QSplitter*>(widget);
  if (splitter)
  {
    QDomElement splitter_elem = doc.createElement("DockSplitter");
    splitter_elem.setAttribute("orientation", (splitter->orientation() == Qt::Horizontal) ? "|" : "-");
    splitter_elem.setAttribute("count", QString::number(splitter->count()));

    QString sizes_str;
    int total_size = 0;
    for (int size : splitter->sizes())
    {
      total_size += size;
    }
    for (int size : splitter->sizes())
    {
      sizes_str += QString::number(double(size) / double(total_size));
      sizes_str += ";";
    }
    sizes_str.resize(sizes_str.size() - 1);
    splitter_elem.setAttribute("sizes", sizes_str);

    for (int i = 0; i < splitter->count(); ++i)
    {
      auto child = saveChildNodesState(doc, splitter->widget(i));
      splitter_elem.appendChild(child);
    }
    return splitter_elem;
  }
  else
  {
    ads::CDockAreaWidget* dockArea = qobject_cast<ads::CDockAreaWidget*>(widget);
    if (dockArea)
    {
      QDomElement area_elem = doc.createElement("DockArea");
      for (int i = 0; i < dockArea->dockWidgetsCount(); ++i)
      {
        auto dock_widget = dynamic_cast<DockWidget*>(dockArea->dockWidget(i));
        if (dock_widget)
        {
          auto plotwidget_elem = dock_widget->plotWidget()->xmlSaveState(doc);
          area_elem.appendChild(plotwidget_elem);
          area_elem.setAttribute("name", dock_widget->toolBar()->label()->text());
        }
      }
      return area_elem;
    }
  }
  return {};
}

int PlotDocker::plotCount() const { return dockAreaCount(); }

PlotWidget* PlotDocker::plotAt(int index)
{
  DockWidget* dock_widget = dynamic_cast<DockWidget*>(dockArea(index)->currentDockWidget());
  return static_cast<PlotWidget*>(dock_widget->plotWidget());
}

void PlotDocker::setHorizontalLink(bool enabled)
{
  // TODO
}

void PlotDocker::zoomOut()
{
  for (int index = 0; index < plotCount(); index++)
  {
    plotAt(index)->zoomOut(false);  // TODO is it false?
  }
}

void PlotDocker::replot()
{
  for (int index = 0; index < plotCount(); index++)
  {
    plotAt(index)->replot();
  }
}

void PlotDocker::on_stylesheetChanged(QString theme)
{
  for (int index = 0; index < plotCount(); index++)
  {
    auto dock_widget = static_cast<DockWidget*>(dockArea(index)->currentDockWidget());
    dock_widget->toolBar()->on_stylesheetChanged(theme);
  }
}

DockWidget::DockWidget(PlotDataMapRef& datamap, QWidget* parent) : ads::CDockWidget("Plot", parent), _datamap(datamap)
{
  setFrameShape(QFrame::NoFrame);
  _plot_widget = std::make_unique<PlotWidget>(datamap, this);
  setWidget(_plot_widget->widget());
  setFeature(ads::CDockWidget::DockWidgetFloatable, false);
  setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);

  _toolbar = new DraggableToolbar(this);
  _toolbar->label()->setText("...");
  qobject_cast<QBoxLayout*>(layout())->insertWidget(0, _toolbar);

  connect(_toolbar->buttonSplitHorizontal(), &QPushButton::clicked, this, &DockWidget::splitHorizontal);

  connect(_toolbar->buttonSplitVertical(), &QPushButton::clicked, this, &DockWidget::splitVertical);

  connect(_plot_widget.get(), &PlotWidget::splitHorizontal, this, &DockWidget::splitHorizontal);

  connect(_plot_widget.get(), &PlotWidget::splitVertical, this, &DockWidget::splitVertical);

  auto FullscreenAction = [=]() {
    PlotDocker* parent_docker = static_cast<PlotDocker*>(dockManager());

    this->toolBar()->toggleFullscreen();
    bool fullscreen = this->toolBar()->isFullscreen();

    for (int i = 0; i < parent_docker->dockAreaCount(); i++)
    {
      auto area = parent_docker->dockArea(i);
      if (area != dockAreaWidget())
      {
        area->setVisible(!fullscreen);
      }
      this->toolBar()->buttonClose()->setHidden(fullscreen);
    }
  };

  QObject::connect(_toolbar->buttonFullscreen(), &QPushButton::clicked, FullscreenAction);

  QObject::connect(_toolbar->buttonClose(), &QPushButton::pressed, [=]() {
    dockAreaWidget()->closeArea();
    this->undoableChange();
  });

  this->layout()->setMargin(10);
}

DockWidget::~DockWidget() = default;

DockWidget* DockWidget::splitHorizontal()
{
  // create a sibling (same parent)
  auto new_widget = new DockWidget(_datamap, qobject_cast<QWidget*>(parent()));

  PlotDocker* parent_docker = static_cast<PlotDocker*>(dockManager());
  auto area = parent_docker->addDockWidget(ads::RightDockWidgetArea, new_widget, dockAreaWidget());

  area->setAllowedAreas(ads::OuterDockAreas);

  parent_docker->plotWidgetAdded(new_widget->plotWidget());

  connect(this, &DockWidget::undoableChange, parent_docker, &PlotDocker::undoableChange);

  this->undoableChange();

  return new_widget;
}

DockWidget* DockWidget::splitVertical()
{
  auto new_widget = new DockWidget(_datamap, qobject_cast<QWidget*>(parent()));

  PlotDocker* parent_docker = static_cast<PlotDocker*>(dockManager());

  auto area = parent_docker->addDockWidget(ads::BottomDockWidgetArea, new_widget, dockAreaWidget());

  area->setAllowedAreas(ads::OuterDockAreas);
  parent_docker->plotWidgetAdded(new_widget->plotWidget());

  connect(this, &DockWidget::undoableChange, parent_docker, &PlotDocker::undoableChange);

  this->undoableChange();

  return new_widget;
}

PlotWidget* DockWidget::plotWidget() { return _plot_widget.get(); }

DraggableToolbar* DockWidget::toolBar() { return _toolbar; }

static void setButtonIcon(QPushButton* button, QIcon icon)
{
  button->setIcon(icon);
  button->setText("");
}

DraggableToolbar::DraggableToolbar(ads::CDockWidget* parent)
  : QWidget(parent), _parent(parent), ui(std::make_unique<Ui::DraggableToolbar>()), _fullscreen_mode(false)
{
  ui->setupUi(this);

  QSettings settings;
  QString theme = settings.value("StyleSheet::theme", "light").toString();
  on_stylesheetChanged(theme);

  ui->buttonFullscreen->setVisible(false);
  ui->buttonSplitHorizontal->setVisible(false);
  ui->buttonSplitVertical->setVisible(false);

  setMouseTracking(true);
  ui->widgetButtons->setMouseTracking(true);

  ui->label->installEventFilter(this);
}

DraggableToolbar::~DraggableToolbar() = default;

QLabel* DraggableToolbar::label() { return ui->label; }

QPushButton* DraggableToolbar::buttonFullscreen() { return ui->buttonFullscreen; }

QPushButton* DraggableToolbar::buttonClose() { return ui->buttonClose; }

QPushButton* DraggableToolbar::buttonSplitHorizontal() { return ui->buttonSplitHorizontal; }

QPushButton* DraggableToolbar::buttonSplitVertical() { return ui->buttonSplitVertical; }

void DraggableToolbar::toggleFullscreen()
{
  _fullscreen_mode = !_fullscreen_mode;

  setButtonIcon(ui->buttonFullscreen, _fullscreen_mode ? _collapse_icon : _expand_icon);

  ui->buttonClose->setHidden(_fullscreen_mode);
  if (_fullscreen_mode)
  {
    ui->buttonSplitHorizontal->setVisible(false);
    ui->buttonSplitVertical->setVisible(false);
  }
}

void DraggableToolbar::enterEvent(QEvent* ev)
{
  ui->buttonFullscreen->setVisible(true);
  ui->buttonSplitHorizontal->setVisible(!_fullscreen_mode);
  ui->buttonSplitVertical->setVisible(!_fullscreen_mode);

  ev->accept();
  QWidget::enterEvent(ev);
}

bool DraggableToolbar::eventFilter(QObject* object, QEvent* event)
{
  if (event->type() == QEvent::MouseButtonDblClick)
  {
    bool ok = true;
    QString newName = QInputDialog::getText(
        this, tr("Change name of the Area"), tr("New name:"), QLineEdit::Normal, ui->label->text(), &ok);
    if (ok)
    {
      ui->label->setText(newName);
    }
    return true;
  }
  else
  {
    return QObject::eventFilter(object, event);
  }
}

void DraggableToolbar::on_stylesheetChanged(QString theme)
{
  _expand_icon = LoadSvg(":/tesseract_widgets/svg/expand.svg", theme);
  _collapse_icon = LoadSvg(":/tesseract_widgets/svg/collapse.svg", theme);
  setButtonIcon(ui->buttonFullscreen, _fullscreen_mode ? _collapse_icon : _expand_icon);
  setButtonIcon(ui->buttonClose, LoadSvg(":/tesseract_widgets/svg/close-button.svg", theme));
  setButtonIcon(ui->buttonSplitHorizontal, LoadSvg(":/tesseract_widgets/svg/add_column.svg", theme));
  setButtonIcon(ui->buttonSplitVertical, LoadSvg(":/tesseract_widgets/svg/add_row.svg", theme));
}

void DraggableToolbar::leaveEvent(QEvent* ev)
{
  ui->buttonFullscreen->setVisible(_fullscreen_mode);
  ui->buttonSplitHorizontal->setVisible(false);
  ui->buttonSplitVertical->setVisible(false);
  QWidget::leaveEvent(ev);
}
}  // namespace tesseract_gui
