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
#ifndef TESSERACT_QT_COMMON_ALLOWED_COLLISION_MATRIX_EVENTS_H
#define TESSERACT_QT_COMMON_ALLOWED_COLLISION_MATRIX_EVENTS_H

#include <memory>
#include <vector>
#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>

namespace tesseract_common
{
class AllowedCollisionMatrix;
}

namespace tesseract_gui::events
{
class AllowedCollisionMatrixClear : public ComponentEvent
{
public:
  AllowedCollisionMatrixClear(std::shared_ptr<const ComponentInfo> component_info);
  ~AllowedCollisionMatrixClear() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::ACM_CLEAR);
};

class AllowedCollisionMatrixSet : public ComponentEvent
{
public:
  AllowedCollisionMatrixSet(std::shared_ptr<const ComponentInfo> component_info,
                            const tesseract_common::AllowedCollisionMatrix& acm);
  ~AllowedCollisionMatrixSet() override;

  const tesseract_common::AllowedCollisionMatrix& getACM() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::ACM_SET);

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;
};

class AllowedCollisionMatrixAdd : public ComponentEvent
{
public:
  AllowedCollisionMatrixAdd(std::shared_ptr<const ComponentInfo> component_info,
                            const std::vector<std::array<std::string, 3>>& entries);
  ~AllowedCollisionMatrixAdd() override;

  const std::vector<std::array<std::string, 3>>& getEntries() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::ACM_ADD);

private:
  std::vector<std::array<std::string, 3>> entries_;
};

class AllowedCollisionMatrixRemove : public ComponentEvent
{
public:
  AllowedCollisionMatrixRemove(std::shared_ptr<const ComponentInfo> component_info,
                               const std::vector<std::array<std::string, 2>>& entries);
  ~AllowedCollisionMatrixRemove() override;

  const std::vector<std::array<std::string, 2>>& getEntries() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::ACM_REMOVE);

private:
  std::vector<std::array<std::string, 2>> entries_;
};

class AllowedCollisionMatrixRemoveLink : public ComponentEvent
{
public:
  AllowedCollisionMatrixRemoveLink(std::shared_ptr<const ComponentInfo> component_info,
                                   const std::vector<std::string>& link_names);
  ~AllowedCollisionMatrixRemoveLink() override;

  const std::vector<std::string>& getLinkNames() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::ACM_REMOVE_LINK);

private:
  std::vector<std::string> link_names_;
};

class AllowedCollisionMatrixGenerate : public ComponentEvent
{
public:
  AllowedCollisionMatrixGenerate(std::shared_ptr<const ComponentInfo> component_info, long resolution);
  ~AllowedCollisionMatrixGenerate() override;

  long getResolution() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::ACM_GENERATE);

private:
  long resolution_;
};

}  // namespace tesseract_gui::events
#endif  // TESSERACT_QT_COMMON_ALLOWED_COLLISION_MATRIX_EVENTS_H
