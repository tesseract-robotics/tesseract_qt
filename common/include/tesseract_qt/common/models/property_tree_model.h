/*
 * @copyright Copyright (c) 2026, Southwest Research Institute
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TESSERACT_QT_COMMON_PROPERTY_TREE_MODEL_H
#define TESSERACT_QT_COMMON_PROPERTY_TREE_MODEL_H

#ifndef Q_MOC_RUN
#include <tesseract_qt/common/plugin_schema_resolver.h>
#include <tesseract/common/property_tree.h>
#include <yaml-cpp/yaml.h>

#include <QAbstractItemModel>
#include <QStringList>
#endif

#include <memory>

namespace tesseract::gui
{
/** @brief Editable Qt model backed by a PropertyTree schema and YAML configuration draft. */
class PropertyTreeModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  /** @brief Additional model roles exposing PropertyTree schema metadata. */
  enum Role
  {
    TypeRole = Qt::UserRole + 1,
    PathRole,
    RequiredRole,
    DefaultRole,
    DocumentationRole,
    LabelRole,
    PlaceholderRole,
    GroupRole,
    ReadOnlyRole,
    MinimumRole,
    MaximumRole,
    MinimumLengthRole,
    MaximumLengthRole,
    EnumValuesRole,
    ValidationErrorsRole,
    DynamicContainerRole,
    CanAddRole,
    CanRemoveRole,
    CanRenameRole
  };
  Q_ENUM(Role)

  explicit PropertyTreeModel(QObject* parent = nullptr);
  ~PropertyTreeModel() override;

  PropertyTreeModel(const PropertyTreeModel&) = delete;
  PropertyTreeModel& operator=(const PropertyTreeModel&) = delete;
  PropertyTreeModel(PropertyTreeModel&&) = delete;
  PropertyTreeModel& operator=(PropertyTreeModel&&) = delete;

  /**
   * @brief Load a schema and optional configuration draft.
   * @param schema Schema template. The model keeps its own deep copy.
   * @param config Optional configuration merged into the schema copy.
   * @throws std::runtime_error if the configuration cannot be merged into the schema.
   */
  void load(const tesseract::common::PropertyTree& schema, const YAML::Node& config = YAML::Node());

  /** @brief Remove the current schema and draft. */
  void clear();

  /** @brief Get the current normalized YAML configuration. */
  YAML::Node yaml() const;

  /**
   * @brief Replace the current YAML draft while retaining the schema and plugin catalogs.
   * @param config New unwrapped YAML draft.
   * @param changed_path Property path associated with the replacement, if known.
   */
  void replaceYaml(const YAML::Node& config, const QString& changed_path = {});

  /** @brief Get validation errors for the current draft. */
  QStringList validationErrors() const;

  /** @brief Return true when the current draft has no validation errors. */
  bool isValid() const;

  /** @brief Revalidate the current draft, for example after registering plugin schemas. */
  void revalidate();

  /** @brief Replace the discovered plugin schema catalogs and rebuild dynamic plugin subtrees. */
  void setPluginSchemaCatalogs(std::vector<PluginSchemaCatalog> catalogs);

  /** @brief Add an entry to the map represented by parent. */
  bool addMapEntry(const QModelIndex& parent, const QString& key);

  /** @brief Append an entry to the list represented by parent. */
  bool addListEntry(const QModelIndex& parent);

  /** @brief Remove a dynamic map or list entry. */
  bool removeEntry(const QModelIndex& index);

  /** @brief Rename a dynamic map entry. */
  bool renameMapEntry(const QModelIndex& index, const QString& key);

  /** @brief Move a dynamic list entry to a new index. */
  bool moveListEntry(const QModelIndex& index, int destination);

  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;

Q_SIGNALS:
  /** @brief Emitted after an edit changes the YAML draft. */
  void configurationChanged();

  /** @brief Emitted after an edit changes the YAML draft at a specific property path. */
  void configurationChangedAt(const QString& path);

  /** @brief Emitted when validation is recomputed. */
  void validationChanged(bool valid, const QStringList& errors);

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;
};
}  // namespace tesseract::gui

#endif  // TESSERACT_QT_COMMON_PROPERTY_TREE_MODEL_H
