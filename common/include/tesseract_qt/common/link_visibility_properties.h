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
#ifndef TESSERACT_QT_COMMON_LINK_VISIBILITY_PROPERTIES_H
#define TESSERACT_QT_COMMON_LINK_VISIBILITY_PROPERTIES_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <unordered_map>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

namespace tesseract_gui
{
/** @brief Stores the link visibility properties */
struct LinkVisibilityProperties
{
  bool link{ true };
  bool visual{ true };
  bool collision{ false };
  bool wirebox{ false };
  bool axis{ false };
};

using LinkVisibilityPropertiesMap = std::unordered_map<std::string, LinkVisibilityProperties>;

}  // namespace tesseract_gui
#endif  // TESSERACT_QT_COMMON_LINK_VISIBILITY_PROPERTIES_H
