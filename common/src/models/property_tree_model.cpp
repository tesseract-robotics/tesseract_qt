/*
 * @copyright Copyright (c) 2026, Southwest Research Institute
 * @par License Apache License 2.0
 */
#include <tesseract_qt/common/models/property_tree_model.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract/common/schema_registry.h>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QHash>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QRegularExpression>
#include <QVariant>
#include <algorithm>
#include <cstdint>
#include <numeric>
#include <set>
#include <stdexcept>
#include <utility>

namespace tesseract::gui
{
namespace
{
struct PathSegment
{
  std::string key;
  std::size_t index{ 0 };
  bool sequence{ false };
};

YAML::Node nodeAt(YAML::Node node, const std::vector<PathSegment>& path)
{
  for (const auto& segment : path)
  {
    YAML::Node next = segment.sequence ? node[segment.index] : node[segment.key];
    node.reset(next);
  }
  return node;
}

void replaceNodeAt(YAML::Node root, const std::vector<PathSegment>& path, const YAML::Node& value)
{
  if (path.empty())
  {
    root = YAML::Clone(value);
    return;
  }

  std::vector<PathSegment> parent_path(path.begin(), path.end() - 1);
  YAML::Node parent = nodeAt(root, parent_path);
  const auto& segment = path.back();
  if (segment.sequence)
    parent[segment.index] = YAML::Clone(value);
  else
    parent[segment.key] = YAML::Clone(value);
}

std::string propertyTypeName(const tesseract::common::PropertyTree& property)
{
  const auto value = property.getAttribute(tesseract::common::property_attribute::TYPE);
  return value.has_value() ? value->as<std::string>() : std::string();
}

YAML::Node defaultValue(const tesseract::common::PropertyTree& schema)
{
  using namespace tesseract::common;
  const auto configured_default = schema.getAttribute(property_attribute::DEFAULT);
  if (configured_default.has_value())
    return YAML::Clone(*configured_default);

  const auto type = propertyTypeName(schema);
  if (type == property_type::ONEOF)
  {
    PropertyTree resolved = schema;
    static_cast<void>(resolved.applyConfig(YAML::Node()));
    return defaultValue(resolved);
  }
  if (type == property_type::CONTAINER || isMapType(type).has_value())
  {
    YAML::Node value(YAML::NodeType::Map);
    for (const auto& key : schema.keys())
    {
      const auto& child = schema.at(key);
      if (child.isRequired() || child.hasAttribute(property_attribute::DEFAULT))
        value[key] = defaultValue(child);
    }
    return value;
  }
  if (isSequenceType(type).has_value())
    return YAML::Node(YAML::NodeType::Sequence);
  return {};
}

QVariant yamlToVariant(const YAML::Node& value)
{
  if (!value || value.IsNull())
    return {};
  if (!value.IsScalar())
  {
    YAML::Emitter emitter;
    emitter << value;
    return QString::fromStdString(emitter.c_str());
  }
  return QString::fromStdString(value.Scalar());
}

QVariant attributeValue(const tesseract::common::PropertyTree& property, std::string_view attribute)
{
  const auto value = property.getAttribute(attribute);
  return value.has_value() ? yamlToVariant(*value) : QVariant();
}

QString normalizedPropertyPath(QString path)
{
  path.replace(QStringLiteral(": "), QStringLiteral("."));
  path.replace(QRegularExpression(QStringLiteral(R"(\[([^\]]+)\])")), QStringLiteral(".\\1"));
  return path;
}

QString typeName(std::string_view type) { return QString::fromUtf8(type.data(), static_cast<int>(type.size())); }

QIcon glyphIcon(const QString& glyph, const QColor& color)
{
  const QString cache_key = glyph + color.name(QColor::HexArgb);
  static QHash<QString, QIcon> cache;
  const auto cached = cache.constFind(cache_key);
  if (cached != cache.cend())
    return *cached;

  QPixmap pixmap(16, 16);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(color);
  painter.drawRoundedRect(QRectF(1.0, 1.0, 14.0, 14.0), 3.0, 3.0);

  QFont font = painter.font();
  font.setBold(true);
  font.setPixelSize(glyph.size() > 1 ? 7 : 10);
  painter.setFont(font);
  painter.setPen(Qt::white);
  painter.drawText(QRect(1, 1, 14, 14), Qt::AlignCenter, glyph);

  painter.end();
  const QIcon icon(pixmap);
  cache.insert(cache_key, icon);
  return icon;
}

QIcon withErrorBadge(const QIcon& base_icon, const QString& cache_key, bool has_error)
{
  if (!has_error)
    return base_icon;

  static QHash<QString, QIcon> cache;
  const auto cached = cache.constFind(cache_key);
  if (cached != cache.cend())
    return *cached;

  QPixmap pixmap = base_icon.pixmap(16, 16);
  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.setPen(QPen(Qt::white, 1.0));
  painter.setBrush(QColor(QStringLiteral("#d32f2f")));
  painter.drawEllipse(QRectF(9.0, 9.0, 7.0, 7.0));
  painter.setPen(Qt::white);
  QFont font = painter.font();
  font.setBold(true);
  font.setPixelSize(6);
  painter.setFont(font);
  painter.drawText(QRect(9, 9, 7, 7), Qt::AlignCenter, QStringLiteral("!"));

  painter.end();
  const QIcon icon(pixmap);
  cache.insert(cache_key, icon);
  return icon;
}

bool booleanAttribute(const tesseract::common::PropertyTree& property, std::string_view attribute)
{
  const auto value = property.getAttribute(attribute);
  return value.has_value() && value->as<bool>();
}
QString propertyType(const tesseract::common::PropertyTree& property)
{
  const auto value = property.getAttribute(tesseract::common::property_attribute::TYPE);
  return value.has_value() ? QString::fromStdString(value->as<std::string>()) : QString();
}
YAML::Node variantToYaml(const QVariant& value, const QString& type)
{
  using namespace tesseract::common;
  const auto typeName = [](std::string_view name) {
    return QString::fromUtf8(name.data(), static_cast<int>(name.size()));
  };
  bool ok{ false };
  if (type == typeName(property_type::BOOL))
  {
    if (value.userType() == QMetaType::Bool)
      return YAML::Node(value.toBool());
    const auto text = value.toString().trimmed().toLower();
    if (text == "true" || text == "1")
      return YAML::Node(true);
    if (text == "false" || text == "0")
      return YAML::Node(false);
    throw std::runtime_error("Expected a boolean value");
  }
  if (type == typeName(property_type::INT32) || type == typeName(property_type::INT64))
  {
    const auto result = value.toLongLong(&ok);
    if (!ok)
      throw std::runtime_error("Expected an integer");
    return YAML::Node(static_cast<std::int64_t>(result));
  }
  if (type == typeName(property_type::UINT32) || type == typeName(property_type::UINT64))
  {
    const auto result = value.toULongLong(&ok);
    if (!ok)
      throw std::runtime_error("Expected an unsigned integer");
    return YAML::Node(static_cast<std::uint64_t>(result));
  }
  if (type == typeName(property_type::FLOAT32) || type == typeName(property_type::FLOAT64))
  {
    const auto result = value.toDouble(&ok);
    if (!ok)
      throw std::runtime_error("Expected a number");
    return YAML::Node(result);
  }
  if (type == typeName(property_type::STRING) || type == typeName(property_type::CHAR))
    return YAML::Node(value.toString().toStdString());
  return YAML::Load(value.toString().toStdString());
}
}  // namespace

struct PropertyTreeModel::Implementation
{
  enum class ItemKind : std::uint8_t
  {
    PROPERTY,
    MAP,
    MAP_ENTRY,
    LIST,
    LIST_ENTRY
  };

