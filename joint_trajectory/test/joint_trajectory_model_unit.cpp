#include <tesseract/common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <gtest/gtest.h>
#include <QApplication>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/joint_trajectory/models/joint_trajectory_model.h>
#include <tesseract_qt/joint_trajectory/models/joint_trajectory_set_item.h>
#include <tesseract_qt/joint_trajectory/models/joint_trajectory_utils.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>
#include <tesseract_qt/common/joint_trajectory_set.h>
#include <tesseract_qt/common/events/joint_trajectory_events.h>
#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/status_log_events.h>

#include <tesseract/environment/environment.h>
#include <tesseract/environment/commands/add_link_command.h>
#include <tesseract/scene_graph/graph.h>
#include <tesseract/scene_graph/joint.h>
#include <tesseract/scene_graph/link.h>
#include <tesseract/scene_graph/scene_state.h>

namespace
{
using tesseract::common::JointTrajectorySet;
using tesseract::environment::Environment;
using tesseract::gui::ComponentInfo;
using tesseract::gui::ComponentInfoManager;
using tesseract::gui::DefaultEnvironmentWrapper;
using tesseract::gui::EnvironmentManager;
using tesseract::gui::JointTrajectoryModel;
using tesseract::gui::JointTrajectorySetItem;

const std::string SCENE_NAME{ "joint_trajectory_model_unit" };

/** Initialize env with base_link, plus extra_link fixed to it when non-empty. */
void initEnvironment(Environment& env, const std::string& extra_link = "")
{
  tesseract::scene_graph::SceneGraph scene_graph(SCENE_NAME);
  scene_graph.addLink(tesseract::scene_graph::Link{ tesseract::common::LinkId("base_link") });
  if (!extra_link.empty())
  {
    tesseract::scene_graph::Joint joint{ tesseract::common::JointId("joint_" + extra_link) };
    joint.type = tesseract::scene_graph::JointType::FIXED;
    joint.parent_link_id = tesseract::common::LinkId("base_link");
    joint.child_link_id = tesseract::common::LinkId(extra_link);
    scene_graph.addLink(tesseract::scene_graph::Link{ tesseract::common::LinkId(extra_link) }, joint);
  }

  if (!env.init(scene_graph))
    throw std::runtime_error("initEnvironment: Environment::init failed");
}

std::unique_ptr<Environment> makeEnvironment(const std::string& extra_link = "")
{
  auto env = std::make_unique<Environment>();
  initEnvironment(*env, extra_link);
  return env;
}

std::shared_ptr<const tesseract::environment::Command> addLinkCommand(const std::string& link_name,
                                                                      const std::string& parent_link = "base_link")
{
  tesseract::scene_graph::Link link{ tesseract::common::LinkId(link_name) };
  tesseract::scene_graph::Joint joint{ tesseract::common::JointId("joint_" + link_name) };
  joint.type = tesseract::scene_graph::JointType::FIXED;
  joint.parent_link_id = tesseract::common::LinkId(parent_link);
  joint.child_link_id = tesseract::common::LinkId(link_name);
  return std::make_shared<tesseract::environment::AddLinkCommand>(link, joint);
}

/** Trajectory set without an environment whose one command adds link_name under parent_link. */
JointTrajectorySet makeCommandSet(const std::string& link_name, const std::string& parent_link = "base_link")
{
  return JointTrajectorySet(tesseract::scene_graph::SceneState::JointValues{},
                            { addLinkCommand(link_name, parent_link) });
}

bool hasLink(const std::shared_ptr<const Environment>& env, const std::string& link_name)
{
  return env->getLink(tesseract::common::LinkId(link_name)) != nullptr;
}

/** Trajectory set the model stores under uuid, or nullptr when absent. */
const JointTrajectorySet* findSet(const JointTrajectoryModel& model, const boost::uuids::uuid& uuid)
{
  for (int ns_row = 0; ns_row < model.rowCount(); ++ns_row)
  {
    const QStandardItem* ns_item = model.item(ns_row);
    for (int row = 0; row < ns_item->rowCount(); ++row)
    {
      const auto* set_item = dynamic_cast<const JointTrajectorySetItem*>(ns_item->child(row));
      if (set_item != nullptr && set_item->trajectory_set.getUUID() == uuid)
        return &set_item->trajectory_set;
    }
  }
  return nullptr;
}

/** Add set to the model whose component is component_info, through the event the application sends. */
void addSet(const std::shared_ptr<const ComponentInfo>& component_info, const JointTrajectorySet& set)
{
  tesseract::gui::events::JointTrajectoryAdd event(component_info, set);
  QApplication::sendEvent(qApp, &event);
}

/** Register env under component_info, as JointTrajectoryWidget does when a trajectory set is selected. */
void showInWidget(const std::shared_ptr<const ComponentInfo>& component_info, std::shared_ptr<Environment> env)
{
  if (!tesseract::gui::registersSelectionEnvironment(component_info))
    throw std::logic_error("showInWidget: JointTrajectoryWidget registers no selection under this component");

  EnvironmentManager::set(std::make_shared<DefaultEnvironmentWrapper>(component_info, std::move(env)));
}

/** Records the text of every StatusLogError sent to the application while alive. */
class StatusLogErrorSpy : public QObject
{
public:
  StatusLogErrorSpy() { qApp->installEventFilter(this); }

