/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2011 SCHUTZ Sacha
 * Copyright (c) 2023 Levi Armstrong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <tesseract_qt/common/models/yaml_model.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_common/utils.h>

#include <QDebug>
#include <QFile>
#include <QFont>

namespace tesseract_gui
{
bool isBool(std::string s)
{
  tesseract_common::trim(s);
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
  return (s == "true" || s == "false");
}

QYamlTreeItem::QYamlTreeItem(QYamlTreeItem* parent) { parent_ = parent; }

QYamlTreeItem::~QYamlTreeItem() { qDeleteAll(childs_); }

void QYamlTreeItem::appendChild(QYamlTreeItem* item) { childs_.append(item); }

QYamlTreeItem* QYamlTreeItem::child(int row) { return childs_.value(row); }

QYamlTreeItem* QYamlTreeItem::parent() { return parent_; }

int QYamlTreeItem::childCount() const { return childs_.count(); }

int QYamlTreeItem::row() const
{
  if (parent_)
    return parent_->childs_.indexOf(const_cast<QYamlTreeItem*>(this));

  return 0;
}

void QYamlTreeItem::setKey(const QString& key) { key_ = key; }

void QYamlTreeItem::setValue(const QVariant& value) { value_ = value; }

void QYamlTreeItem::setType(YAML::NodeType::value node_type, YAML::ScalarType::value scalar_type)
{
  node_type_ = node_type;
  scalar_type_ = scalar_type;
}

QString QYamlTreeItem::key() const { return key_; }

QVariant QYamlTreeItem::value() const { return value_; }

YAML::NodeType::value QYamlTreeItem::nodeType() const { return node_type_; }
YAML::ScalarType::value QYamlTreeItem::scalarType() const { return scalar_type_; }

QYamlTreeItem* QYamlTreeItem::load(const YAML::Node& node, QYamlTreeItem* parent)
{
  auto* rootItem = new QYamlTreeItem(parent);
  rootItem->setKey("root");

  if (node.IsMap())
  {
    // Get all childs
    for (const auto& pair : node)
    {
      std::string key = pair.first.as<std::string>();
      QYamlTreeItem* child = load(pair.second, rootItem);
      child->setKey(QString::fromStdString(key));
      rootItem->appendChild(child);
    }
    rootItem->setType(node.Type());
  }
  else if (node.IsSequence())
  {
    // Get all childs
    int index = 0;
    for (const auto& v : node)
    {
      QYamlTreeItem* child = load(v, rootItem);
      child->setKey(QString("[%1]").arg(index));
      rootItem->appendChild(child);
      ++index;
    }
    rootItem->setType(node.Type());
  }
  else
  {
    auto val_str = node.as<std::string>();

    if (node.Tag() != "!")
    {
      try
      {
        if (isBool(val_str))
        {
          auto v = node.as<bool>();
          rootItem->setValue(QVariant(v));
          rootItem->setType(node.Type(), YAML::ScalarType::Bool);
          return rootItem;
        }

        if (tesseract_common::isNumeric(val_str))
        {
          if (val_str.find(".") == std::string::npos)
          {
            auto v = node.as<int>();
            rootItem->setValue(QVariant(v));
            rootItem->setType(node.Type(), YAML::ScalarType::Int);
            return rootItem;
          }

          auto v = node.as<double>();
          rootItem->setValue(QVariant(v));
          rootItem->setType(node.Type(), YAML::ScalarType::Double);
          return rootItem;
        }
      }
      catch (const YAML::BadConversion& e)
      {
      }
    }

    rootItem->setValue(QVariant(QString::fromStdString(val_str)));
    rootItem->setType(node.Type(), YAML::ScalarType::String);
  }

  return rootItem;
}

QYamlModel::QYamlModel(QObject* parent) : QAbstractItemModel(parent), root_item_{ new QYamlTreeItem }
{
  headers_.append("key");
  headers_.append("value");
}

QYamlModel::QYamlModel(const QString& fileName, QObject* parent)
  : QAbstractItemModel(parent), root_item_{ new QYamlTreeItem }
{
  headers_.append("key");
  headers_.append("value");
  loadFile(fileName);
}

QYamlModel::QYamlModel(const YAML::Node& node, QObject* parent)
  : QAbstractItemModel(parent), root_item_{ new QYamlTreeItem }
{
  headers_.append("key");
  headers_.append("value");
  load(node);
}

QYamlModel::~QYamlModel() { delete root_item_; }

bool QYamlModel::loadFile(const QString& fileName)
{
  try
  {
    YAML::Node n = YAML::LoadFile(fileName.toStdString());
    return load(n);
  }
  catch (const YAML::BadConversion& e)
  {
    qDebug() << Q_FUNC_INFO << "cannot load yaml";
    return false;
  }
}

bool QYamlModel::load(const QString& str)
{
  try
  {
    YAML::Node n = YAML::Load(str.toStdString());
    return load(n);
  }
  catch (const YAML::BadConversion& e)
  {
    qDebug() << Q_FUNC_INFO << "cannot load yaml";
    return false;
  }
}

bool QYamlModel::load(const YAML::Node& node)
{
  if (!node.IsNull())
  {
    beginResetModel();
    delete root_item_;
    root_item_ = QYamlTreeItem::load(node);
    endResetModel();
    return true;
  }

  qDebug() << Q_FUNC_INFO << "cannot load yaml";
  return false;
}

QVariant QYamlModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return {};

