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
#ifndef TESSERACT_QT_COMMON_TOOL_PATH_EVENTS_H
#define TESSERACT_QT_COMMON_TOOL_PATH_EVENTS_H

#include <memory>
#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>

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
class ToolPathAdd : public ComponentEvent
{
public:
  ToolPathAdd(std::shared_ptr<const ComponentInfo> component_info, const tesseract_gui::ToolPath& tool_path);
  ToolPathAdd(const ToolPathAdd& other);
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
class ToolPathRemove : public ComponentEvent
{
public:
  ToolPathRemove(std::shared_ptr<const ComponentInfo> component_info, const boost::uuids::uuid& uuid);
  ToolPathRemove(const ToolPathRemove& other);
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
class ToolPathRemoveAll : public ComponentEvent
{
public:
  ToolPathRemoveAll(std::shared_ptr<const ComponentInfo> component_info);
  ToolPathRemoveAll(const ToolPathRemoveAll& other);
  ~ToolPathRemoveAll() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_REMOVE_ALL);
};

/** @brief Event called to remove selected a tool paths for a given scene */
class ToolPathRemoveSelected : public ComponentEvent
{
public:
  ToolPathRemoveSelected(std::shared_ptr<const ComponentInfo> component_info);
  ToolPathRemoveSelected(const ToolPathRemoveSelected& other);
  ~ToolPathRemoveSelected() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_REMOVE_SELECTED);
};

/** @brief Event called to hide a tool path */
class ToolPathHide : public ComponentEvent
{
public:
  ToolPathHide(std::shared_ptr<const ComponentInfo> component_info, const boost::uuids::uuid& uuid);
  ToolPathHide(std::shared_ptr<const ComponentInfo> component_info,
               const boost::uuids::uuid& uuid,
               const boost::uuids::uuid& child_uuid);
  ToolPathHide(const ToolPathHide& other);
  ~ToolPathHide() override;

  const boost::uuids::uuid& getUUID() const;
  const boost::uuids::uuid& getChildUUID() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_HIDE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to hide all a tool paths for a given scene */
class ToolPathHideAll : public ComponentEvent
{
public:
  ToolPathHideAll(std::shared_ptr<const ComponentInfo> component_info);
  ToolPathHideAll(const ToolPathHideAll& other);
  ~ToolPathHideAll() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_HIDE_ALL);
};

/** @brief Event called to show a tool path */
class ToolPathShow : public ComponentEvent
{
public:
  ToolPathShow(std::shared_ptr<const ComponentInfo> component_info, const boost::uuids::uuid& uuid);
  ToolPathShow(std::shared_ptr<const ComponentInfo> component_info,
               const boost::uuids::uuid& uuid,
               const boost::uuids::uuid& child_uuid);
  ToolPathShow(const ToolPathShow& other);
  ~ToolPathShow() override;

  const boost::uuids::uuid& getUUID() const;
  const boost::uuids::uuid& getChildUUID() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_SHOW);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to show all a tool paths for a given scene */
class ToolPathShowAll : public ComponentEvent
{
public:
  ToolPathShowAll(std::shared_ptr<const ComponentInfo> component_info);
  ToolPathShowAll(const ToolPathShowAll& other);
  ~ToolPathShowAll() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_SHOW_ALL);
};

/** @brief Event called to open a tool path for a given scene */
class ToolPathOpen : public ComponentEvent
{
public:
  ToolPathOpen(std::shared_ptr<const ComponentInfo> component_info);
  ToolPathOpen(const ToolPathOpen& other);
  ~ToolPathOpen() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_OPEN);
};

/** @brief Event called to save a tool path for a given scene */
class ToolPathSave : public ComponentEvent
{
public:
  ToolPathSave(std::shared_ptr<const ComponentInfo> component_info);
  ToolPathSave(const ToolPathSave& other);
  ~ToolPathSave() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::TOOL_PATH_SAVE);
};

}  // namespace tesseract_gui::events

#endif  // TESSERACT_QT_COMMON_TOOL_PATH_EVENTS_H
