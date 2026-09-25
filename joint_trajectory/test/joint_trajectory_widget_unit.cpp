#include <tesseract/common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <gtest/gtest.h>
#include <Eigen/Core>
#include <QApplication>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QItemSelectionModel>
#include <QSlider>
#include <QStandardItem>
#include <boost/uuid/uuid.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/joint_trajectory/widgets/joint_trajectory_widget.h>
#include <tesseract_qt/joint_trajectory/models/joint_trajectory_model.h>
#include <tesseract_qt/joint_trajectory/models/joint_trajectory_set_item.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>
#include <tesseract_qt/common/joint_trajectory_set.h>
#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/joint_trajectory_events.h>
#include <tesseract_qt/common/events/status_log_events.h>

#include <tesseract/common/joint_state.h>
#include <tesseract/environment/environment.h>
#include <tesseract/scene_graph/graph.h>
#include <tesseract/scene_graph/joint.h>
#include <tesseract/scene_graph/link.h>
#include <tesseract/scene_graph/scene_state.h>

namespace
{
using tesseract::common::JointId;
using tesseract::common::JointTrajectorySet;
using tesseract::environment::Environment;
using tesseract::gui::ComponentInfoManager;
using tesseract::gui::DefaultEnvironmentWrapper;
using tesseract::gui::EnvironmentManager;
using tesseract::gui::JointTrajectoryModel;
using tesseract::gui::JointTrajectorySetItem;
using tesseract::gui::JointTrajectoryWidget;

const std::string SCENE_NAME{ "joint_trajectory_widget_unit" };

/** Environment with continuous joints j1 and j2 on base_link, both at zero. */
std::unique_ptr<Environment> makeEnvironment()
{
  tesseract::scene_graph::SceneGraph scene_graph(SCENE_NAME);
  scene_graph.addLink(tesseract::scene_graph::Link{ tesseract::common::LinkId("base_link") });
  for (const char* suffix : { "1", "2" })
  {
    tesseract::scene_graph::Joint joint{ JointId(std::string("j") + suffix) };
    joint.type = tesseract::scene_graph::JointType::CONTINUOUS;
    joint.axis = Eigen::Vector3d::UnitZ();
    joint.parent_link_id = tesseract::common::LinkId("base_link");
    joint.child_link_id = tesseract::common::LinkId(std::string("link_") + suffix);
    scene_graph.addLink(tesseract::scene_graph::Link{ tesseract::common::LinkId(std::string("link_") + suffix) },
                        joint);
  }

  auto env = std::make_unique<Environment>();
  if (!env->init(scene_graph))
    throw std::runtime_error("makeEnvironment: Environment::init failed");
  return env;
}

/** Trajectory that moves joint_name from 0 to 1 over one second. */
tesseract::common::JointTrajectory makeMotion(const std::string& joint_name)
{
  std::vector<tesseract::common::JointState> states;
  for (const double position : { 0.0, 1.0 })
  {
    states.emplace_back(std::vector<JointId>{ JointId(joint_name) }, Eigen::VectorXd::Constant(1, position));
    states.back().time = position;
  }
  return tesseract::common::JointTrajectory(std::move(states));
}

/** Set without an environment whose one trajectory moves joint_name from 0 to 1. */
JointTrajectorySet makeMotionSet(const std::string& joint_name)
{
  JointTrajectorySet set(tesseract::scene_graph::SceneState::JointValues{ { JointId(joint_name), 0.0 } });
  set.appendJointTrajectory(makeMotion(joint_name));
  return set;
}

double jointValue(const Environment& env, const std::string& joint_name)
{
  return env.getCurrentJointValues(std::vector<JointId>{ JointId(joint_name) })(0);
}

/** Model index of the set stored under uuid; invalid when absent. */
QModelIndex setIndex(const JointTrajectoryModel& model, const boost::uuids::uuid& uuid)
{
  for (int ns_row = 0; ns_row < model.rowCount(); ++ns_row)
  {
    const QStandardItem* ns_item = model.item(ns_row);
    for (int row = 0; row < ns_item->rowCount(); ++row)
    {
      const auto* set_item = dynamic_cast<const JointTrajectorySetItem*>(ns_item->child(row));
      if (set_item != nullptr && set_item->trajectory_set.getUUID() == uuid)
        return model.indexFromItem(set_item);
    }
  }
  return {};
}

/** Make index the widget's current row, as clicking it in the tree does. */
void select(JointTrajectoryWidget& widget, const QModelIndex& index)
{
  widget.getSelectionModel().setCurrentIndex(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

/** Add set to the model whose component is component_info, through the event the application sends. */
void addSet(const std::shared_ptr<const tesseract::gui::ComponentInfo>& component_info, const JointTrajectorySet& set)
{
  tesseract::gui::events::JointTrajectoryAdd event(component_info, set);
  QApplication::sendEvent(qApp, &event);
}

QSlider& slider(JointTrajectoryWidget& widget)
{
  auto* trajectory_slider = widget.findChild<QSlider*>("trajectorySlider");
  if (trajectory_slider == nullptr)
    throw std::logic_error("slider: JointTrajectoryWidget has no trajectorySlider");
  return *trajectory_slider;
}

/**
 * Pump the event loop until at least ms of wall-clock time have passed, so a QTimer::singleShot scheduled before
 * the call has had the chance to fire.
 */
void spin(int ms)
{
  QElapsedTimer timer;
  timer.start();
  while (timer.elapsed() < ms)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
}

/** Records every StatusLogWarn sent to the application while alive. */
class StatusLogWarnSpy : public QObject
{
public:
  StatusLogWarnSpy() { qApp->installEventFilter(this); }

  bool eventFilter(QObject* obj, QEvent* event) override
  {
    if (event->type() == tesseract::gui::events::EventType::STATUS_LOG_WARN)
      messages.push_back(static_cast<tesseract::gui::events::StatusLogWarn*>(event)->getString().toStdString());

    return QObject::eventFilter(obj, event);
  }

  std::vector<std::string> messages;
};

class JointTrajectoryWidgetUnit : public testing::Test
{
protected:
  void SetUp() override { EnvironmentManager::clear(); }
  void TearDown() override { EnvironmentManager::clear(); }
};
}  // namespace

TEST_F(JointTrajectoryWidgetUnit, SelectingAnotherSetRestoresSharedEnvironment)  // NOLINT
{
  auto parent = ComponentInfoManager::create(SCENE_NAME);
  EnvironmentManager::set(std::make_shared<DefaultEnvironmentWrapper>(parent, makeEnvironment()));
  JointTrajectoryWidget widget(ComponentInfoManager::createChild(parent));
  auto model = widget.getModel();

  const JointTrajectorySet moves_j1 = makeMotionSet("j1");
  const JointTrajectorySet moves_j2 = makeMotionSet("j2");
  addSet(widget.getComponentInfo(), moves_j1);
  addSet(widget.getComponentInfo(), moves_j2);
  const QModelIndex j1_index = setIndex(*model, moves_j1.getUUID());
  const QModelIndex j2_index = setIndex(*model, moves_j2.getUUID());
  ASSERT_TRUE(j1_index.isValid());
  ASSERT_TRUE(j2_index.isValid());
  const auto env = model->getJointTrajectorySet(j1_index).getEnvironment();
  ASSERT_NE(env, nullptr);
  // Both sets arrived at one source revision, so they share one environment.
  ASSERT_EQ(model->getJointTrajectorySet(j2_index).getEnvironment(), env);

  select(widget, j1_index);
  slider(widget).setValue(slider(widget).maximum());
  ASSERT_NEAR(jointValue(*env, "j1"), 1.0, 1e-6);

  select(widget, j2_index);
  EXPECT_NEAR(jointValue(*env, "j1"), 0.0, 1e-6);
}

TEST_F(JointTrajectoryWidgetUnit, SetWithoutEnvironmentDrivesNoEnvironment)  // NOLINT
{
  auto parent = ComponentInfoManager::create(SCENE_NAME);
  JointTrajectoryWidget widget(ComponentInfoManager::createChild(parent));
  auto model = widget.getModel();

  JointTrajectorySet owns_environment(makeEnvironment());
  owns_environment.appendJointTrajectory(makeMotion("j2"));
  const JointTrajectorySet without_environment = makeMotionSet("j1");
  addSet(widget.getComponentInfo(), owns_environment);
  addSet(widget.getComponentInfo(), without_environment);
  const QModelIndex owner_index = setIndex(*model, owns_environment.getUUID());
  const QModelIndex bare_index = setIndex(*model, without_environment.getUUID());
  ASSERT_TRUE(owner_index.isValid());
  ASSERT_TRUE(bare_index.isValid());
  const auto env = model->getJointTrajectorySet(owner_index).getEnvironment();
  ASSERT_NE(env, nullptr);
  // Nothing is registered, so the model has no source environment for this set.
  ASSERT_EQ(model->getJointTrajectorySet(bare_index).getEnvironment(), nullptr);

  select(widget, owner_index);
  select(widget, bare_index);
  slider(widget).setValue(slider(widget).maximum());
  EXPECT_NEAR(jointValue(*env, "j1"), 0.0, 1e-6);
}

TEST_F(JointTrajectoryWidgetUnit, ReselectingSetAfterEnvironmentlessSelectionDoesNotReregister)  // NOLINT
{
  constexpr int kRegistrationDelayMarginMs = 300;

  auto parent = ComponentInfoManager::create(SCENE_NAME);
  JointTrajectoryWidget widget(ComponentInfoManager::createChild(parent));
  auto model = widget.getModel();

  JointTrajectorySet owns_environment(makeEnvironment());
  owns_environment.appendJointTrajectory(makeMotion("j2"));
  const JointTrajectorySet without_environment = makeMotionSet("j1");
  addSet(widget.getComponentInfo(), owns_environment);
  addSet(widget.getComponentInfo(), without_environment);
  const QModelIndex owner_index = setIndex(*model, owns_environment.getUUID());
  const QModelIndex bare_index = setIndex(*model, without_environment.getUUID());
  ASSERT_TRUE(owner_index.isValid());
  ASSERT_TRUE(bare_index.isValid());

  select(widget, owner_index);
  spin(kRegistrationDelayMarginMs);
  auto first_wrapper = EnvironmentManager::get(widget.getComponentInfo());
  ASSERT_NE(first_wrapper, nullptr);

  select(widget, bare_index);
  spin(kRegistrationDelayMarginMs);

  select(widget, owner_index);
  spin(kRegistrationDelayMarginMs);
  auto second_wrapper = EnvironmentManager::get(widget.getComponentInfo());
  EXPECT_EQ(second_wrapper, first_wrapper);
}

TEST_F(JointTrajectoryWidgetUnit, SelectingSetWithoutEnvironmentReportsStatusOnce)  // NOLINT
{
  auto parent = ComponentInfoManager::create(SCENE_NAME);
  JointTrajectoryWidget widget(ComponentInfoManager::createChild(parent));
  auto model = widget.getModel();

  JointTrajectorySet owns_environment(makeEnvironment());
  owns_environment.appendJointTrajectory(makeMotion("j2"));
  const JointTrajectorySet without_environment = makeMotionSet("j1");
  addSet(widget.getComponentInfo(), owns_environment);
  addSet(widget.getComponentInfo(), without_environment);
  const QModelIndex owner_index = setIndex(*model, owns_environment.getUUID());
  const QModelIndex bare_index = setIndex(*model, without_environment.getUUID());
  ASSERT_TRUE(owner_index.isValid());
  ASSERT_TRUE(bare_index.isValid());

  StatusLogWarnSpy spy;
  select(widget, owner_index);
  EXPECT_EQ(spy.messages.size(), 0U);

  select(widget, bare_index);
  EXPECT_EQ(spy.messages.size(), 1U);
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  qputenv("QT_QPA_PLATFORM", "offscreen");
  QApplication app(argc, argv);
  return RUN_ALL_TESTS();
}
