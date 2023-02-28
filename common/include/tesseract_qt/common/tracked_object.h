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
#ifndef TESSERACT_GUI_COMMON_TRACKED_OBJECT_H
#define TESSERACT_GUI_COMMON_TRACKED_OBJECT_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <Eigen/Eigen>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

namespace tesseract_gui
{
template <typename T>
class TrackedObject
{
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  TrackedObject() : uuid_(boost::uuids::random_generator()()) {}
  TrackedObject(T tracked_object) : object_(tracked_object), uuid_(boost::uuids::random_generator()()) {}
  TrackedObject(T tracked_object, boost::uuids::uuid uuid) : object_(tracked_object), uuid_(uuid) {}

  const boost::uuids::uuid& getUUID() const { return uuid_; }

  const T& operator()() const { return object_; }
  T& operator()() { return object_; }

private:
  T object_;
  boost::uuids::uuid uuid_{};
};

}  // namespace tesseract_gui

#endif  // TESSERACT_GUI_COMMON_TRACKED_OBJECT_H
