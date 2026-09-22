/*
 * @copyright Copyright (C) 2026 Levi Armstrong <levi.armstrong@gmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 */
#ifndef TESSERACT_QT_COMMON_PROPERTY_TREE_ITEM_DELEGATE_H
#define TESSERACT_QT_COMMON_PROPERTY_TREE_ITEM_DELEGATE_H

#include <QStyledItemDelegate>

namespace tesseract::gui
{
/** @brief Creates schema-aware editors for values exposed by PropertyTreeModel. */
class PropertyTreeItemDelegate : public QStyledItemDelegate
{
public:
  explicit PropertyTreeItemDelegate(QObject* parent = nullptr);
  ~PropertyTreeItemDelegate() override;

  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
  void updateEditorGeometry(QWidget* editor,
                            const QStyleOptionViewItem& option,
                            const QModelIndex& index) const override;
};
}  // namespace tesseract::gui

#endif  // TESSERACT_QT_COMMON_PROPERTY_TREE_ITEM_DELEGATE_H