  auto* item = static_cast<QYamlTreeItem*>(index.internalPointer());

  if (role == Qt::DisplayRole)
  {
    if (index.column() == 0)
      return QString("%1").arg(item->key());

    if (index.column() == 1)
      return item->value();
  }
  else if (role == Qt::EditRole)
  {
    if (index.column() == 1)
      return item->value();
  }
  else if (role == Qt::DecorationRole && show_icons_)
  {
    if (index.column() == 0)
    {
      if (item->nodeType() == YAML::NodeType::Scalar)
      {
        if (item->scalarType() == YAML::ScalarType::Double || item->scalarType() == YAML::ScalarType::Int)
          return icons::getNumericIcon();

        if (item->scalarType() == YAML::ScalarType::Bool)
          return icons::getTrueFalseIcon();

        return icons::getTextIcon();
      }

      if (item->nodeType() == YAML::NodeType::Map)
        return icons::getKeyValueIcon();

      if (item->nodeType() == YAML::NodeType::Sequence)
        return icons::getArrayIcon();
    }
  }

  return {};
}

bool QYamlModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  int col = index.column();
  if (Qt::EditRole == role)
  {
    if (col == 1)
    {
      auto* item = static_cast<QYamlTreeItem*>(index.internalPointer());
      item->setValue(value);
      emit dataChanged(index, index, { Qt::EditRole });
      return true;
    }
  }

  return false;
}

QVariant QYamlModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return {};

  if (orientation == Qt::Horizontal)
    return headers_.value(section);

  return {};
}

QModelIndex QYamlModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!hasIndex(row, column, parent))
    return {};

  QYamlTreeItem* parentItem;

  if (!parent.isValid())
    parentItem = root_item_;
  else
    parentItem = static_cast<QYamlTreeItem*>(parent.internalPointer());

  QYamlTreeItem* childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);

  return {};
}

QModelIndex QYamlModel::parent(const QModelIndex& index) const
{
  if (!index.isValid())
    return {};

  auto* childItem = static_cast<QYamlTreeItem*>(index.internalPointer());
  auto* parentItem = childItem->parent();

  if (parentItem == root_item_)
    return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}

int QYamlModel::rowCount(const QModelIndex& parent) const
{
  QYamlTreeItem* parentItem;
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid())
    parentItem = root_item_;
  else
    parentItem = static_cast<QYamlTreeItem*>(parent.internalPointer());

  return parentItem->childCount();
}

int QYamlModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent)
  return 2;
}

Qt::ItemFlags QYamlModel::flags(const QModelIndex& index) const
{
  int col = index.column();
  auto* item = static_cast<QYamlTreeItem*>(index.internalPointer());

  if ((col == 1) && (item->nodeType() == YAML::NodeType::Scalar))
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);

  return QAbstractItemModel::flags(index);
}

YAML::Node QYamlModel::yaml() { return genYaml(root_item_); }

void QYamlModel::showIcons(bool enable) { show_icons_ = enable; }

YAML::Node QYamlModel::genYaml(QYamlTreeItem* item) const
{
  auto node_type = item->nodeType();
  int nchild = item->childCount();

  if (node_type == YAML::NodeType::Map)
  {
    YAML::Node m;
    for (int i = 0; i < nchild; ++i)
    {
      auto ch = item->child(i);
      auto key = ch->key();
      m[key.toStdString()] = genYaml(ch);
    }
    return m;
  }

  if (node_type == YAML::NodeType::Sequence)
  {
    YAML::Node s;
    for (int i = 0; i < nchild; ++i)
    {
      auto ch = item->child(i);
      s.push_back(genYaml(ch));
    }
    return s;
  }

  YAML::Node va;
  switch (item->value().type())
  {
    case QVariant::Bool:
    {
      va = item->value().toBool();
      break;
    }
    default:
      va = item->value().toString().toStdString();
      break;
  }
  return va;
}

}  // namespace tesseract_gui
