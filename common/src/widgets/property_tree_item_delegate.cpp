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
#include <tesseract_qt/common/widgets/property_tree_item_delegate.h>

#include <tesseract_qt/common/models/property_tree_model.h>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSpinBox>

#include <limits>

namespace tesseract::gui
{
namespace
{
constexpr int VALUE_COLUMN{ 1 };

QString typeName(std::string_view type) { return QString::fromUtf8(type.data(), static_cast<int>(type.size())); }
}  // namespace

PropertyTreeItemDelegate::PropertyTreeItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

PropertyTreeItemDelegate::~PropertyTreeItemDelegate() = default;

QWidget* PropertyTreeItemDelegate::createEditor(QWidget* parent,
                                                const QStyleOptionViewItem& option,
                                                const QModelIndex& index) const
{
  if (index.column() != VALUE_COLUMN)
    return QStyledItemDelegate::createEditor(parent, option, index);

  const QStringList enum_values = index.data(PropertyTreeModel::EnumValuesRole).toStringList();
  if (!enum_values.empty())
  {
    auto* editor = new QComboBox(parent);
    editor->addItems(enum_values);
    return editor;
  }

  const QString type = index.data(PropertyTreeModel::TypeRole).toString();
  using namespace tesseract::common;
  if (type == typeName(property_type::BOOL))
    return nullptr;

  if (type == typeName(property_type::INT32))
  {
    auto* editor = new QSpinBox(parent);
    editor->setMinimum(index.data(PropertyTreeModel::MinimumRole).isValid() ?
                           index.data(PropertyTreeModel::MinimumRole).toInt() :
                           std::numeric_limits<int>::lowest());
    editor->setMaximum(index.data(PropertyTreeModel::MaximumRole).isValid() ?
                           index.data(PropertyTreeModel::MaximumRole).toInt() :
                           std::numeric_limits<int>::max());
    return editor;
  }

  if (type == typeName(property_type::UINT32) || type == typeName(property_type::INT64) ||
      type == typeName(property_type::UINT64))
  {
    auto* editor = new QLineEdit(parent);
    const QString expression = (type == typeName(property_type::UINT32) || type == typeName(property_type::UINT64)) ?
                                   QStringLiteral("[0-9]+") :
                                   QStringLiteral("-?[0-9]+");
    editor->setValidator(new QRegularExpressionValidator(QRegularExpression(expression), editor));
    return editor;
  }

  if (type == typeName(property_type::FLOAT32) || type == typeName(property_type::FLOAT64))
  {
    auto* editor = new QDoubleSpinBox(parent);
    editor->setDecimals(12);
    editor->setMinimum(index.data(PropertyTreeModel::MinimumRole).isValid() ?
                           index.data(PropertyTreeModel::MinimumRole).toDouble() :
                           -std::numeric_limits<double>::max());
    editor->setMaximum(index.data(PropertyTreeModel::MaximumRole).isValid() ?
                           index.data(PropertyTreeModel::MaximumRole).toDouble() :
                           std::numeric_limits<double>::max());
    return editor;
  }

  if (type == typeName(property_type::STRING) || type == typeName(property_type::CHAR))
  {
    auto* editor = new QLineEdit(parent);
    editor->setPlaceholderText(index.data(PropertyTreeModel::PlaceholderRole).toString());
    if (index.data(PropertyTreeModel::MaximumLengthRole).isValid())
      editor->setMaxLength(index.data(PropertyTreeModel::MaximumLengthRole).toInt());
    return editor;
  }

  auto* editor = new QPlainTextEdit(parent);
  editor->setTabChangesFocus(true);
  editor->setMinimumHeight(80);
  return editor;
}

void PropertyTreeItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  const QVariant value = index.data(Qt::EditRole);
  if (auto* combo_box = qobject_cast<QComboBox*>(editor))
  {
    const int item_index = combo_box->findText(value.toString());
    combo_box->setCurrentIndex(item_index >= 0 ? item_index : 0);
  }
  else if (auto* spin_box = qobject_cast<QSpinBox*>(editor))
  {
    spin_box->setValue(value.toInt());
  }
  else if (auto* double_spin_box = qobject_cast<QDoubleSpinBox*>(editor))
  {
    double_spin_box->setValue(value.toDouble());
  }
  else if (auto* line_edit = qobject_cast<QLineEdit*>(editor))
  {
    line_edit->setText(value.toString());
  }
  else if (auto* yaml_edit = qobject_cast<QPlainTextEdit*>(editor))
  {
    yaml_edit->setPlainText(value.toString());
  }
  else
  {
    QStyledItemDelegate::setEditorData(editor, index);
  }
}

void PropertyTreeItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  if (auto* combo_box = qobject_cast<QComboBox*>(editor))
    model->setData(index, combo_box->currentText());
  else if (auto* spin_box = qobject_cast<QSpinBox*>(editor))
    model->setData(index, spin_box->value());
  else if (auto* double_spin_box = qobject_cast<QDoubleSpinBox*>(editor))
    model->setData(index, double_spin_box->value());
  else if (auto* line_edit = qobject_cast<QLineEdit*>(editor))
    model->setData(index, line_edit->text());
  else if (auto* yaml_edit = qobject_cast<QPlainTextEdit*>(editor))
    model->setData(index, yaml_edit->toPlainText());
  else
    QStyledItemDelegate::setModelData(editor, model, index);
}

void PropertyTreeItemDelegate::updateEditorGeometry(QWidget* editor,
                                                    const QStyleOptionViewItem& option,
                                                    const QModelIndex& /*index*/) const
{
  editor->setGeometry(option.rect);
}
}  // namespace tesseract::gui
