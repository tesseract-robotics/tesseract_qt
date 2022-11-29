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

namespace tesseract_gui
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
class ToolPathAdd : public SceneEvent
{
public:
  ToolPathAdd(const std::string& scene_name, const tesseract_gui::ToolPath& tool_path);
  ~ToolPathAdd() override;

  const tesseract_gui::ToolPath& getToolPath() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_ADD);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to remove a tool path */
class ToolPathRemove : public SceneEvent
{
public:
  ToolPathRemove(const std::string& scene_name, const boost::uuids::uuid& uuid);
  ~ToolPathRemove() override;

  const boost::uuids::uuid& getUUID() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_REMOVE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to remove all a tool paths for a given scene */
class ToolPathRemoveAll : public SceneEvent
{
public:
  ToolPathRemoveAll(const std::string& scene_name);
  ~ToolPathRemoveAll() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_REMOVE_ALL);
};

/** @brief Event called to remove selected a tool paths for a given scene */
class ToolPathRemoveSelected : public SceneEvent
{
public:
  ToolPathRemoveSelected(const std::string& scene_name);
  ~ToolPathRemoveSelected() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_REMOVE_SELECTED);
};

/** @brief Event called to hide a tool path */
class ToolPathHide : public SceneEvent
{
public:
  ToolPathHide(const std::string& scene_name, const boost::uuids::uuid& uuid);
  ~ToolPathHide() override;

  const boost::uuids::uuid& getUUID() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_HIDE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to hide all a tool paths for a given scene */
class ToolPathHideAll : public SceneEvent
{
public:
  ToolPathHideAll(const std::string& scene_name);
  ~ToolPathHideAll() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_HIDE_ALL);
};

/** @brief Event called to show a tool path */
class ToolPathShow : public SceneEvent
{
public:
  ToolPathShow(const std::string& scene_name, const boost::uuids::uuid& uuid);
  ~ToolPathShow() override;

  const boost::uuids::uuid& getUUID() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_SHOW);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to show all a tool paths for a given scene */
class ToolPathShowAll : public SceneEvent
{
public:
  ToolPathShowAll(const std::string& scene_name);
  ~ToolPathShowAll() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_SHOW_ALL);
};

}  // namespace tesseract_gui::events

#endif  // TESSERACT_QT_TOOL_PATH_TOOL_PATH_EVENTS_H