  struct Item
  {
    std::string key;
    std::string path;
    tesseract::common::PropertyTree* property{ nullptr };
    std::unique_ptr<tesseract::common::PropertyTree> owned_property;
    std::vector<PathSegment> yaml_path;
    ItemKind kind{ ItemKind::PROPERTY };
    Item* parent{ nullptr };
    std::vector<std::unique_ptr<Item>> children;
    int row() const
    {
      if (parent == nullptr)
        return 0;
      const auto it = std::find_if(
          parent->children.begin(), parent->children.end(), [this](const auto& child) { return child.get() == this; });
      return (it == parent->children.end()) ? 0 : static_cast<int>(std::distance(parent->children.begin(), it));
    }
  };
  tesseract::common::PropertyTree schema;
  tesseract::common::PropertyTree tree;
  YAML::Node draft;
  std::vector<PluginSchemaCatalog> catalogs;
  std::unique_ptr<Item> root;
  QStringList errors;
  const PluginSchemaCatalog* findCatalog(std::string_view property_path, std::string_view base_type) const
  {
    const PluginSchemaCatalog* nearest_catalog{ nullptr };
    for (const auto& catalog : catalogs)
    {
      if (catalog.base_type != base_type || property_path.size() < catalog.property_path.size() ||
          property_path.substr(0, catalog.property_path.size()) != catalog.property_path)
        continue;

      const bool path_boundary = property_path.size() == catalog.property_path.size() ||
                                 property_path[catalog.property_path.size()] == '.' ||
                                 property_path[catalog.property_path.size()] == '[';
      if (path_boundary &&
          (nearest_catalog == nullptr || catalog.property_path.size() > nearest_catalog->property_path.size()))
        nearest_catalog = &catalog;
    }
    if (nearest_catalog != nullptr)
      return nearest_catalog;

    const PluginSchemaCatalog* matching_catalog{ nullptr };
    for (const auto& catalog : catalogs)
    {
      if (catalog.base_type != base_type)
        continue;
      if (matching_catalog != nullptr)
        return nullptr;
      matching_catalog = &catalog;
    }
    return matching_catalog;
  }