  bool eventFilter(QObject* obj, QEvent* event) override
  {
    if (event->type() == tesseract::gui::events::EventType::STATUS_LOG_ERROR)
      messages.push_back(static_cast<tesseract::gui::events::StatusLogError*>(event)->getString().toStdString());

    return QObject::eventFilter(obj, event);
  }

  std::vector<std::string> messages;
};

class JointTrajectoryModelUnit : public testing::Test
{
protected:
  void SetUp() override { EnvironmentManager::clear(); }
  void TearDown() override { EnvironmentManager::clear(); }
};
}  // namespace

TEST_F(JointTrajectoryModelUnit, UnparentedComponentClonesItsOwnEnvironment)  // NOLINT
{
  auto component = ComponentInfoManager::create(SCENE_NAME);
  std::shared_ptr<Environment> base = makeEnvironment();
  EnvironmentManager::set(std::make_shared<DefaultEnvironmentWrapper>(component, base));

  JointTrajectoryModel model(component);

  const JointTrajectorySet set = makeCommandSet("link_a");
  addSet(component, set);
  const JointTrajectorySet* stored = findSet(model, set.getUUID());
  ASSERT_NE(stored, nullptr);
  ASSERT_NE(stored->getEnvironment(), nullptr);
  EXPECT_TRUE(hasLink(stored->getEnvironment(), "link_a"));
  EXPECT_FALSE(hasLink(base, "link_a"));
}

TEST_F(JointTrajectoryModelUnit, SelectionEnvironmentIsRegisteredOnlyUnderChildComponents)  // NOLINT
{
  auto root = ComponentInfoManager::create(SCENE_NAME);
  auto child = ComponentInfoManager::createChild(root);

  EXPECT_FALSE(tesseract::gui::registersSelectionEnvironment(nullptr));
  EXPECT_FALSE(tesseract::gui::registersSelectionEnvironment(root));
  EXPECT_TRUE(tesseract::gui::registersSelectionEnvironment(child));
}

TEST_F(JointTrajectoryModelUnit, CommandSetClonesParentEnvironmentAfterSelection)  // NOLINT
{
  auto parent = ComponentInfoManager::create(SCENE_NAME);
  auto child = ComponentInfoManager::createChild(parent);
  std::shared_ptr<Environment> base = makeEnvironment();
  EnvironmentManager::set(std::make_shared<DefaultEnvironmentWrapper>(parent, base));

  JointTrajectoryModel model(child);

  const JointTrajectorySet first = makeCommandSet("link_a");
  addSet(child, first);
  const JointTrajectorySet* stored_first = findSet(model, first.getUUID());
  ASSERT_NE(stored_first, nullptr);
  ASSERT_NE(stored_first->getEnvironment(), nullptr);
  showInWidget(child, stored_first->getEnvironment());

  const JointTrajectorySet second = makeCommandSet("link_a");
  EXPECT_NO_THROW(addSet(child, second));  // NOLINT
  const JointTrajectorySet* stored_second = findSet(model, second.getUUID());
  ASSERT_NE(stored_second, nullptr);
  ASSERT_NE(stored_second->getEnvironment(), nullptr);
  EXPECT_TRUE(hasLink(stored_second->getEnvironment(), "link_a"));
  EXPECT_FALSE(hasLink(base, "link_a"));
}

