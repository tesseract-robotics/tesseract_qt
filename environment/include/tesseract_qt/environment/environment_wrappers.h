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
#ifndef TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_WRAPPERS_H
#define TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_WRAPPERS_H

#include <tesseract_qt/common/environment_wrapper.h>

namespace tesseract_environment
{
class EnvironmentMonitor;
class Event;
}  // namespace tesseract_environment

namespace tesseract_gui
{
class DefaultEnvironmentWrapper : public EnvironmentWrapper
{
  Q_OBJECT
public:
  DefaultEnvironmentWrapper(std::shared_ptr<tesseract_environment::Environment> env);
  DefaultEnvironmentWrapper(ComponentInfo component_info, std::shared_ptr<tesseract_environment::Environment> env);
  ~DefaultEnvironmentWrapper() override;

  std::shared_ptr<const tesseract_environment::Environment> getEnvironment() const override;

private:
  std::shared_ptr<tesseract_environment::Environment> env_;

  void tesseractEventFilter(const tesseract_environment::Event& event);

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};

class MonitorEnvironmentWrapper : public EnvironmentWrapper
{
  Q_OBJECT
public:
  MonitorEnvironmentWrapper(std::shared_ptr<tesseract_environment::EnvironmentMonitor> env_monitor);
  MonitorEnvironmentWrapper(ComponentInfo component_info,
                            std::shared_ptr<tesseract_environment::EnvironmentMonitor> env_monitor);
  ~MonitorEnvironmentWrapper() override;

  std::shared_ptr<const tesseract_environment::Environment> getEnvironment() const override;

private:
  std::shared_ptr<tesseract_environment::EnvironmentMonitor> env_monitor_;

  void tesseractEventFilter(const tesseract_environment::Event& event);

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_WRAPPERS_H
