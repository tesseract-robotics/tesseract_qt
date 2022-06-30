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
#ifndef TESSERACT_QT_PLOT_PLOT_DOCKER_H
#define TESSERACT_QT_PLOT_PLOT_DOCKER_H

//#include "Qads/DockManager.h"
//#include "Qads/DockWidget.h"
//#include "Qads/DockAreaWidget.h"
//#include "Qads/DockAreaTitleBar.h"
//#include "Qads/DockAreaTabBar.h"
//#include "Qads/FloatingDockContainer.h"
//#include "Qads/DockComponentsFactory.h"

#include <DockManager.h>
#include <DockWidget.h>
#include <DockAreaWidget.h>
#include <DockAreaTitleBar.h>
#include <DockAreaTabBar.h>
#include <FloatingDockContainer.h>
#include <DockComponentsFactory.h>
#include <tesseract_qt/plot/plot_data.h>
#include <tesseract_qt/plot/plot_widget.h>

namespace Ui
{
class DraggableToolbar;
}

namespace tesseract_gui
{
class DraggableToolbar : public QWidget
{
  Q_OBJECT

public:
  explicit DraggableToolbar(ads::CDockWidget* parent);
  ~DraggableToolbar() override;

  QLabel* label();

  QPushButton* buttonFullscreen();

  QPushButton* buttonClose();

  QPushButton* buttonSplitHorizontal();

  QPushButton* buttonSplitVertical();

  void toggleFullscreen();

  bool isFullscreen() const { return _fullscreen_mode; }

  bool eventFilter(QObject* object, QEvent* event) override;

public Q_SLOTS:

  void on_stylesheetChanged(QString theme);

private:
  void enterEvent(QEvent*) override;
  void leaveEvent(QEvent*) override;

  ads::CDockWidget* _parent;
  std::unique_ptr<Ui::DraggableToolbar> ui;
  bool _fullscreen_mode;

  QIcon _expand_icon;
  QIcon _collapse_icon;
};

class DockWidget : public ads::CDockWidget
{
  Q_OBJECT

public:
  DockWidget(PlotDataMapRef& datamap, QWidget* parent = nullptr);

  ~DockWidget() override;

  PlotWidget* plotWidget();

  DraggableToolbar* toolBar();

public Q_SLOTS:
  DockWidget* splitHorizontal();

  DockWidget* splitVertical();

private:
  std::unique_ptr<PlotWidget> _plot_widget = nullptr;

  DraggableToolbar* _toolbar;

  PlotDataMapRef& _datamap;

Q_SIGNALS:
  void undoableChange();
};

class PlotDocker : public ads::CDockManager
{
  Q_OBJECT

public:
  PlotDocker(QString name, PlotDataMapRef& datamap, QWidget* parent = nullptr);

  QString name() const;

  void setName(QString name);

  int plotCount() const;

  PlotWidget* plotAt(int index);

  void setHorizontalLink(bool enabled);

  void zoomOut();

  void replot();

public Q_SLOTS:

  void on_stylesheetChanged(QString theme);

private:
  QString _name;

  PlotDataMapRef& _datamap;

Q_SIGNALS:

  void plotWidgetAdded(PlotWidget*);

  void undoableChange();
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_PLOT_PLOT_DOCKER_H