TEST_F(JointTrajectoryModelUnit, SharedSetClonesParentEnvironmentAfterSelection)  // NOLINT
{
  auto parent = ComponentInfoManager::create(SCENE_NAME);
  auto child = ComponentInfoManager::createChild(parent);
  EnvironmentManager::set(std::make_shared<DefaultEnvironmentWrapper>(parent, makeEnvironment()));

  JointTrajectoryModel model(child);

  const JointTrajectorySet first = makeCommandSet("link_a");
  addSet(child, first);
  const JointTrajectorySet* stored_first = findSet(model, first.getUUID());
  ASSERT_NE(stored_first, nullptr);
  ASSERT_NE(stored_first->getEnvironment(), nullptr);
  showInWidget(child, stored_first->getEnvironment());

  const JointTrajectorySet second(tesseract::scene_graph::SceneState::JointValues{});
  addSet(child, second);
  const JointTrajectorySet* stored_second = findSet(model, second.getUUID());
  ASSERT_NE(stored_second, nullptr);
  ASSERT_NE(stored_second->getEnvironment(), nullptr);
  EXPECT_FALSE(hasLink(stored_second->getEnvironment(), "link_a"));
}

TEST_F(JointTrajectoryModelUnit, SelectionIsNeverUsedAsDefaultSource)  // NOLINT
{
  auto parent = ComponentInfoManager::create(SCENE_NAME);
  auto child = ComponentInfoManager::createChild(parent);

  JointTrajectoryModel model(child);

  const JointTrajectorySet with_environment(makeEnvironment("link_d"));
  addSet(child, with_environment);
  const JointTrajectorySet* stored_d = findSet(model, with_environment.getUUID());
  ASSERT_NE(stored_d, nullptr);
  // Nothing else is registered, so this selection becomes the manager's default.
  showInWidget(child, stored_d->getEnvironment());

  const JointTrajectorySet commands_only = makeCommandSet("link_e");
  addSet(child, commands_only);
  const JointTrajectorySet* stored_e = findSet(model, commands_only.getUUID());
  ASSERT_NE(stored_e, nullptr);
  EXPECT_EQ(stored_e->getEnvironment(), nullptr);
  EXPECT_EQ(stored_e->getEnvironmentCommands().size(), 1U);
}

TEST_F(JointTrajectoryModelUnit, SharedCloneIsNotReusedForReplacedEnvironment)  // NOLINT
{
  auto component = ComponentInfoManager::create(SCENE_NAME);
  JointTrajectoryModel model(component);

  std::shared_ptr<Environment> first_env = makeEnvironment("link_first");
  const int first_revision = first_env->getRevision();
  const std::weak_ptr<Environment> first_watch = first_env;
  EnvironmentManager::set(std::make_shared<DefaultEnvironmentWrapper>(component, first_env));

  // The stored set keeps the shared clone of first_env alive after first_env is destroyed.
  const JointTrajectorySet first(tesseract::scene_graph::SceneState::JointValues{});
  addSet(component, first);
  const JointTrajectorySet* stored_first = findSet(model, first.getUUID());
  ASSERT_NE(stored_first, nullptr);
  ASSERT_TRUE(hasLink(stored_first->getEnvironment(), "link_first"));

  EnvironmentManager::remove(component);
  first_env.reset();
  ASSERT_TRUE(first_watch.expired());

  // A replacement at the same revision must be cloned afresh, never served the dead environment's clone.
  std::shared_ptr<Environment> second_env = makeEnvironment("link_second");
  ASSERT_EQ(second_env->getRevision(), first_revision);
  EnvironmentManager::set(std::make_shared<DefaultEnvironmentWrapper>(component, second_env));

  const JointTrajectorySet second(tesseract::scene_graph::SceneState::JointValues{});
  addSet(component, second);
  const JointTrajectorySet* stored_second = findSet(model, second.getUUID());
  ASSERT_NE(stored_second, nullptr);
  ASSERT_NE(stored_second->getEnvironment(), nullptr);
  EXPECT_TRUE(hasLink(stored_second->getEnvironment(), "link_second"));
  EXPECT_FALSE(hasLink(stored_second->getEnvironment(), "link_first"));
}

