#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>

#include <gtest/gtest.h>

namespace tesseract::gui
{
TEST(ComponentInfoUnit, IsChildRejectsNullOther)
{
  auto component = ComponentInfoManager::create("component_info_unit");

  // isChild() walks other's parent chain, so it must reject null before dereferencing it.
  EXPECT_FALSE(component->isChild(nullptr));
}
}  // namespace tesseract::gui