  std::map<std::string, tesseract::common::PropertyTree> pluginSchemas(std::string_view base_type,
                                                                       std::string_view property_path) const
  {
    const PluginSchemaCatalog* catalog = findCatalog(property_path, base_type);
    if (catalog != nullptr)
      return catalog->schemas;

    std::map<std::string, tesseract::common::PropertyTree> schemas;
    auto registry = tesseract::common::SchemaRegistry::instance();
    for (const auto& type : registry->getDerivedTypes(std::string(base_type)))
    {
      if (registry->contains(type))
        schemas.emplace(type, registry->get(type));
    }
    return schemas;
  }

  static tesseract::common::PropertyTree schemaForType(std::string_view type)
  {
    using namespace tesseract::common;
    auto registry = tesseract::common::SchemaRegistry::instance();
    PropertyTree schema;
    if (registry->contains(std::string(type)))
      schema = registry->get(std::string(type));
    else
      schema.setAttribute(property_attribute::TYPE, type);
    return schema;
  }

  tesseract::common::PropertyTree pluginInfoSchema(std::string_view base_type,
                                                   const YAML::Node& value,
                                                   std::string_view property_path) const
  {
    using namespace tesseract::common;
    PropertyTree schema;
    schema.setAttribute(property_attribute::TYPE, property_type::CONTAINER);
    schema.setAttribute(property_attribute::PLUGIN_BASE_TYPE, base_type);
    schema["class"].setAttribute(property_attribute::TYPE, property_type::STRING);
    schema["class"].setAttribute(property_attribute::REQUIRED, true);

    const auto plugin_schemas = pluginSchemas(base_type, property_path);
    std::vector<std::string> aliases;
    aliases.reserve(plugin_schemas.size());
    for (const auto& entry : plugin_schemas)
      aliases.push_back(entry.first);
    if (!aliases.empty())
      schema["class"].setAttribute(property_attribute::ENUM, aliases);

    const auto selected = (value && value.IsMap() && value["class"]) ? value["class"].as<std::string>() : "";
    const auto selected_schema = plugin_schemas.find(selected);
    if (selected_schema != plugin_schemas.end())
      schema["config"] = selected_schema->second;
    if (schema.find("config") == nullptr)
    {
      schema["config"].setAttribute(property_attribute::TYPE, property_type::CONTAINER);
    }
    return schema;
  }

  tesseract::common::PropertyTree elementSchema(const tesseract::common::PropertyTree& container,
                                                std::string_view type,
                                                const YAML::Node& value,
                                                std::string_view property_path) const
  {
    if (booleanAttribute(container, tesseract::common::property_attribute::ACCEPTS_DERIVED_TYPES))
      return pluginInfoSchema(type, value, property_path);
    return schemaForType(type);
  }

  void resolveCustomType(Item& item) const
  {
    using namespace tesseract::common;
    std::set<std::string> visited_types;
    while (item.property != nullptr)
    {
      const std::string type = propertyTypeName(*item.property);
      if (type.empty() || !visited_types.insert(type).second)
        return;

      PropertyTree resolved;
      if (booleanAttribute(*item.property, property_attribute::ACCEPTS_DERIVED_TYPES) && !isMapType(type).has_value() &&
          !isSequenceType(type).has_value())
      {
        resolved = pluginInfoSchema(type, item.property->getValue(), item.path);
      }
      else
      {
        auto registry = SchemaRegistry::instance();
        if (!registry->contains(type))
          return;
        resolved = schemaForType(type);
        if (propertyTypeName(resolved).empty() && resolved.keys().empty())
          return;
      }

      for (const auto& attribute : item.property->getAttributeKeys())
      {
        if (attribute == property_attribute::TYPE || attribute == property_attribute::ACCEPTS_DERIVED_TYPES)
          continue;
        resolved.setAttribute(attribute, *item.property->getAttribute(attribute));
      }

      const YAML::Node value = YAML::Clone(item.property->getValue());
      try
      {
        static_cast<void>(resolved.applyConfig(value, true));
      }
      catch (const std::exception&)
      {
        resolved.setValue(value);
      }
      item.owned_property = std::make_unique<PropertyTree>(std::move(resolved));
      item.property = item.owned_property.get();
    }
  }

