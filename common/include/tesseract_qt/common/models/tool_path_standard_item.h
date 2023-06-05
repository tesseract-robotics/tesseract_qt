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
#ifndef TESSERACT_QT_COMMON_TOOL_PATH_STANDARD_ITEM_H
#define TESSERACT_QT_COMMON_TOOL_PATH_STANDARD_ITEM_H

#include <QStandardItem>
#include <tesseract_qt/common/tool_path.h>

namespace tesseract_gui
{
class ToolPathStandardItem : public QStandardItem
{
public:
  explicit ToolPathStandardItem(const ToolPath& tool_path);
  ToolPathStandardItem(const QString& text, const ToolPath& tool_path);
  ToolPathStandardItem(const QIcon& icon, const QString& text, const ToolPath& tool_path);
  int type() const override;

  const boost::uuids::uuid& getUUID() const;
  const boost::uuids::uuid& getParentUUID() const;
  ToolPath getToolPath() const;
  const std::string& getWorkingFrame() const;
  tesseract_common::Toolpath getCommonToolPath() const;

private:
  void ctor(const ToolPath& tool_path);
  boost::uuids::uuid uuid_{};
  boost::uuids::uuid parent_uuid_{};
  std::string description_;
  std::string working_frame_;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_COMMON_TOOL_PATH_STANDARD_ITEM_H
