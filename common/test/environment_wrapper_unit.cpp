#include <tesseract/common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <gtest/gtest.h>
#include <QApplication>
#include <QCoreApplication>
#include <QEvent>
#include <QThread>
#include <algorithm>
#include <chrono>
#include <thread>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract/environment/environment.h>
#include <tesseract/environment/commands/add_kinematics_information_command.h>
#include <tesseract/environment/commands/add_link_command.h>
#include <tesseract/environment/commands/change_joint_origin_command.h>
#include <tesseract/environment/commands/change_link_collision_enabled_command.h>
#include <tesseract/environment/commands/remove_link_command.h>
#include <tesseract/scene_graph/graph.h>
#include <tesseract/scene_graph/joint.h>
#include <tesseract/scene_graph/link.h>

#include <tesseract_qt/common/component_info_manager.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>
#include <tesseract_qt/common/events/environment_events.h>
#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/environment/models/environment_commands_model.h>

namespace
{
tesseract::scene_graph::SceneGraph makeSceneGraph()
{
  tesseract::scene_graph::SceneGraph scene_graph("test_scene_graph");
  scene_graph.addLink(tesseract::scene_graph::Link("base_link"));
  return scene_graph;
}

tesseract::scene_graph::Joint makeFixedJoint(const std::string& name,
                                             const std::string& parent,
                                             const std::string& child)
{
  tesseract::scene_graph::Joint joint(name);
  joint.type = tesseract::scene_graph::JointType::FIXED;
  joint.parent_link_id = parent;
  joint.child_link_id = child;
  return joint;
}

std::shared_ptr<tesseract::environment::AddLinkCommand> makeAddLinkCommand(const std::string& link_name,
                                                                           const std::string& joint_name,
                                                                           const std::string& parent,
                                                                           bool replace_allowed = false)
{
  return std::make_shared<tesseract::environment::AddLinkCommand>(
      tesseract::scene_graph::Link(link_name), makeFixedJoint(joint_name, parent, link_name), replace_allowed);
}

class SceneGraphEventRecorder : public QObject
{
public:
  void reset()
  {
    scene_graph_set_count = 0;
    commands_set_count = 0;
    commands_set_size = 0;
    kinematic_groups_set_count = 0;
    commands_appended = 0;
    event_order.clear();
    added_links.clear();
    added_joints.clear();
    added_joint_objects.clear();
    removed_links.clear();
    removed_joints.clear();
  }