  void populate(Item& parent)
  {
    resolveCustomType(parent);
    const auto type = propertyTypeName(*parent.property);
    const auto map_type = tesseract::common::isMapType(type);
    if (map_type.has_value())
    {
      parent.kind = ItemKind::MAP;
      const YAML::Node& value = parent.property->getValue();
      if (!value || !value.IsMap())
        return;

      for (const auto& entry : value)
      {
        const auto key = entry.first.as<std::string>();
        auto child = std::make_unique<Item>();
        child->key = key;
        child->path = parent.path.empty() ? key : parent.path + "." + key;
        child->owned_property = std::make_unique<tesseract::common::PropertyTree>(
            elementSchema(*parent.property, map_type->second, entry.second, parent.path));
        try
        {
          static_cast<void>(child->owned_property->applyConfig(YAML::Clone(entry.second), true));
        }
        catch (const std::exception&)
        {
          child->owned_property->setValue(YAML::Clone(entry.second));
        }
        child->property = child->owned_property.get();
        child->parent = &parent;
        child->kind = ItemKind::MAP_ENTRY;
        child->yaml_path = parent.yaml_path;
        child->yaml_path.push_back({ key, 0, false });
        populate(*child);
        parent.children.push_back(std::move(child));
      }
      return;
    }

    const auto sequence_type = tesseract::common::isSequenceType(type);
    if (sequence_type.has_value())
    {
      parent.kind = ItemKind::LIST;
      const YAML::Node& value = parent.property->getValue();
      if (!value || !value.IsSequence())
        return;

      for (std::size_t index = 0; index < value.size(); ++index)
      {
        auto child = std::make_unique<Item>();
        child->key = std::to_string(index);
        child->path = parent.path + "[" + child->key + "]";
        child->owned_property = std::make_unique<tesseract::common::PropertyTree>(
            elementSchema(*parent.property, sequence_type->first, value[index], parent.path));
        try
        {
          static_cast<void>(child->owned_property->applyConfig(YAML::Clone(value[index]), true));
        }
        catch (const std::exception&)
        {
          child->owned_property->setValue(YAML::Clone(value[index]));
        }
        child->property = child->owned_property.get();
        child->parent = &parent;
        child->kind = ItemKind::LIST_ENTRY;
        child->yaml_path = parent.yaml_path;
        child->yaml_path.push_back({ {}, index, true });
        populate(*child);
        parent.children.push_back(std::move(child));
      }
      return;
    }

    for (const auto& key : parent.property->keys())
    {
      auto& property = parent.property->at(key);
      if (booleanAttribute(property, tesseract::common::property_attribute::HIDDEN))
        continue;
      auto child = std::make_unique<Item>();
      child->key = key;
      child->path = parent.path.empty() ? key : parent.path + "." + key;
      const auto child_type = propertyTypeName(property);
      if (booleanAttribute(property, tesseract::common::property_attribute::ACCEPTS_DERIVED_TYPES) &&
          !tesseract::common::isMapType(child_type).has_value() &&
          !tesseract::common::isSequenceType(child_type).has_value())
      {
        child->owned_property = std::make_unique<tesseract::common::PropertyTree>(
            pluginInfoSchema(child_type, property.getValue(), child->path));
        if (property.getValue() && !property.getValue().IsNull())
          static_cast<void>(child->owned_property->applyConfig(YAML::Clone(property.getValue()), true));
        child->property = child->owned_property.get();
      }
      else
      {
        child->property = &property;
      }
      child->parent = &parent;
      child->yaml_path = parent.yaml_path;
      child->yaml_path.push_back({ key, 0, false });
      populate(*child);
      parent.children.push_back(std::move(child));
    }
  }

  void rebuild()
  {
    root = std::make_unique<Item>();
    root->property = &tree;
    populate(*root);
  }

  void applyConfig(const YAML::Node& config)
  {
    draft = YAML::Clone(config);
    tree = schema;
    static_cast<void>(tree.applyConfig(YAML::Clone(draft), false));
    rebuild();
    validate();
  }
  void validate()
  {
    errors.clear();
    try
    {
      auto validation_tree = schema;
      for (const auto& error : validation_tree.applyConfig(YAML::Clone(draft), false))
        errors.push_back(QString::fromStdString(error));
    }
    catch (const std::exception& exception)
    {
      errors.push_back(QString::fromStdString(exception.what()));
    }
  }

