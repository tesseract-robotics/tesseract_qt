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
#include <tesseract_qt/common/events/allowed_collision_matrix_events.h>

#include <tesseract_common/allowed_collision_matrix.h>

namespace tesseract::gui::events
{
AllowedCollisionMatrixClear::AllowedCollisionMatrixClear(std::shared_ptr<const ComponentInfo> component_info)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::ACM_CLEAR))
{
}

AllowedCollisionMatrixClear::~AllowedCollisionMatrixClear() = default;

//////////////////////////////////////////

struct AllowedCollisionMatrixSet::Implementation
{
  tesseract::common::AllowedCollisionMatrix acm;
};

AllowedCollisionMatrixSet::AllowedCollisionMatrixSet(std::shared_ptr<const ComponentInfo> component_info,
                                                     const tesseract::common::AllowedCollisionMatrix& acm)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::ACM_SET))
  , data_(std::make_unique<Implementation>())
{
  data_->acm = acm;
}

AllowedCollisionMatrixSet::~AllowedCollisionMatrixSet() = default;

const tesseract::common::AllowedCollisionMatrix& AllowedCollisionMatrixSet::getACM() const { return data_->acm; }

//////////////////////////////////////////

AllowedCollisionMatrixAdd::AllowedCollisionMatrixAdd(std::shared_ptr<const ComponentInfo> component_info,
                                                     const std::vector<std::array<std::string, 3>>& entries)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::ACM_ADD)), entries_(entries)
{
}

AllowedCollisionMatrixAdd::~AllowedCollisionMatrixAdd() = default;

const std::vector<std::array<std::string, 3>>& AllowedCollisionMatrixAdd::getEntries() const { return entries_; }

//////////////////////////////////////////

AllowedCollisionMatrixRemove::AllowedCollisionMatrixRemove(std::shared_ptr<const ComponentInfo> component_info,
                                                           const std::vector<std::array<std::string, 2>>& entries)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::ACM_REMOVE)), entries_(entries)
{
}

AllowedCollisionMatrixRemove::~AllowedCollisionMatrixRemove() = default;

const std::vector<std::array<std::string, 2>>& AllowedCollisionMatrixRemove::getEntries() const { return entries_; }

//////////////////////////////////////////

AllowedCollisionMatrixRemoveLink::AllowedCollisionMatrixRemoveLink(std::shared_ptr<const ComponentInfo> component_info,
                                                                   const std::vector<std::string>& link_names)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::ACM_REMOVE_LINK)), link_names_(link_names)
{
}

AllowedCollisionMatrixRemoveLink::~AllowedCollisionMatrixRemoveLink() = default;

const std::vector<std::string>& AllowedCollisionMatrixRemoveLink::getLinkNames() const { return link_names_; }

//////////////////////////////////////////

AllowedCollisionMatrixGenerate::AllowedCollisionMatrixGenerate(std::shared_ptr<const ComponentInfo> component_info,
                                                               long resolution)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::ACM_GENERATE)), resolution_(resolution)
{
}

AllowedCollisionMatrixGenerate::~AllowedCollisionMatrixGenerate() = default;

long AllowedCollisionMatrixGenerate::getResolution() const { return resolution_; }
}  // namespace tesseract::gui::events
