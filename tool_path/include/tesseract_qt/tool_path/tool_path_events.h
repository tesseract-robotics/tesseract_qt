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
#ifndef TESSERACT_QT_TOOL_PATH_TOOL_PATH_EVENTS_H
#define TESSERACT_QT_TOOL_PATH_TOOL_PATH_EVENTS_H

#include <QEvent>
#include <memory>
#include <tesseract_qt/common/event_type.h>

namespace tesseract_common
{
class ToolPath;
}

namespace boost::uuids
{
struct uuid;
}

namespace tesseract_gui::events
{
/** @brief Event called to add a tool path */
class AddToolPath : public QEvent
{
public:
  AddToolPath(const std::string& scene_name, const tesseract_common::ToolPath& tool_path);
  ~AddToolPath() override;

  const std::string& getSceneName() const;

  const tesseract_common::ToolPath& getToolPath() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::ADD_TOOL_PATH);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to remove a tool path */
class RemoveToolPath : public QEvent
{
public:
  RemoveToolPath(const std::string& scene_name, const boost::uuids::uuid& uuid);
  ~RemoveToolPath() override;

  const std::string& getSceneName() const;

  const boost::uuids::uuid& getUUID() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::REMOVE_TOOL_PATH);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};
}  // namespace tesseract_gui::events

#endif  // TESSERACT_QT_TOOL_PATH_TOOL_PATH_EVENTS_H