  int scene_graph_set_count{ 0 };
  int commands_set_count{ 0 };
  int kinematic_groups_set_count{ 0 };
  std::size_t commands_set_size{ 0 };
  std::size_t commands_appended{ 0 };
  std::vector<int> event_order;
  std::vector<std::string> added_links;
  std::vector<std::string> added_joints;
  std::vector<std::shared_ptr<const tesseract::scene_graph::Joint>> added_joint_objects;
  std::vector<std::string> removed_links;
  std::vector<std::string> removed_joints;
  Qt::HANDLE last_thread{ nullptr };

protected:
  bool eventFilter(QObject* watched, QEvent* event) override
  {
    using tesseract::gui::events::EventType;
    const auto type = event->type();
    if (type == EventType::SCENE_GRAPH_SET)
      ++scene_graph_set_count;
    else if (type == EventType::SCENE_GRAPH_ADD_LINK)
      added_links.push_back(static_cast<tesseract::gui::events::SceneGraphAddLink*>(event)->getLink()->getName());
    else if (type == EventType::SCENE_GRAPH_ADD_JOINT)
    {
      const auto& joint = static_cast<tesseract::gui::events::SceneGraphAddJoint*>(event)->getJoint();
      added_joints.push_back(joint->getName());
      added_joint_objects.push_back(joint);
    }
    else if (type == EventType::SCENE_GRAPH_REMOVE_LINK)
      removed_links.push_back(static_cast<tesseract::gui::events::SceneGraphRemoveLink*>(event)->getLinkName());
    else if (type == EventType::SCENE_GRAPH_REMOVE_JOINT)
      removed_joints.push_back(static_cast<tesseract::gui::events::SceneGraphRemoveJoint*>(event)->getJointName());
    else if (type == EventType::KINEMATIC_GROUPS_SET)
      ++kinematic_groups_set_count;
    else if (type == EventType::ENVIRONMENT_COMMANDS_SET)
    {
      ++commands_set_count;
      commands_set_size = static_cast<tesseract::gui::events::EnvironmentCommandsSet*>(event)->getCommands().size();
    }
    else if (type == EventType::ENVIRONMENT_COMMANDS_APPEND)
      commands_appended += static_cast<tesseract::gui::events::EnvironmentCommandsAppend*>(event)->getCommands().size();
    else
      return QObject::eventFilter(watched, event);

    event_order.push_back(type);
    last_thread = QThread::currentThreadId();
    return QObject::eventFilter(watched, event);
  }
};

std::vector<std::string> sorted(std::vector<std::string> values)
{
  std::sort(values.begin(), values.end());
  return values;
}

void processPendingEvents()
{
  for (int i = 0; i < 5; ++i)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
}

class EnvironmentWrapperUnit : public ::testing::Test
{
protected:
  void SetUp() override
  {
    tesseract::gui::EnvironmentManager::clear();
    tesseract::gui::ComponentInfoManager::clear();

    component_info = tesseract::gui::ComponentInfoManager::create("wrapper_test_scene", "wrapper_test_component");
    ASSERT_NE(component_info, nullptr);

    env = std::make_shared<tesseract::environment::Environment>();
    ASSERT_TRUE(env->init(makeSceneGraph()));

    wrapper = std::make_shared<tesseract::gui::DefaultEnvironmentWrapper>(component_info, env);
    tesseract::gui::EnvironmentManager::set(wrapper);
    processPendingEvents();

    qApp->installEventFilter(&recorder);
  }

  void TearDown() override
  {
    qApp->removeEventFilter(&recorder);
    tesseract::gui::EnvironmentManager::clear();
    tesseract::gui::ComponentInfoManager::clear();
  }

  std::shared_ptr<tesseract::gui::ComponentInfo> component_info;
  std::shared_ptr<tesseract::environment::Environment> env;
  std::shared_ptr<tesseract::gui::DefaultEnvironmentWrapper> wrapper;
  SceneGraphEventRecorder recorder;
};
}  // namespace

TEST_F(EnvironmentWrapperUnit, AddLinkSendsIncrementalEvents)  // NOLINT
{
  ASSERT_TRUE(env->applyCommand(makeAddLinkCommand("tool_link", "tool_joint", "base_link")));
  processPendingEvents();

  EXPECT_EQ(recorder.scene_graph_set_count, 0);
  EXPECT_EQ(recorder.added_links, std::vector<std::string>{ "tool_link" });
  EXPECT_EQ(recorder.added_joints, std::vector<std::string>{ "tool_joint" });
  EXPECT_TRUE(recorder.removed_links.empty());
  EXPECT_TRUE(recorder.removed_joints.empty());
  EXPECT_EQ(recorder.commands_set_count, 1);
  EXPECT_EQ(recorder.commands_set_size, env->getCommandHistory().size());
  EXPECT_EQ(recorder.commands_appended, 0);
  // Unchanged kinematics information is not re-sent
  EXPECT_EQ(recorder.kinematic_groups_set_count, 0);
}

TEST_F(EnvironmentWrapperUnit, KinematicsInformationAddedInTopologyBatchIsSent)  // NOLINT
{
  tesseract::srdf::KinematicsInformation kin_info;
  kin_info.link_groups["tool_group"] = { "tool_link" };
  ASSERT_TRUE(
      env->applyCommands({ makeAddLinkCommand("tool_link", "tool_joint", "base_link"),
                           std::make_shared<tesseract::environment::AddKinematicsInformationCommand>(kin_info) }));
  processPendingEvents();

  EXPECT_EQ(recorder.added_links, std::vector<std::string>{ "tool_link" });
  EXPECT_EQ(recorder.kinematic_groups_set_count, 1);
}