TEST_F(JointTrajectoryModelUnit, SiblingSelectionIsNeverUsedAsDefaultSource)  // NOLINT
{
  auto parent = ComponentInfoManager::create(SCENE_NAME);
  auto first_widget = ComponentInfoManager::createChild(parent);
  auto second_widget = ComponentInfoManager::createChild(parent);

  // Nothing is registered under parent, so the sibling widget's selection is the only environment and so the default.
  showInWidget(first_widget, makeEnvironment("link_selected"));

  JointTrajectoryModel model(second_widget);
  const JointTrajectorySet set = makeCommandSet("link_a");
  addSet(second_widget, set);

  const JointTrajectorySet* stored = findSet(model, set.getUUID());
  ASSERT_NE(stored, nullptr);
  EXPECT_EQ(stored->getEnvironment(), nullptr);
}

TEST_F(JointTrajectoryModelUnit, SetRemovedByRemoveAllCanBeAddedAgain)  // NOLINT
{
  auto component = ComponentInfoManager::create(SCENE_NAME);
  EnvironmentManager::set(std::make_shared<DefaultEnvironmentWrapper>(component, makeEnvironment()));

  JointTrajectoryModel model(component);
  const JointTrajectorySet set = makeCommandSet("link_a");
  addSet(component, set);
  ASSERT_NE(findSet(model, set.getUUID()), nullptr);

  tesseract::gui::events::JointTrajectoryRemoveAll remove_all(component);
  QApplication::sendEvent(qApp, &remove_all);
  ASSERT_EQ(findSet(model, set.getUUID()), nullptr);

  // The removal deleted every item, so re-adding the same uuid must not reach the one it deleted.
  addSet(component, set);
  const JointTrajectorySet* stored = findSet(model, set.getUUID());
  ASSERT_NE(stored, nullptr);
  ASSERT_NE(stored->getEnvironment(), nullptr);
  EXPECT_TRUE(hasLink(stored->getEnvironment(), "link_a"));
}

TEST_F(JointTrajectoryModelUnit, AddEventWithFailingCommandsReportsError)  // NOLINT
{
  auto component = ComponentInfoManager::create(SCENE_NAME);
  EnvironmentManager::set(std::make_shared<DefaultEnvironmentWrapper>(component, makeEnvironment()));
  JointTrajectoryModel model(component);
  StatusLogErrorSpy spy;

  const JointTrajectorySet set = makeCommandSet("link_f", "no_such_link");
  tesseract::gui::events::JointTrajectoryAdd event(component, set);
  EXPECT_NO_THROW(QApplication::sendEvent(qApp, &event));  // NOLINT

  EXPECT_EQ(findSet(model, set.getUUID()), nullptr);
  EXPECT_EQ(spy.messages.size(), 1U);
}

TEST_F(JointTrajectoryModelUnit, RemoveEventForUnknownSetReportsError)  // NOLINT
{
  auto component = ComponentInfoManager::create(SCENE_NAME);
  JointTrajectoryModel model(component);

  const JointTrajectorySet set = makeCommandSet("link_g");
  addSet(component, set);
  ASSERT_NE(findSet(model, set.getUUID()), nullptr);

  StatusLogErrorSpy spy;
  tesseract::gui::events::JointTrajectoryRemove event(component, boost::uuids::random_generator()());
  EXPECT_NO_THROW(QApplication::sendEvent(qApp, &event));  // NOLINT

  EXPECT_EQ(spy.messages.size(), 1U);
  // The failed removal must leave the unrelated, already-stored set untouched.
  EXPECT_NE(findSet(model, set.getUUID()), nullptr);
}

TEST_F(JointTrajectoryModelUnit, NullComponentInfoIsRejectedNotDereferenced)  // NOLINT
{
  auto component = ComponentInfoManager::create(SCENE_NAME);
  EnvironmentManager::set(std::make_shared<DefaultEnvironmentWrapper>(component, makeEnvironment()));

  // Both must refuse a null component rather than dereference it. find() is reached with one in production:
  // ManipulationWidget's convenience constructor passes null.
  EXPECT_EQ(EnvironmentManager::find(nullptr), nullptr);
  EXPECT_FALSE(component->isChild(nullptr));
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  qputenv("QT_QPA_PLATFORM", "offscreen");
  QApplication app(argc, argv);
  return RUN_ALL_TESTS();
}
