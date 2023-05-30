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
#ifndef TESSERACT_GUI_COMMON_ENVIRONMENT_WRAPPER_H
#define TESSERACT_GUI_COMMON_ENVIRONMENT_WRAPPER_H

#include <memory>
#include <QObject>

namespace tesseract_environment
{
class Environment;
class EnvironmentMonitor;
class Event;
}  // namespace tesseract_environment

namespace tesseract_gui
{
struct ComponentInfo;
class EnvironmentWrapper : public QObject
{
  Q_OBJECT
public:
  EnvironmentWrapper(std::shared_ptr<const ComponentInfo> component_info);
  virtual ~EnvironmentWrapper();

  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

  virtual std::shared_ptr<const tesseract_environment::Environment> getEnvironment() const = 0;
  virtual std::shared_ptr<tesseract_environment::Environment> getEnvironment() = 0;

  virtual const tesseract_environment::Environment& environment() const = 0;
  virtual tesseract_environment::Environment& environment() = 0;

  /** @brief This broadcast events with associated data for models */
  void broadcast() const;

protected:
  friend class EnvironmentManager;

  std::shared_ptr<const ComponentInfo> component_info_;
  bool initialized_{ false };
  int revision_{ 0 };

  /** @brief This function is called when added to the environment manager */
  void init();
};

class DefaultEnvironmentWrapper : public EnvironmentWrapper
{
  Q_OBJECT
public:
  DefaultEnvironmentWrapper(std::shared_ptr<tesseract_environment::Environment> env);
  DefaultEnvironmentWrapper(std::shared_ptr<const ComponentInfo> component_info,
                            std::shared_ptr<tesseract_environment::Environment> env);
  ~DefaultEnvironmentWrapper() override;

  std::shared_ptr<const tesseract_environment::Environment> getEnvironment() const override;
  std::shared_ptr<tesseract_environment::Environment> getEnvironment() override;

  const tesseract_environment::Environment& environment() const override;
  tesseract_environment::Environment& environment() override;

private:
  std::shared_ptr<tesseract_environment::Environment> env_;

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};

class MonitorEnvironmentWrapper : public EnvironmentWrapper
{
  Q_OBJECT
public:
  MonitorEnvironmentWrapper(std::shared_ptr<tesseract_environment::EnvironmentMonitor> env_monitor);
  MonitorEnvironmentWrapper(std::shared_ptr<const ComponentInfo> component_info,
                            std::shared_ptr<tesseract_environment::EnvironmentMonitor> env_monitor);
  ~MonitorEnvironmentWrapper() override;

  std::shared_ptr<const tesseract_environment::Environment> getEnvironment() const override;
  std::shared_ptr<tesseract_environment::Environment> getEnvironment() override;

  const tesseract_environment::Environment& environment() const override;
  tesseract_environment::Environment& environment() override;

  std::shared_ptr<tesseract_environment::EnvironmentMonitor> getEnvironmentMonitor();
  std::shared_ptr<const tesseract_environment::EnvironmentMonitor> getEnvironmentMonitor() const;

private:
  std::shared_ptr<tesseract_environment::EnvironmentMonitor> env_monitor_;

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_GUI_COMMON_ENVIRONMENT_WRAPPER_H