  QStringList errorsForPath(const std::string& path) const
  {
    if (path.empty())
      return errors;
    const QString prefix = normalizedPropertyPath(QString::fromStdString(path));
    QStringList result;
    for (const auto& error : errors)
    {
      const QString normalized_error = normalizedPropertyPath(error);
      if (normalized_error == prefix || normalized_error.startsWith(prefix + "."))
        result.push_back(error);
    }
    return result;
  }
};

PropertyTreeModel::PropertyTreeModel(QObject* parent)
  : QAbstractItemModel(parent), data_(std::make_unique<Implementation>())
{
}

PropertyTreeModel::~PropertyTreeModel() = default;

void PropertyTreeModel::load(const tesseract::common::PropertyTree& schema, const YAML::Node& config)
{
  beginResetModel();
  try
  {
    data_->schema = schema;
    data_->catalogs.clear();
    data_->applyConfig((config && !config.IsNull()) ? config : YAML::Node(YAML::NodeType::Map));
  }
  catch (...)
  {
    data_->schema = {};
    data_->tree = {};
    data_->draft = {};
    data_->catalogs.clear();
    data_->root.reset();
    data_->errors.clear();
    endResetModel();
    throw;
  }
  endResetModel();
  Q_EMIT validationChanged(data_->errors.empty(), data_->errors);
}

void PropertyTreeModel::clear()
{
  beginResetModel();
  data_->schema = {};
  data_->tree = {};
  data_->draft = {};
  data_->catalogs.clear();
  data_->root.reset();
  data_->errors.clear();
  endResetModel();
  Q_EMIT validationChanged(true, {});
}

YAML::Node PropertyTreeModel::yaml() const { return data_->root == nullptr ? YAML::Node() : YAML::Clone(data_->draft); }

void PropertyTreeModel::replaceYaml(const YAML::Node& config, const QString& changed_path)
{
  if (data_->root == nullptr)
    throw std::runtime_error("PropertyTreeModel: cannot replace YAML before loading a schema");

  beginResetModel();
  try
  {
    data_->applyConfig(YAML::Clone(config));
  }
  catch (...)
  {
    endResetModel();
    throw;
  }
  endResetModel();
  Q_EMIT configurationChanged();
  Q_EMIT configurationChangedAt(changed_path);
  Q_EMIT validationChanged(data_->errors.empty(), data_->errors);
}

QStringList PropertyTreeModel::validationErrors() const { return data_->errors; }
bool PropertyTreeModel::isValid() const { return data_->errors.empty(); }

void PropertyTreeModel::revalidate()
{
  data_->validate();
  Q_EMIT validationChanged(data_->errors.empty(), data_->errors);
}
void PropertyTreeModel::setPluginSchemaCatalogs(std::vector<PluginSchemaCatalog> catalogs)
{
  const YAML::Node config = YAML::Clone(data_->draft);
  beginResetModel();
  data_->catalogs = std::move(catalogs);
  data_->applyConfig(config);
  endResetModel();
  Q_EMIT validationChanged(data_->errors.empty(), data_->errors);
}
bool PropertyTreeModel::addMapEntry(const QModelIndex& parent_index, const QString& key)
{
  if (!parent_index.isValid() || key.isEmpty())
    return false;
  auto* item = static_cast<Implementation::Item*>(parent_index.internalPointer());
  const auto map_type = tesseract::common::isMapType(propertyTypeName(*item->property));
  if (item->kind != Implementation::ItemKind::MAP || !map_type.has_value())
    return false;
  YAML::Node config = YAML::Clone(data_->draft);
  YAML::Node map = nodeAt(config, item->yaml_path);
  const auto map_key = key.toStdString();
  if (map[map_key])
    return false;
  auto entry_schema = data_->elementSchema(*item->property, map_type->second, {}, item->path);
  YAML::Node value = defaultValue(entry_schema);
  if (booleanAttribute(*item->property, tesseract::common::property_attribute::ACCEPTS_DERIVED_TYPES))
  {
    const auto plugin_schemas = data_->pluginSchemas(map_type->second, item->path);
    if (!plugin_schemas.empty())
    {
      value["class"] = plugin_schemas.begin()->first;
      value["config"] = defaultValue(plugin_schemas.begin()->second);
    }
  }
  map[map_key] = value;
  const QString changed_path = QString::fromStdString(item->path);
  beginResetModel();
  data_->applyConfig(config);
  endResetModel();
  Q_EMIT configurationChanged();
  Q_EMIT configurationChangedAt(changed_path);
  Q_EMIT validationChanged(data_->errors.empty(), data_->errors);
  return true;
}

bool PropertyTreeModel::addListEntry(const QModelIndex& parent_index)
{
  if (!parent_index.isValid())
    return false;
  auto* item = static_cast<Implementation::Item*>(parent_index.internalPointer());
  const auto sequence_type = tesseract::common::isSequenceType(propertyTypeName(*item->property));
  if (item->kind != Implementation::ItemKind::LIST || !sequence_type.has_value() || sequence_type->second != 0)
    return false;
  YAML::Node config = YAML::Clone(data_->draft);
  YAML::Node sequence = nodeAt(config, item->yaml_path);
  sequence.push_back(defaultValue(data_->elementSchema(*item->property, sequence_type->first, {}, item->path)));
  const QString changed_path = QString::fromStdString(item->path);
  beginResetModel();
  data_->applyConfig(config);
  endResetModel();
  Q_EMIT configurationChanged();
  Q_EMIT configurationChangedAt(changed_path);
  Q_EMIT validationChanged(data_->errors.empty(), data_->errors);
  return true;
}

bool PropertyTreeModel::removeEntry(const QModelIndex& model_index)
{
  if (!model_index.isValid())
    return false;
  auto* item = static_cast<Implementation::Item*>(model_index.internalPointer());
  if (item->parent == nullptr)
    return false;
  YAML::Node config = YAML::Clone(data_->draft);
  YAML::Node parent = nodeAt(config, item->parent->yaml_path);
  bool removed{ false };
  if (item->kind == Implementation::ItemKind::MAP_ENTRY)
    removed = parent.remove(item->key);
  else if (item->kind == Implementation::ItemKind::LIST_ENTRY)
    removed = parent.remove(item->yaml_path.back().index);
  if (!removed)
    return false;
  const QString changed_path = QString::fromStdString(item->parent->path);
  beginResetModel();
  data_->applyConfig(config);
  endResetModel();
  Q_EMIT configurationChanged();
  Q_EMIT configurationChangedAt(changed_path);
  Q_EMIT validationChanged(data_->errors.empty(), data_->errors);
  return true;
}

bool PropertyTreeModel::renameMapEntry(const QModelIndex& model_index, const QString& key)
{
  if (!model_index.isValid() || key.isEmpty())
    return false;
  auto* item = static_cast<Implementation::Item*>(model_index.internalPointer());
  if (item->kind != Implementation::ItemKind::MAP_ENTRY || item->parent == nullptr)
    return false;
  YAML::Node config = YAML::Clone(data_->draft);
  YAML::Node parent = nodeAt(config, item->parent->yaml_path);
  const auto new_key = key.toStdString();
  if (new_key == item->key)
    return true;
  if (parent[new_key])
    return false;
  YAML::Node value = YAML::Clone(parent[item->key]);
  parent.remove(item->key);
  parent[new_key] = value;
  const QString changed_path = QString::fromStdString(item->parent->path);
  beginResetModel();
  data_->applyConfig(config);
  endResetModel();
  Q_EMIT configurationChanged();
  Q_EMIT configurationChangedAt(changed_path);
  Q_EMIT validationChanged(data_->errors.empty(), data_->errors);
  return true;
}

bool PropertyTreeModel::moveListEntry(const QModelIndex& model_index, int destination)
{
  if (!model_index.isValid() || destination < 0)
    return false;
  auto* item = static_cast<Implementation::Item*>(model_index.internalPointer());
  if (item->kind != Implementation::ItemKind::LIST_ENTRY || item->parent == nullptr)
    return false;
  YAML::Node config = YAML::Clone(data_->draft);
  YAML::Node sequence = nodeAt(config, item->parent->yaml_path);
  const auto source = item->yaml_path.back().index;
  if (destination >= static_cast<int>(sequence.size()) || source == static_cast<std::size_t>(destination))
    return false;
  std::vector<std::size_t> indices(sequence.size());
  std::iota(indices.begin(), indices.end(), 0);
  const auto moved = indices[source];
  indices.erase(indices.begin() + static_cast<std::ptrdiff_t>(source));
  indices.insert(indices.begin() + destination, moved);
  YAML::Node replacement(YAML::NodeType::Sequence);
  for (const auto index : indices)
    replacement.push_back(YAML::Clone(sequence[index]));
  replaceNodeAt(config, item->parent->yaml_path, replacement);
  const QString changed_path = QString::fromStdString(item->parent->path);
  beginResetModel();
  data_->applyConfig(config);
  endResetModel();
  Q_EMIT configurationChanged();
  Q_EMIT configurationChangedAt(changed_path);
  Q_EMIT validationChanged(data_->errors.empty(), data_->errors);
  return true;
}

QModelIndex PropertyTreeModel::index(int row, int column, const QModelIndex& parent_index) const
{
  if (!hasIndex(row, column, parent_index) || data_->root == nullptr)
    return {};
  auto* parent =
      parent_index.isValid() ? static_cast<Implementation::Item*>(parent_index.internalPointer()) : data_->root.get();
  if (row < 0 || row >= static_cast<int>(parent->children.size()))
    return {};
  return createIndex(row, column, parent->children[static_cast<std::size_t>(row)].get());
}
QModelIndex PropertyTreeModel::parent(const QModelIndex& child_index) const
{
  if (!child_index.isValid() || data_->root == nullptr)
    return {};
  auto* item = static_cast<Implementation::Item*>(child_index.internalPointer());
  auto* parent_item = item->parent;
  if (parent_item == nullptr || parent_item == data_->root.get())
    return {};
  return createIndex(parent_item->row(), 0, parent_item);
}

int PropertyTreeModel::rowCount(const QModelIndex& parent_index) const
{
  if (data_->root == nullptr || (parent_index.isValid() && parent_index.column() != 0))
    return 0;
  const auto* parent =
      parent_index.isValid() ? static_cast<Implementation::Item*>(parent_index.internalPointer()) : data_->root.get();
  return static_cast<int>(parent->children.size());
}

int PropertyTreeModel::columnCount(const QModelIndex&) const { return 3; }

QVariant PropertyTreeModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return {};
  const auto* item = static_cast<Implementation::Item*>(index.internalPointer());
  const auto& property = *item->property;
  const auto type = propertyType(property);
  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    if (index.column() == 0)
    {
      const auto label = attributeValue(property, tesseract::common::property_attribute::LABEL);
      return label.isValid() ? label : QString::fromStdString(item->key);
    }
    if (index.column() == 1)
    {
      if (item->kind == Implementation::ItemKind::MAP)
        return item->children.size() == 1 ? tr("1 entry") : tr("%1 entries").arg(item->children.size());
      if (item->kind == Implementation::ItemKind::LIST)
        return item->children.size() == 1 ? tr("1 item") : tr("%1 items").arg(item->children.size());
      if (!item->children.empty())
        return {};
      return yamlToVariant(property.getValue());
    }
    if (index.column() == 2)
      return type;
  }
  if (role == Qt::CheckStateRole && index.column() == 1 && type == "bool" && property.getValue() &&
      !property.getValue().IsNull())
    return property.getValue().as<bool>() ? Qt::Checked : Qt::Unchecked;
  if (role == Qt::DecorationRole && index.column() == 0)
  {
    QIcon icon;
    QString cache_key;
    switch (item->kind)
    {
      case Implementation::ItemKind::MAP:
        icon = icons::getKeyValueIcon();
        cache_key = QStringLiteral("map");
        break;
      case Implementation::ItemKind::LIST:
        icon = icons::getArrayIcon();
        cache_key = QStringLiteral("list");
        break;
      case Implementation::ItemKind::MAP_ENTRY:
        if (!item->children.empty())
        {
          icon = icons::getKeyValueIcon();
          cache_key = QStringLiteral("map-entry");
          break;
        }
        [[fallthrough]];
      case Implementation::ItemKind::LIST_ENTRY:
        if (item->kind == Implementation::ItemKind::LIST_ENTRY && !item->children.empty())
        {
          icon = icons::getArrayIcon();
          cache_key = QStringLiteral("list-entry");
          break;
        }
        [[fallthrough]];
      case Implementation::ItemKind::PROPERTY:
      {
        using namespace tesseract::common;
        const bool integer_type = type == typeName(property_type::INT32) || type == typeName(property_type::UINT32) ||
                                  type == typeName(property_type::INT64) || type == typeName(property_type::UINT64);
        if (type == typeName(property_type::CONTAINER))
        {
          icon = icons::getSetIcon();
          cache_key = QStringLiteral("container");
        }
        else if (type == typeName(property_type::STRING))
        {
          icon = item->key == "class" ? icons::getPluginIcon() : icons::getTextIcon();
          cache_key = item->key == "class" ? QStringLiteral("plugin-class") : QStringLiteral("string");
        }
        else if (type == typeName(property_type::CHAR))
        {
          icon = icons::getTextIcon();
          cache_key = QStringLiteral("character");
        }
        else if (type == typeName(property_type::BOOL))
        {
          icon = icons::getTrueFalseIcon();
          cache_key = QStringLiteral("boolean");
        }
        else if (type == typeName(property_type::FLOAT32) || type == typeName(property_type::FLOAT64))
        {
          icon = icons::getNumericIcon();
          cache_key = QStringLiteral("floating-point");
        }
        else if (integer_type)
        {
          icon = icons::getNumericIcon();
          cache_key = QStringLiteral("integer");
        }
        else if (type.startsWith(QStringLiteral("Eigen::")))
        {
          icon = glyphIcon(QStringLiteral("V"), QColor(QStringLiteral("#5e35b1")));
          cache_key = QStringLiteral("eigen-vector");
        }
        else
        {
          icon = icons::getPluginIcon();
          cache_key = QStringLiteral("custom");
        }
        break;
      }
    }
    return withErrorBadge(icon, cache_key, !data_->errorsForPath(item->path).empty());
  }
  switch (role)
  {
    case TypeRole:
      return type;
    case PathRole:
      return QString::fromStdString(item->path);
    case RequiredRole:
      return property.isRequired();
    case DefaultRole:
      return attributeValue(property, tesseract::common::property_attribute::DEFAULT);
    case DocumentationRole:
      return attributeValue(property, tesseract::common::property_attribute::DOC);
    case LabelRole:
      return attributeValue(property, tesseract::common::property_attribute::LABEL);
    case PlaceholderRole:
      return attributeValue(property, tesseract::common::property_attribute::PLACEHOLDER);
    case GroupRole:
      return attributeValue(property, tesseract::common::property_attribute::GROUP);
    case ReadOnlyRole:
      return booleanAttribute(property, tesseract::common::property_attribute::READ_ONLY);
    case MinimumRole:
      return attributeValue(property, tesseract::common::property_attribute::MINIMUM);
    case MaximumRole:
      return attributeValue(property, tesseract::common::property_attribute::MAXIMUM);
    case MinimumLengthRole:
      return attributeValue(property, tesseract::common::property_attribute::MINIMUM_LENGTH);
    case MaximumLengthRole:
      return attributeValue(property, tesseract::common::property_attribute::MAXIMUM_LENGTH);
    case EnumValuesRole:
    {
      const auto values = property.getAttribute(tesseract::common::property_attribute::ENUM);
      if (!values)
        return {};
      QStringList result;
      for (const auto& value : *values)
        result.push_back(QString::fromStdString(value.as<std::string>()));
      return result;
    }
    case ValidationErrorsRole:
      return data_->errorsForPath(item->path);
    case DynamicContainerRole:
      return item->kind == Implementation::ItemKind::MAP || item->kind == Implementation::ItemKind::LIST;
    case CanAddRole:
    {
      if (item->kind == Implementation::ItemKind::MAP)
        return true;
      const auto type_info = tesseract::common::isSequenceType(propertyTypeName(property));
      return item->kind == Implementation::ItemKind::LIST && type_info.has_value() && type_info->second == 0;
    }
    case CanRemoveRole:
      return item->kind == Implementation::ItemKind::MAP_ENTRY || item->kind == Implementation::ItemKind::LIST_ENTRY;
    case CanRenameRole:
      return item->kind == Implementation::ItemKind::MAP_ENTRY;
    case Qt::ToolTipRole:
    {
      QStringList tooltip;
      const auto doc = attributeValue(property, tesseract::common::property_attribute::DOC);
      if (doc.isValid())
        tooltip.push_back(doc.toString());
      tooltip.append(data_->errorsForPath(item->path));
      return tooltip.join("\n");
    }
    case Qt::ForegroundRole:
      return data_->errorsForPath(item->path).empty() ? QVariant() : QVariant::fromValue(QBrush(QColor(Qt::red)));
    default:
      return {};
  }
}

