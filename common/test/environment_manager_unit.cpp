#include <tesseract_qt/common/environment_manager.h>

#include <gtest/gtest.h>

namespace tesseract::gui
{
TEST(EnvironmentManagerUnit, FindRejectsNullComponentInfo)
{
  // find() must refuse a null component rather than dereference it while walking up the parent chain.
  // ManipulationWidget's convenience constructor reaches it with one.
  EXPECT_EQ(EnvironmentManager::find(nullptr), nullptr);
}
}  // namespace tesseract::gui
