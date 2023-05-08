#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <memory>
#include <string>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/common/plugin_infos.h>
#include <tesseract_common/yaml_utils.h>

namespace tesseract_gui
{
/*********************************************************/
/******          StudioPluginInfo           *****/
/*********************************************************/
void StudioPluginInfo::insert(const StudioPluginInfo& other)
{
  search_paths.insert(other.search_paths.begin(), other.search_paths.end());
  search_libraries.insert(other.search_libraries.begin(), other.search_libraries.end());

  if (!other.plugin_infos.default_plugin.empty())
    plugin_infos.default_plugin = other.plugin_infos.default_plugin;

  for (const auto& plugin_info : other.plugin_infos.plugins)
    plugin_infos.plugins[plugin_info.first] = plugin_info.second;

  for (const auto& component_info : other.component_infos)
    component_infos[component_info.first] = component_info.second;
}

void StudioPluginInfo::clear()
{
  search_paths.clear();
  search_libraries.clear();
  component_infos.clear();
  plugin_infos.clear();
}

bool StudioPluginInfo::empty() const
{
  return (search_paths.empty() && search_libraries.empty() && component_infos.empty() && plugin_infos.plugins.empty());
}

bool StudioPluginInfo::operator==(const StudioPluginInfo& rhs) const
{
  bool equal = true;
  equal &= tesseract_common::isIdenticalSet<std::string>(search_paths, rhs.search_paths);
  equal &= tesseract_common::isIdenticalSet<std::string>(search_libraries, rhs.search_libraries);
  equal &= (component_infos == rhs.component_infos);
  equal &= (plugin_infos == rhs.plugin_infos);
  return equal;
}
bool StudioPluginInfo::operator!=(const StudioPluginInfo& rhs) const { return !operator==(rhs); }

template <class Archive>
void StudioPluginInfo::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& BOOST_SERIALIZATION_NVP(search_paths);
  ar& BOOST_SERIALIZATION_NVP(search_libraries);
  ar& BOOST_SERIALIZATION_NVP(component_infos);
  ar& BOOST_SERIALIZATION_NVP(plugin_infos);
}

}  // namespace tesseract_gui

#include <tesseract_common/serialization.h>
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_gui::StudioPluginInfo)
BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_gui::StudioPluginInfo)
