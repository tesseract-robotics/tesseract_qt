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

#include <tesseract_qt/common/widgets/status_log_tool_bar.h>
#include <tesseract_qt/common/events/log_events.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/component_info.h>

#include <QApplication>
#include <QEvent>
#include <QMessageBox>

namespace tesseract_gui
{
struct StatusLogToolBar::Implementation
{
  QAction* clear_all{ nullptr };
  QAction* toggle_info{ nullptr };
  QAction* toggle_warn{ nullptr };
  QAction* toggle_error{ nullptr };

  void onInfoToggle(bool toggled)
  {
    if (toggled)
    {
      events::StatusLogInfoToggleOnEvent event;
      QApplication::sendEvent(qApp, &event);
    }
    else
    {
      events::StatusLogInfoToggleOffEvent event;
      QApplication::sendEvent(qApp, &event);
    }
  }

  void onWarnToggle(bool toggled)
  {
    if (toggled)
    {
      events::StatusLogWarnToggleOnEvent event;
      QApplication::sendEvent(qApp, &event);
    }
    else
    {
      events::StatusLogWarnToggleOffEvent event;
      QApplication::sendEvent(qApp, &event);
    }
  }

  void onErrorToggle(bool toggled)
  {
    if (toggled)
    {
      events::StatusLogErrorToggleOnEvent event;
      QApplication::sendEvent(qApp, &event);
    }
    else
    {
      events::StatusLogErrorToggleOffEvent event;
      QApplication::sendEvent(qApp, &event);
    }
  }
};

StatusLogToolBar::StatusLogToolBar(QWidget* parent) : QToolBar(parent), data_(std::make_unique<Implementation>())
{
  data_->clear_all = addAction(icons::getClearIcon(), "Clear All", []() {
    events::StatusLogClearEvent event;
    QApplication::sendEvent(qApp, &event);
  });
  addSeparator();
  data_->toggle_info = addAction(icons::getInfoMsgIcon(), "Toggle Info", []() {});
  data_->toggle_warn = addAction(icons::getWarnMsgIcon(), "Toggle Warn", []() {});
  data_->toggle_error = addAction(icons::getErrorMsgIcon(), "Toggle Error", []() {});

  data_->toggle_info->setCheckable(true);
  data_->toggle_warn->setCheckable(true);
  data_->toggle_error->setCheckable(true);

  connect(data_->toggle_info, &QAction::toggled, this, [this](bool state) { data_->onInfoToggle(state); });
  connect(data_->toggle_warn, &QAction::toggled, this, [this](bool state) { data_->onWarnToggle(state); });
  connect(data_->toggle_error, &QAction::toggled, this, [this](bool state) { data_->onErrorToggle(state); });
}

StatusLogToolBar::~StatusLogToolBar() = default;

}  // namespace tesseract_gui
