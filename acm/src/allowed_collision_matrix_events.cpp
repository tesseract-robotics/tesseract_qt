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
#include <tesseract_qt/acm/allowed_collision_matrix_events.h>
#include <tesseract_common/allowed_collision_matrix.h>

namespace tesseract_gui::events
{
AllowedCollisionMatrixClear::AllowedCollisionMatrixClear(const std::string& scene_name) : SceneEvent(scene_name, kType)
{
}

AllowedCollisionMatrixClear::~AllowedCollisionMatrixClear() = default;

//////////////////////////////////////////

struct AllowedCollisionMatrixSet::Implementation
{
  tesseract_common::AllowedCollisionMatrix acm;
};

AllowedCollisionMatrixSet::AllowedCollisionMatrixSet(const std::string& scene_name,
                                                     const tesseract_common::AllowedCollisionMatrix& acm)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->acm = acm;
}

AllowedCollisionMatrixSet::~AllowedCollisionMatrixSet() = default;

const tesseract_common::AllowedCollisionMatrix& AllowedCollisionMatrixSet::getACM() const { return data_->acm; }

//////////////////////////////////////////

AllowedCollisionMatrixAdd::AllowedCollisionMatrixAdd(const std::string& scene_name,
                                                     const std::vector<std::array<std::string, 3>>& entries)
  : SceneEvent(scene_name, kType), entries_(entries)
{
}

AllowedCollisionMatrixAdd::~AllowedCollisionMatrixAdd() = default;

const std::vector<std::array<std::string, 3>>& AllowedCollisionMatrixAdd::getEntries() const { return entries_; }

//////////////////////////////////////////

AllowedCollisionMatrixRemove::AllowedCollisionMatrixRemove(const std::string& scene_name,
                                                           const std::vector<std::array<std::string, 2>>& entries)
  : SceneEvent(scene_name, kType), entries_(entries)
{
}

AllowedCollisionMatrixRemove::~AllowedCollisionMatrixRemove() = default;

const std::vector<std::array<std::string, 2>>& AllowedCollisionMatrixRemove::getEntries() const { return entries_; }

//////////////////////////////////////////

AllowedCollisionMatrixRemoveLink::AllowedCollisionMatrixRemoveLink(const std::string& scene_name,
                                                                   const std::vector<std::string>& link_names)
  : SceneEvent(scene_name, kType), link_names_(link_names)
{
}

AllowedCollisionMatrixRemoveLink::~AllowedCollisionMatrixRemoveLink() = default;

const std::vector<std::string>& AllowedCollisionMatrixRemoveLink::getLinkNames() const { return link_names_; }

//////////////////////////////////////////

AllowedCollisionMatrixShow::AllowedCollisionMatrixShow(const std::string& scene_name,
                                                       const std::vector<std::string>& links)
  : SceneEvent(scene_name, kType), links_(links)
{
}

AllowedCollisionMatrixShow::~AllowedCollisionMatrixShow() = default;

const std::vector<std::string>& AllowedCollisionMatrixShow::getLinks() const { return links_; }

//////////////////////////////////////////

}  // namespace tesseract_gui::events
