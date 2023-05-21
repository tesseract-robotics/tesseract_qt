/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2023 Levi Armstrong <levi.armstrong@gmail.com>
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
#ifndef TESSERACT_QT_COMMON_MANIPULATION_EVENTS_H
#define TESSERACT_QT_COMMON_MANIPULATION_EVENTS_H

#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>

#include <unordered_map>
#include <memory>

#include <QEvent>
#include <QString>

namespace tesseract_gui::events
{
class ManipulationChanged : public ComponentEvent
{
public:
  ManipulationChanged(std::shared_ptr<const ComponentInfo> component_info,
                      std::string state_name,
                      std::unordered_map<std::string, std::shared_ptr<const ComponentInfo>> state_component_infos);
  ManipulationChanged(const ManipulationChanged& other);
  ~ManipulationChanged() override;

  const std::string& getStateName() const;
  const std::unordered_map<std::string, std::shared_ptr<const ComponentInfo>>& getStateComponentInfos() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::MANIPULATION_CHANGED);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

}  // namespace tesseract_gui::events

#endif  // TESSERACT_QT_COMMON_MANIPULATION_EVENTS_H