bool PropertyTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid() || index.column() != 1)
    return false;
  auto* item = static_cast<Implementation::Item*>(index.internalPointer());
  const QString changed_path = QString::fromStdString(item->path);
  if (!item->children.empty() || booleanAttribute(*item->property, tesseract::common::property_attribute::READ_ONLY))
    return false;
  try
  {
    QVariant edited = value;
    if (role == Qt::CheckStateRole)
      edited = value.toInt() == Qt::Checked;
    else if (role != Qt::EditRole)
      return false;
    const YAML::Node edited_yaml = variantToYaml(edited, propertyType(*item->property));
    const auto plugin_base_type =
        (item->key == "class" && item->parent != nullptr) ?
            item->parent->property->getAttribute(tesseract::common::property_attribute::PLUGIN_BASE_TYPE) :
            std::nullopt;
    if (!plugin_base_type.has_value())
    {
      item->property->setValue(edited_yaml);
      YAML::Node target = nodeAt(data_->draft, item->yaml_path);
      target = edited_yaml;
      data_->validate();
    }
    else
    {
      YAML::Node config = YAML::Clone(data_->draft);
      YAML::Node target = nodeAt(config, item->yaml_path);
      target = edited_yaml;
      const auto plugin_schemas = data_->pluginSchemas(plugin_base_type->as<std::string>(), item->parent->path);
      const auto selected_schema = plugin_schemas.find(edited_yaml.as<std::string>());
      if (selected_schema == plugin_schemas.end())
        return false;

      YAML::Node plugin_info = nodeAt(config, item->parent->yaml_path);
      plugin_info["config"] = defaultValue(selected_schema->second);
      beginResetModel();
      data_->applyConfig(config);
      endResetModel();
      Q_EMIT configurationChanged();
      Q_EMIT configurationChangedAt(changed_path);
      Q_EMIT validationChanged(data_->errors.empty(), data_->errors);
      return true;
    }
  }
  catch (const std::exception&)
  {
    return false;
  }
  Q_EMIT dataChanged(index, index);
  Q_EMIT configurationChanged();
  Q_EMIT configurationChangedAt(changed_path);
  Q_EMIT validationChanged(data_->errors.empty(), data_->errors);
  return true;
}

QVariant PropertyTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    return {};
  if (section == 0)
    return QStringLiteral("Property");
  if (section == 1)
    return QStringLiteral("Value");
  if (section == 2)
    return QStringLiteral("Type");
  return {};
}

Qt::ItemFlags PropertyTreeModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
    return Qt::NoItemFlags;
  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  const auto* item = static_cast<Implementation::Item*>(index.internalPointer());
  if (index.column() == 1 && item->children.empty() &&
      !booleanAttribute(*item->property, tesseract::common::property_attribute::READ_ONLY))
  {
    flags |= Qt::ItemIsEditable;
    if (propertyType(*item->property) == "bool")
      flags |= Qt::ItemIsUserCheckable;
  }
  return flags;
}
}  // namespace tesseract::gui