TEST_F(EnvironmentWrapperUnit, CommandsModelAppendsNewCommandsWithoutRebuilding)  // NOLINT
{
  tesseract::gui::EnvironmentCommandsModel model(component_info);
  auto* root = model.item(0);
  ASSERT_NE(root, nullptr);
  ASSERT_EQ(root->rowCount(), static_cast<int>(env->getCommandHistory().size()));
  auto* first_command = root->child(0);

  ASSERT_TRUE(env->applyCommand(makeAddLinkCommand("tool_link", "tool_joint", "base_link")));
  processPendingEvents();

  ASSERT_EQ(model.item(0), root);
  EXPECT_EQ(root->child(0), first_command);
  EXPECT_EQ(root->rowCount(), static_cast<int>(env->getCommandHistory().size()));
  EXPECT_EQ(model.getCommands().size(), env->getCommandHistory().size());
}

TEST_F(EnvironmentWrapperUnit, NonTopologyCommandSendsNoSceneGraphOrCommandEvents)  // NOLINT
{
  ASSERT_TRUE(env->applyCommand(
      std::make_shared<tesseract::environment::ChangeLinkCollisionEnabledCommand>("base_link", false)));
  processPendingEvents();

  EXPECT_EQ(recorder.scene_graph_set_count, 0);
  EXPECT_TRUE(recorder.added_links.empty());
  EXPECT_TRUE(recorder.removed_links.empty());
  EXPECT_EQ(recorder.commands_set_count, 0);
  EXPECT_EQ(recorder.commands_appended, 0);
}

TEST_F(EnvironmentWrapperUnit, QueuedFullUpdateIsDeliveredBeforeLaterIncrementalUpdate)  // NOLINT
{
  // Queue a full update from a worker thread, then add a link on the GUI thread before the queue is processed
  std::thread worker([this]() { wrapper->broadcast(); });
  worker.join();

  ASSERT_TRUE(env->applyCommand(makeAddLinkCommand("tool_link", "tool_joint", "base_link")));
  processPendingEvents();

  using tesseract::gui::events::EventType;
  auto set_it = std::find(recorder.event_order.begin(), recorder.event_order.end(), EventType::SCENE_GRAPH_SET);
  auto add_it = std::find(recorder.event_order.begin(), recorder.event_order.end(), EventType::SCENE_GRAPH_ADD_LINK);
  ASSERT_NE(set_it, recorder.event_order.end());
  ASSERT_NE(add_it, recorder.event_order.end());
  EXPECT_LT(set_it, add_it);
}

TEST_F(EnvironmentWrapperUnit, QueuedUpdatesAreDroppedWhenWrapperIsDestroyed)  // NOLINT
{
  std::thread worker([this]() { env->applyCommand(makeAddLinkCommand("tool_link", "tool_joint", "base_link")); });
  worker.join();

  tesseract::gui::EnvironmentManager::remove(component_info);
  wrapper.reset();
  processPendingEvents();

  EXPECT_TRUE(recorder.added_links.empty());
}

TEST_F(EnvironmentWrapperUnit, ReplaceLinkRemovesThenAddsLink)  // NOLINT
{
  ASSERT_TRUE(env->applyCommand(makeAddLinkCommand("tool_link", "tool_joint", "base_link")));
  processPendingEvents();
  recorder.reset();

  ASSERT_TRUE(env->applyCommand(makeAddLinkCommand("tool_link", "tool_joint", "base_link", true)));
  processPendingEvents();

  EXPECT_EQ(recorder.scene_graph_set_count, 0);
  EXPECT_EQ(recorder.removed_links, std::vector<std::string>{ "tool_link" });
  EXPECT_EQ(recorder.added_links, std::vector<std::string>{ "tool_link" });
}

