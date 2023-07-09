/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2011 SCHUTZ Sacha
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
#ifndef TESSERACT_QT_COMMON_YAML_MODEL_H
#define TESSERACT_QT_COMMON_YAML_MODEL_H

#include <QAbstractItemModel>
#include <yaml-cpp/yaml.h>

class QIODevice;

namespace YAML
{
struct ScalarType
{
  enum value
  {
    Undefined,
    Null,
    Bool,
    Double,
    Int,
    String
  };
};
}  // namespace YAML

namespace tesseract_gui
{
class QYamlTreeItem
{
public:
  QYamlTreeItem(QYamlTreeItem* parent = nullptr);
  ~QYamlTreeItem();
  void appendChild(QYamlTreeItem* item);
  QYamlTreeItem* child(int row);
  QYamlTreeItem* parent();
  int childCount() const;
  int row() const;
  void setKey(const QString& key);
  void setValue(const QVariant& value);
  void setType(YAML::NodeType::value node_type, YAML::ScalarType::value scalar_type = YAML::ScalarType::Null);
  QString key() const;
  QVariant value() const;
  YAML::NodeType::value nodeType() const;
  YAML::ScalarType::value scalarType() const;

  static QYamlTreeItem* load(const YAML::Node& value, QYamlTreeItem* parent = nullptr);

private:
  QString key_;
  QVariant value_;
  YAML::NodeType::value node_type_{ YAML::NodeType::Null };
  YAML::ScalarType::value scalar_type_{ YAML::ScalarType::Null };
  QList<QYamlTreeItem*> childs_;
  QYamlTreeItem* parent_ = nullptr;
};

//---------------------------------------------------

class QYamlModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  explicit QYamlModel(QObject* parent = nullptr);
  QYamlModel(const QString& fileName, QObject* parent = nullptr);
  QYamlModel(const YAML::Node& node, QObject* parent = nullptr);
  ~QYamlModel();
  bool loadFile(const QString& fileName);
  bool load(const QString& str);
  bool load(const YAML::Node& node);
  QVariant data(const QModelIndex& index, int role) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  YAML::Node yaml();
  void showIcons(bool enable);

private:
  YAML::Node genYaml(QYamlTreeItem*) const;
  QYamlTreeItem* root_item_ = nullptr;
  QStringList headers_;
  bool show_icons_{ false };
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_COMMON_YAML_MODEL_H
