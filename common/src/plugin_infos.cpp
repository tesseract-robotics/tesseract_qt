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
#include <boost/serialization/shared_ptr.hpp>
#include <memory>
#include <string>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/common/plugin_infos.h>
#include <tesseract_common/yaml_utils.h>

namespace tesseract_gui
{
void StudioPluginInfo::clear()
{
  search_paths.clear();
  search_libraries.clear();
  plugin_infos.clear();
}

bool StudioPluginInfo::empty() const
{
  return (search_paths.empty() && search_libraries.empty() && plugin_infos.plugins.empty());
}

bool StudioPluginInfo::operator==(const StudioPluginInfo& rhs) const
{
  bool equal = true;
  equal &= tesseract_common::isIdenticalSet<std::string>(search_paths, rhs.search_paths);
  equal &= tesseract_common::isIdenticalSet<std::string>(search_libraries, rhs.search_libraries);
  equal &= (plugin_infos == rhs.plugin_infos);
  return equal;
}
bool StudioPluginInfo::operator!=(const StudioPluginInfo& rhs) const { return !operator==(rhs); }

template <class Archive>
void StudioPluginInfo::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& BOOST_SERIALIZATION_NVP(search_paths);
  ar& BOOST_SERIALIZATION_NVP(search_libraries);
  //  ar& BOOST_SERIALIZATION_NVP(component_infos);
  ar& BOOST_SERIALIZATION_NVP(plugin_infos);
}

}  // namespace tesseract_gui

#include <tesseract_common/serialization.h>
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_gui::StudioPluginInfo)
BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_gui::StudioPluginInfo)