TEST_F(EnvironmentWrapperUnit, RemoveLinkRemovesChildrenIncrementally)  // NOLINT
{
  ASSERT_TRUE(env->applyCommand(makeAddLinkCommand("tool_link", "tool_joint", "base_link")));
  ASSERT_TRUE(env->applyCommand(makeAddLinkCommand("tip_link", "tip_joint", "tool_link")));
  processPendingEvents();
  recorder.reset();

  ASSERT_TRUE(env->applyCommand(std::make_shared<tesseract::environment::RemoveLinkCommand>("tool_link")));
  processPendingEvents();

  EXPECT_EQ(recorder.scene_graph_set_count, 0);
  EXPECT_EQ(sorted(recorder.removed_links), (std::vector<std::string>{ "tip_link", "tool_link" }));
  EXPECT_EQ(sorted(recorder.removed_joints), (std::vector<std::string>{ "tip_joint", "tool_joint" }));
  EXPECT_TRUE(recorder.added_links.empty());
}

TEST_F(EnvironmentWrapperUnit, JointModifiedInPlaceInTopologyBatchIsResent)  // NOLINT
{
  ASSERT_TRUE(env->applyCommand(makeAddLinkCommand("tool_link", "tool_joint", "base_link")));
  processPendingEvents();
  recorder.reset();

  Eigen::Isometry3d origin = Eigen::Isometry3d::Identity();
  origin.translation() = Eigen::Vector3d(0.1, 0.2, 0.3);
  ASSERT_TRUE(
      env->applyCommands({ makeAddLinkCommand("tip_link", "tip_joint", "tool_link"),
                           std::make_shared<tesseract::environment::ChangeJointOriginCommand>("tool_joint", origin) }));
  processPendingEvents();

  EXPECT_EQ(recorder.scene_graph_set_count, 0);
  EXPECT_EQ(recorder.removed_joints, std::vector<std::string>{ "tool_joint" });
  EXPECT_TRUE(recorder.removed_links.empty());
  EXPECT_EQ(recorder.added_links, std::vector<std::string>{ "tip_link" });
  ASSERT_EQ(sorted(recorder.added_joints), (std::vector<std::string>{ "tip_joint", "tool_joint" }));
  for (const auto& joint : recorder.added_joint_objects)
  {
    if (joint->getName() == "tool_joint")
    {
      EXPECT_TRUE(joint->parent_to_joint_origin_transform.isApprox(origin));
    }
  }
}

TEST_F(EnvironmentWrapperUnit, LinkModifiedInPlaceInTopologyBatchIsResent)  // NOLINT
{
  ASSERT_TRUE(env->applyCommands(
      { makeAddLinkCommand("tool_link", "tool_joint", "base_link"),
        std::make_shared<tesseract::environment::ChangeLinkCollisionEnabledCommand>("base_link", false) }));
  processPendingEvents();

  EXPECT_EQ(recorder.scene_graph_set_count, 0);
  EXPECT_EQ(recorder.removed_links, std::vector<std::string>{ "base_link" });
  EXPECT_EQ(sorted(recorder.added_links), (std::vector<std::string>{ "base_link", "tool_link" }));
  EXPECT_TRUE(recorder.removed_joints.empty());
}

TEST_F(EnvironmentWrapperUnit, IncrementalEventsAreDeliveredOnGuiThread)  // NOLINT
{
  std::thread worker([this]() { env->applyCommand(makeAddLinkCommand("tool_link", "tool_joint", "base_link")); });

  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  while (recorder.added_links.empty() && std::chrono::steady_clock::now() < deadline)
  {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  worker.join();
  processPendingEvents();

  EXPECT_EQ(recorder.added_links, std::vector<std::string>{ "tool_link" });
  EXPECT_EQ(recorder.last_thread, qApp->thread()->currentThreadId());
}

int main(int argc, char** argv)
{
  qputenv("QT_QPA_PLATFORM", QByteArray("offscreen"));
  QApplication application(argc, argv);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
