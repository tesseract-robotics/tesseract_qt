#include <tesseract/common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <gtest/gtest.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/common/entity_container.h>
#include <tesseract_qt/common/entity_manager.h>

TEST(EntityContainerUnit, RemoveEntitiesRemovesTrackedAndUntrackedEntitiesById)  // NOLINT
{
  auto manager = std::make_shared<tesseract::gui::EntityManager>();
  auto container = manager->getEntityContainer("container");
  const std::string ns = tesseract::gui::EntityContainer::VISUAL_NS;

  auto link = container->addTrackedEntity(ns, "link");
  auto link_visuals = container->addTrackedEntity(ns, "link::Visuals");
  auto other_link = container->addTrackedEntity(ns, "other_link");
  auto link_geometry = container->addUntrackedEntity(ns);
  auto other_geometry = container->addUntrackedEntity(ns);

  container->removeEntities(ns, { link.id, link_visuals.id, link_geometry.id, -100 });

  EXPECT_FALSE(container->hasTrackedEntity(ns, "link"));
  EXPECT_FALSE(container->hasTrackedEntity(ns, "link::Visuals"));
  EXPECT_TRUE(container->hasTrackedEntity(ns, "other_link"));
  EXPECT_EQ(container->getTrackedEntity(ns, "other_link").id, other_link.id);

  auto untracked = container->getUntrackedEntities(ns);
  ASSERT_EQ(untracked.size(), 1);
  EXPECT_EQ(untracked.front().id, other_geometry.id);

  // Unknown namespaces are ignored
  EXPECT_NO_THROW(container->removeEntities("unknown", { other_link.id }));  // NOLINT
}
