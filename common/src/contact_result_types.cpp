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
#include <tesseract_qt/common/contact_results_types.h>

namespace tesseract_gui
{
ContactResultMap convert(const tesseract_collision::ContactResultMap& contact_results)
{
  // Convert to tracked objects
  ContactResultMap tracked_object;
  for (const auto& contact : contact_results)
  {
    ContactResultVector crv;
    for (const auto& result : contact.second)
      crv().emplace_back(tesseract_gui::ContactResult(result));

    tracked_object[contact.first] = crv;
  }
  return tracked_object;
}

std::vector<ContactResultMap> convert(const std::vector<tesseract_collision::ContactResultMap>& contact_results)
{
  std::vector<ContactResultMap> tracked_contact_results;
  tracked_contact_results.reserve(contact_results.size());
  for (const auto& state_results : contact_results)
    tracked_contact_results.push_back(convert(state_results));

  return tracked_contact_results;
}

}  // namespace tesseract_gui
