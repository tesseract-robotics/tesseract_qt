/*
 * Copyright (C) 2017 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <tesseract_qt/rendering/render_widget.h>
#include <tesseract_qt/rendering/render_events.h>
#include <tesseract_qt/rendering/conversions.h>
#include <tesseract_qt/rendering/interactive_view_control.h>

// TODO(louise) Remove these pragmas once ign-rendering
// is disabling the warnings
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#include <algorithm>
#include <map>
#include <sstream>
#include <vector>
#include <gz/rendering/Material.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/RayQuery.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>
#include <gz/rendering/Grid.hh>
#include <gz/rendering/AxisVisual.hh>
#include <gz/common/KeyEvent.hh>
#include <gz/common/MouseEvent.hh>
#include <gz/common/Console.hh>
#include <gz/math/Color.hh>

#include <QOpenGLTextureBlitter>
#include <QOffscreenSurface>
#include <QApplication>
#include <QTimer>
#include <QKeyEvent>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace tesseract_gui
{
/** @brief Private data class for Renderer */
class Renderer::Implementation
{
public:
  /** @brief Render engine to use */
  std::string engine_name{ "ogre2" };

  /** @brief Unique scene name */
  std::string scene_name{ "scene" };

  /** @brief Scene background color */
  gz::math::Color background_color{ 0.8f, 0.8f, 0.8f, 1.0f };

  /** @brief Ambient color */
  gz::math::Color ambient_light{ 0.4f, 0.4f, 0.4f, 1.0f };

  /** @brief Initial Camera pose */
  gz::math::Pose3d camera_pose{ -6, 0, 6, 0, 0.5, 0 };

  /** @briefDefault camera near clipping plane distance */
  double camera_near_clip{ 0.01 };

  /** @brief Default camera far clipping plane distance */
  double camera_far_clip{ 1000.0 };

  /**
   * @brief Set the level of anti-aliasing used during rendering.
   * @details If a value of 0 is given, no anti-aliasing will be performed. Higher values can significantly
   * slow-down rendering times, depending on the underlying render engine.
   */
  unsigned int camera_anti_aliasing{ 8 };

  /** @brief True if sky is enabled */
  bool sky_enable{ false };

  /** @brief True if grid is enabled */
  bool grid_enable{ true };

  /** @brief True if shadows is enabled */
  bool shadows_enable{ false };

  /**
   * @brief True if hover event is enabled
   * @details This is an expensive operation so only enable if needed
   */
  bool hover_event_enable{ false };

  /** @brief True if engine has been initialized */
  bool initialized{ false };

  /** @brief Render texture size */
  QSize texture_size{ 1920, 1200 };

  /** @brief Render texture opengl id */
  unsigned int texture_id{ 0 };

  /** @brief Flag to indicate texture size has changed. */
  bool texture_dirty{ false };

  ///////////////////////////////////////////////////////

  /** @brief Flag to indicate if mouse event is dirty */
  bool mouse_dirty{ false };

  /** @brief Flag to indicate if hover event is dirty */
  bool hover_dirty{ false };

  /** @brief Flag to indicate if drop event is dirty */
  bool drop_dirty{ false };

  /** @brief Mouse event */
  gz::common::MouseEvent mouse_event;

  /** @brief A list of mouse events */
  std::list<gz::common::MouseEvent> mouse_events;

  /** @brief Key event */
  gz::common::KeyEvent key_event;

  /**
   * @brief Max number of mouse events to store in the queue.
   * These events are then propagated to other gui plugins. A queue is used
   * instead of just keeping the latest mouse event so that we can capture
   * important events like mouse presses. However we keep the queue size
   * small on purpose so that we do not flood other gui plugins with events
   * that may be outdated.
   */
  const unsigned int kMaxMouseEventSize = 5u;

  /** @brief Mutex to protect mouse events */
  std::mutex mutex;

  /** @brief User camera */
  gz::rendering::CameraPtr camera{ nullptr };

  /** @brief User light */
  gz::rendering::DirectionalLightPtr light{ nullptr };

  /** @brief User grid */
  gz::rendering::VisualPtr grid{ nullptr };

  /** @brief The currently hovered mouse position in screen coordinates */
  gz::math::Vector2i mouse_hover_pos{ gz::math::Vector2i::Zero };

  /** @brief The currently drop mouse position in screen coordinates */
  gz::math::Vector2i mouse_drop_pos{ gz::math::Vector2i::Zero };

  /** @brief The dropped text in the scene */
  std::string drop_text;

  /** @brief Ray query for mouse clicks */
  gz::rendering::RayQueryPtr ray_query{ nullptr };

  gz::rendering::ScenePtr getScene() const
  {
    auto* engine = gz::rendering::engine(engine_name);
    if (engine == nullptr)
      return nullptr;

    return engine->SceneByName(scene_name);
  }
};

/////////////////////////////////////////////////
Renderer::Renderer() : data_(std::make_unique<Implementation>()) {}

void Renderer::setEngineName(const std::string& name)
{
  if (data_->initialized)
    throw std::runtime_error("RenderWidget, can not change engine name after initialization!");

  data_->engine_name = name;
}

const std::string& Renderer::getEngineName() const { return data_->engine_name; }

void Renderer::setSceneName(const std::string& name)
{
  if (data_->initialized)
    throw std::runtime_error("RenderWidget, can not change scene name after initialization!");

  data_->scene_name = name;
}

const std::string& Renderer::getSceneName() const { return data_->scene_name; }

void Renderer::setBackgroundColor(const gz::math::Color& color)
{
  data_->background_color = color;

  if (!data_->initialized)
    return;

  auto scene = data_->getScene();
  if (!scene)
    return;

  scene->SetBackgroundColor(color);
}
gz::math::Color Renderer::getBackgroundColor() const { return data_->background_color; }

void Renderer::setAmbientLight(const gz::math::Color& color)
{
  data_->ambient_light = color;

  if (!data_->initialized)
    return;

  auto scene = data_->getScene();
  if (!scene)
    return;

  scene->SetAmbientLight(color);
}

gz::math::Color Renderer::getAmbientLight() const { return data_->ambient_light; }

void Renderer::setInitialCameraPose(const gz::math::Pose3d& camera_pose) { data_->camera_pose = camera_pose; }

gz::math::Pose3d Renderer::getInitialCameraPose() const { return data_->camera_pose; }

gz::math::Pose3d Renderer::getCameraPose() const
{
  if (!data_->initialized)
    return data_->camera_pose;

  return data_->camera->LocalPose();
}

void Renderer::setCameraNearClipDistance(double near)
{
  data_->camera_near_clip = near;

  if (!data_->initialized)
    return;

  data_->camera->SetNearClipPlane(near);
}

double Renderer::getCameraNearClipDistance() const { return data_->camera_near_clip; }

void Renderer::setCameraFarClipDistance(double far)
{
  data_->camera_far_clip = far;

  if (!data_->initialized)
    return;

  data_->camera->SetFarClipPlane(far);
}

double Renderer::getCameraFarClipDistance() const { return data_->camera_far_clip; }

void Renderer::setCameraAntiAliasing(unsigned int value)
{
  data_->camera_anti_aliasing = value;

  if (!data_->initialized)
    return;

  data_->camera->SetAntiAliasing(value);
}

unsigned int Renderer::getCameraAntiAliasing() const { return data_->camera_anti_aliasing; }

void Renderer::setSkyEnabled(bool enabled)
{
  data_->sky_enable = enabled;

  if (!data_->initialized)
    return;

  auto scene = data_->getScene();
  if (!scene)
    return;

  scene->SetSkyEnabled(enabled);
}

bool Renderer::skyEnabled() const { return data_->sky_enable; }

void Renderer::setGridEnabled(bool enabled)
{
  data_->grid_enable = enabled;

  if (!data_->initialized)
    return;

  data_->grid->SetVisible(enabled);
}

bool Renderer::gridEnabled() const { return data_->grid_enable; }

void Renderer::setShadowsEnabled(bool enabled)
{
  data_->shadows_enable = enabled;

  if (!data_->initialized)
    return;

  data_->light->SetCastShadows(enabled);
}

bool Renderer::shadowsEnabled() const { return data_->shadows_enable; }

void Renderer::setHoverEventEnabled(bool enabled)
{
  std::lock_guard<std::mutex> lock(data_->mutex);
  data_->hover_event_enable = enabled;
}

bool Renderer::hoverEventEnabled() const
{
  std::lock_guard<std::mutex> lock(data_->mutex);
  return data_->hover_event_enable;
}

const QSize& Renderer::getTextureSize() const { return data_->texture_size; }

unsigned int Renderer::getTextureId() const { return data_->texture_id; }

/////////////////////////////////////////////////
void Renderer::render()
{
  if (!data_->initialized)
    return;

  if (data_->texture_dirty)
  {
    std::lock_guard<std::mutex> lock(data_->mutex);
    data_->camera->SetImageWidth(data_->texture_size.width());
    data_->camera->SetImageHeight(data_->texture_size.height());
    data_->camera->SetAspectRatio(static_cast<double>(data_->texture_size.width()) /
                                  static_cast<double>(data_->texture_size.height()));
    // setting the size should cause the render texture to be rebuilt
    data_->camera->PreRender();
    data_->texture_id = data_->camera->RenderTextureGLId();
    data_->texture_dirty = false;
  }

  // view control
  handleMouseEvent();

  if (qApp != nullptr)
  {
    events::PreRender event(data_->scene_name);
    QApplication::sendEvent(qApp, &event);
  }

  {
    std::lock_guard<std::mutex> lock(data_->mutex);
    data_->camera->Update();
    assert(data_->texture_id == data_->camera->RenderTextureGLId());
  }

  if (qApp != nullptr)
  {
    events::Render event(data_->scene_name);
    QApplication::sendEvent(qApp, &event);
  }

  return;
}

void Renderer::resize(int width, int height)
{
  std::lock_guard<std::mutex> lock(data_->mutex);
  data_->texture_size = QSize(width, height);
  data_->texture_dirty = true;
}

/////////////////////////////////////////////////
void Renderer::handleMouseEvent()
{
  std::lock_guard<std::mutex> lock(data_->mutex);
  for (const auto& e : data_->mouse_events)
  {
    data_->mouse_event = e;

    broadcastDrag();
    broadcastMousePress();
    broadcastLeftClick();
    broadcastRightClick();
    broadcastScroll();
    broadcastKeyPress();
    broadcastKeyRelease();
  }
  data_->mouse_events.clear();

  broadcastHoverPos();
  broadcastDrop();
  data_->mouse_dirty = false;
}

////////////////////////////////////////////////
void Renderer::handleKeyPress(const gz::common::KeyEvent& event)
{
  std::lock_guard<std::mutex> lock(data_->mutex);

  data_->key_event = event;

  data_->mouse_event.SetControl(data_->key_event.Control());
  data_->mouse_event.SetShift(data_->key_event.Shift());
  data_->mouse_event.SetAlt(data_->key_event.Alt());
}

////////////////////////////////////////////////
void Renderer::handleKeyRelease(const gz::common::KeyEvent& event)
{
  std::lock_guard<std::mutex> lock(data_->mutex);

  data_->key_event = event;

  data_->mouse_event.SetControl(data_->key_event.Control());
  data_->mouse_event.SetShift(data_->key_event.Shift());
  data_->mouse_event.SetAlt(data_->key_event.Alt());
}

/////////////////////////////////////////////////
void Renderer::broadcastDrop()
{
  if (!data_->drop_dirty)
    return;
  events::DropOnScene dropOnSceneEvent(data_->drop_text, data_->mouse_drop_pos, data_->scene_name);
  QApplication::sendEvent(qApp, &dropOnSceneEvent);
  data_->drop_dirty = false;
}

/////////////////////////////////////////////////
void Renderer::broadcastHoverPos()
{
  if (!data_->hover_dirty)
    return;

  auto pos = screenToScene(data_->mouse_hover_pos);

  events::HoverToScene hoverToSceneEvent(pos, data_->scene_name);
  QApplication::sendEvent(qApp, &hoverToSceneEvent);

  gz::common::MouseEvent hoverMouseEvent = data_->mouse_event;
  hoverMouseEvent.SetPos(data_->mouse_hover_pos);
  hoverMouseEvent.SetDragging(false);
  hoverMouseEvent.SetType(gz::common::MouseEvent::MOVE);
  events::HoverOnScene hoverOnSceneEvent(hoverMouseEvent, data_->scene_name);
  QApplication::sendEvent(qApp, &hoverOnSceneEvent);

  data_->hover_dirty = false;
}

/////////////////////////////////////////////////
void Renderer::broadcastDrag()
{
  if (!data_->mouse_dirty)
    return;

  // Only broadcast drag if dragging
  if (!data_->mouse_event.Dragging())
    return;

  events::DragOnScene dragEvent(data_->mouse_event, data_->scene_name);
  QApplication::sendEvent(qApp, &dragEvent);
}

/////////////////////////////////////////////////
void Renderer::broadcastLeftClick()
{
  if (!data_->mouse_dirty)
    return;

  if (data_->mouse_event.Button() != gz::common::MouseEvent::LEFT ||
      data_->mouse_event.Type() != gz::common::MouseEvent::RELEASE)
    return;

  auto pos = screenToScene(data_->mouse_event.Pos());

  events::LeftClickToScene leftClickToSceneEvent(pos, data_->scene_name);
  QApplication::sendEvent(qApp, &leftClickToSceneEvent);

  events::LeftClickOnScene leftClickOnSceneEvent(data_->mouse_event, data_->scene_name);
  QApplication::sendEvent(qApp, &leftClickOnSceneEvent);
}

/////////////////////////////////////////////////
void Renderer::broadcastRightClick()
{
  if (!data_->mouse_dirty)
    return;

  if (data_->mouse_event.Button() != gz::common::MouseEvent::RIGHT ||
      data_->mouse_event.Type() != gz::common::MouseEvent::RELEASE)
    return;

  auto pos = screenToScene(data_->mouse_event.Pos());

  events::RightClickToScene rightClickToSceneEvent(pos, data_->scene_name);
  QApplication::sendEvent(qApp, &rightClickToSceneEvent);

  events::RightClickOnScene rightClickOnSceneEvent(data_->mouse_event, data_->scene_name);
  QApplication::sendEvent(qApp, &rightClickOnSceneEvent);
}

/////////////////////////////////////////////////
void Renderer::broadcastMousePress()
{
  if (!data_->mouse_dirty)
    return;

  if (data_->mouse_event.Type() != gz::common::MouseEvent::PRESS)
    return;

  events::MousePressOnScene event(data_->mouse_event, data_->scene_name);
  QApplication::sendEvent(qApp, &event);
}

/////////////////////////////////////////////////
void Renderer::broadcastScroll()
{
  if (!data_->mouse_dirty)
    return;

  if (data_->mouse_event.Type() != gz::common::MouseEvent::SCROLL)
    return;

  events::ScrollOnScene scrollOnSceneEvent(data_->mouse_event, data_->scene_name);
  QApplication::sendEvent(qApp, &scrollOnSceneEvent);
}

/////////////////////////////////////////////////
void Renderer::broadcastKeyRelease()
{
  if (data_->key_event.Type() != gz::common::KeyEvent::RELEASE)
    return;

  events::KeyReleaseOnScene keyRelease(data_->key_event, data_->scene_name);
  QApplication::sendEvent(qApp, &keyRelease);

  data_->key_event.SetType(gz::common::KeyEvent::NO_EVENT);
}

/////////////////////////////////////////////////
void Renderer::broadcastKeyPress()
{
  if (data_->key_event.Type() != gz::common::KeyEvent::PRESS)
    return;

  events::KeyPressOnScene keyPress(data_->key_event, data_->scene_name);
  QApplication::sendEvent(qApp, &keyPress);

  data_->key_event.SetType(gz::common::KeyEvent::NO_EVENT);
}

/////////////////////////////////////////////////
bool Renderer::isInitialized() const { return data_->initialized; }

/////////////////////////////////////////////////
void Renderer::initialize()
{
  if (data_->initialized)
    return;

  std::lock_guard<std::mutex> lock(data_->mutex);
  std::map<std::string, std::string> params;
  // Ensure that the Qt application and Ogre2 share an OpenGL context
  params["useCurrentGLContext"] = "1";
  auto* engine = gz::rendering::engine(data_->engine_name, params);
  if (engine == nullptr)
  {
    ignerr << "Engine [" << data_->engine_name << "] is not supported" << std::endl;
    return;
  }

  // Scene
  auto scene = engine->SceneByName(data_->scene_name);
  if (!scene)
  {
    igndbg << "Create scene [" << data_->scene_name << "]" << std::endl;
    scene = engine->CreateScene(data_->scene_name);
    scene->SetAmbientLight(data_->ambient_light);
    scene->SetBackgroundColor(data_->background_color);
  }

  scene->SetSkyEnabled(data_->sky_enable);

  data_->grid = scene->VisualByName("tesseract_grid");
  if (data_->grid == nullptr)
  {
    gz::rendering::VisualPtr root = scene->RootVisual();

    // create gray material
    gz::rendering::MaterialPtr gray = scene->CreateMaterial();
    gray->SetAmbient(0.7, 0.7, 0.7);
    gray->SetDiffuse(0.7, 0.7, 0.7);
    gray->SetSpecular(0.7, 0.7, 0.7);

    // create grid visual
    unsigned id = 1000;  // static_cast<unsigned>(dataPtr->entity_manager.addVisual("tesseract_grid"));
    data_->grid = scene->CreateVisual(id, "tesseract_grid");
    gz::rendering::GridPtr gridGeom = scene->CreateGrid();
    if (!gridGeom)
    {
      ignwarn << "Failed to create grid for scene [" << scene->Name() << "] on engine [" << scene->Engine()->Name()
              << "]" << std::endl;
      return;
    }
    gridGeom->SetCellCount(20);
    gridGeom->SetCellLength(1);
    gridGeom->SetVerticalCellCount(0);
    data_->grid->AddGeometry(gridGeom);
    data_->grid->SetLocalPosition(0, 0, 0.015);
    data_->grid->SetMaterial(gray);
    root->AddChild(data_->grid);
  }

  data_->grid->SetVisible(data_->grid_enable);

  auto root = scene->RootVisual();

  // Camera
  data_->camera = scene->CreateCamera();
  data_->camera->SetUserData("user-camera", true);
  root->AddChild(data_->camera);
  data_->camera->SetLocalPose(data_->camera_pose);
  data_->camera->SetNearClipPlane(data_->camera_near_clip);
  data_->camera->SetFarClipPlane(data_->camera_far_clip);
  data_->camera->SetImageWidth(data_->texture_size.width());
  data_->camera->SetImageHeight(data_->texture_size.height());
  data_->camera->SetAntiAliasing(data_->camera_anti_aliasing);
  data_->camera->SetHFOV(M_PI * 0.5);

  /** @todo LEVI Need figure out haw to get the camera by name. */
  // create directional light
  data_->light = scene->CreateDirectionalLight();
  data_->light->SetDirection(1, 0, 0);
  data_->light->SetDiffuseColor(0.8, 0.8, 0.8);
  data_->light->SetSpecularColor(0.5, 0.5, 0.5);
  data_->light->SetCastShadows(data_->shadows_enable);
  data_->camera->AddChild(data_->light);

  // Generate initial texture
  data_->camera->PreRender();
  data_->texture_id = data_->camera->RenderTextureGLId();
  data_->camera->Update();

  // Ray Query
  data_->ray_query = data_->camera->Scene()->CreateRayQuery();

  data_->initialized = true;
}

/////////////////////////////////////////////////
void Renderer::destroy()
{
  auto* engine = gz::rendering::engine(data_->engine_name);
  if (engine == nullptr)
    return;
  auto scene = engine->SceneByName(data_->scene_name);
  if (!scene)
    return;
  scene->DestroySensor(data_->camera);

  // If that was the last sensor, destroy scene
  if (scene->SensorCount() == 0)
  {
    igndbg << "Destroy scene [" << scene->Name() << "]" << std::endl;
    engine->DestroyScene(scene);

    // If that was the last scene, terminate engine
    if (engine->SceneCount() == 0)
      gz::rendering::unloadEngine(data_->engine_name);
  }
}

/////////////////////////////////////////////////
void Renderer::newHoverEvent(const gz::math::Vector2i& hover_pos)
{
  std::lock_guard<std::mutex> lock(data_->mutex);
  if (data_->hover_event_enable)
  {
    data_->mouse_hover_pos = hover_pos;
    data_->hover_dirty = true;
  }
}

/////////////////////////////////////////////////
void Renderer::newDropEvent(const std::string& drop_text, const gz::math::Vector2i& drop_pos)
{
  std::lock_guard<std::mutex> lock(data_->mutex);
  data_->drop_text = drop_text;
  data_->mouse_drop_pos = drop_pos;
  data_->drop_dirty = true;
}

/////////////////////////////////////////////////
void Renderer::newMouseEvent(const gz::common::MouseEvent& event)
{
  std::lock_guard<std::mutex> lock(data_->mutex);
  if (data_->mouse_events.size() >= data_->kMaxMouseEventSize)
    data_->mouse_events.pop_front();

  data_->mouse_events.push_back(event);
  data_->mouse_dirty = true;
}

/////////////////////////////////////////////////
gz::math::Vector3d Renderer::screenToScene(const gz::math::Vector2i& screen_pos) const
{
  // TODO(ahcorde): Replace this code with function in ign-rendering
  // Require this commit
  // https://github.com/ignitionrobotics/ign-rendering/pull/363
  // in ign-rendering6

  // Normalize point on the image
  double width = data_->camera->ImageWidth();
  double height = data_->camera->ImageHeight();

  double nx = 2.0 * screen_pos.X() / width - 1.0;
  double ny = 1.0 - 2.0 * screen_pos.Y() / height;

  // Make a ray query
  data_->ray_query->SetFromCamera(data_->camera, gz::math::Vector2d(nx, ny));

  auto result = data_->ray_query->ClosestPoint();
  if (result)
    return result.point;

  // Set point to be 10m away if no intersection found
  return data_->ray_query->Origin() + data_->ray_query->Direction() * 10;
}

/// \brief Private data class for RenderWindowItem
class RenderWidget::Implementation
{
public:
  /** @brief Keep latest mouse event */
  gz::common::MouseEvent mouseEvent;

  /** @brief The rendering renderer */
  Renderer renderer;

  /** @brief The view controller */
  InteractiveViewControl::Ptr view_controller;

  /** @brief The texture blitter to draw the texture on the widget */
  QOpenGLTextureBlitter texture_blitter;

  /** @brief The active render texture id */
  unsigned int texture_id{ 0 };

  /** @brief The offscreen surface used by the render engine */
  QOffscreenSurface* surface{ nullptr };

  /** @brief The shared context used by the render engine */
  QOpenGLContext* context{ nullptr };

  /** @brief This calls update on a timer */
  QTimer update_timer;

  /** @brief The desired update frequency for rendering, default is 60hz */
  int update_frequency{ 60 };

  /** @brief List of our QT connections. */
  QList<QMetaObject::Connection> connections;
};

/////////////////////////////////////////////////
RenderWidget::RenderWidget(const std::string& scene_name, const std::string& engine_name, QWidget* parent)
  : QOpenGLWidget(parent), data_(std::make_unique<Implementation>())
{
  data_->renderer.setEngineName(engine_name);
  data_->renderer.setSceneName(scene_name);
  data_->view_controller = std::make_shared<InteractiveViewControl>(scene_name);

  connect(&data_->update_timer, SIGNAL(timeout()), this, SLOT(onFrameSwapped()));
  data_->update_timer.start(1000.0 / data_->update_frequency);

  // The use of QTimer is prefered over the method below
  //  // This is critical to sync the monitor refresh with rendering of the widget
  //  connect(this, SIGNAL(frameSwapped()), this, SLOT(onFrameSwapped()));
}

/////////////////////////////////////////////////
RenderWidget::~RenderWidget()
{
  data_->context->makeCurrent(data_->surface);

  // Destroy renderer
  data_->renderer.destroy();

  data_->context->doneCurrent();

  // schedule this to be deleted only after we're done cleaning up
  data_->surface->deleteLater();

  delete data_->context;
  data_->context = nullptr;

  // Disconnect our QT connections.
  for (const auto& conn : data_->connections)
    QObject::disconnect(conn);
}

void RenderWidget::initializeGL()
{
  initializeOpenGLFunctions();

  // Create the texture blitter
  data_->texture_blitter.create();

  {  // Initialize renderer
    // Some GL implementations requres that the currently bound context is
    // made non-current before we set up sharing, so we doneCurrent here
    // and makeCurrent down below while setting up our own context.
    doneCurrent();

    data_->context = new QOpenGLContext();

    // set the surface format (this is managed globally in Main.cpp)
    // auto surfaceFormat = RenderThread::createSurfaceFormat();
    // m_renderThread->context->setFormat(surfaceFormat);
    data_->context->setFormat(context()->format());

    data_->context->setShareContext(context());
    data_->context->create();

    // Run on the Main (GUI = QML) thread
    data_->surface = new QOffscreenSurface();
    data_->surface->setFormat(data_->context->format());
    data_->surface->create();

    // carry out any initialisation before moving to thread
    data_->context->makeCurrent(data_->surface);

    // create renderer
    if (!data_->renderer.isInitialized())
      data_->renderer.initialize();

    data_->context->doneCurrent();
  }

  // check if engine has been successfully initialized
  if (!data_->renderer.isInitialized())
  {
    ignerr << "Unable to initialize renderer" << std::endl;
  }
}

void RenderWidget::resizeGL(int w, int h) { data_->renderer.resize(w, h); }

/////////////////////////////////////////////////
void RenderWidget::paintGL()
{
  data_->context->makeCurrent(data_->surface);
  data_->renderer.render();
  data_->context->doneCurrent();

  makeCurrent();
  data_->texture_blitter.bind(GL_TEXTURE_2D);
  const QRect target_rect(QPoint(0, 0), size());
  const QRect viewport_rect = QRect(QPoint(0, 0), data_->renderer.getTextureSize());
  QMatrix4x4 target = QOpenGLTextureBlitter::targetTransform(target_rect, viewport_rect);
  data_->texture_blitter.blit(data_->renderer.getTextureId(), target, QOpenGLTextureBlitter::OriginTopLeft);
  data_->texture_blitter.release();
  doneCurrent();
}

/////////////////////////////////////////////////
void RenderWidget::setBackgroundColor(const gz::math::Color& color) { data_->renderer.setBackgroundColor(color); }
gz::math::Color RenderWidget::getBackgroundColor() const { return data_->renderer.getBackgroundColor(); }

/////////////////////////////////////////////////
void RenderWidget::setAmbientLight(const gz::math::Color& ambient) { data_->renderer.setAmbientLight(ambient); }
gz::math::Color RenderWidget::getAmbientLight() const { return data_->renderer.getAmbientLight(); }

/////////////////////////////////////////////////
void RenderWidget::setEngineName(const std::string& name) { data_->renderer.setEngineName(name); }
const std::string& RenderWidget::getEngineName() const { return data_->renderer.getEngineName(); }

/////////////////////////////////////////////////
void RenderWidget::setSceneName(const std::string& name) { data_->renderer.setSceneName(name); }
const std::string& RenderWidget::getSceneName() const { return data_->renderer.getSceneName(); }

/////////////////////////////////////////////////
void RenderWidget::setInitialCameraPose(const gz::math::Pose3d& pose) { data_->renderer.setInitialCameraPose(pose); }
gz::math::Pose3d RenderWidget::getInitialCameraPose() const { return data_->renderer.getInitialCameraPose(); }

/////////////////////////////////////////////////
void RenderWidget::setCameraNearClip(double near) { data_->renderer.setCameraNearClipDistance(near); }
double RenderWidget::getCameraNearClip() const { return data_->renderer.getCameraNearClipDistance(); }

/////////////////////////////////////////////////
void RenderWidget::setCameraFarClip(double far) { data_->renderer.setCameraFarClipDistance(far); }
double RenderWidget::getCameraFarClip() const { return data_->renderer.getCameraFarClipDistance(); }

/////////////////////////////////////////////////
void RenderWidget::setCameraAntiAliasing(unsigned int value) { data_->renderer.setCameraAntiAliasing(value); }
unsigned int RenderWidget::getCameraAntiAliasing() const { return data_->renderer.getCameraAntiAliasing(); }

/////////////////////////////////////////////////
void RenderWidget::setSkyEnabled(bool enabled) { data_->renderer.setSkyEnabled(enabled); }
bool RenderWidget::skyEnabled() const { return data_->renderer.skyEnabled(); }

/////////////////////////////////////////////////
void RenderWidget::setGridEnabled(bool enabled) { data_->renderer.setGridEnabled(enabled); }
bool RenderWidget::gridEnabled() const { return data_->renderer.gridEnabled(); }

/////////////////////////////////////////////////
void RenderWidget::setShadowsEnabled(bool enabled) { data_->renderer.setShadowsEnabled(enabled); }
bool RenderWidget::shadowsEnabled() const { return data_->renderer.shadowsEnabled(); }

/////////////////////////////////////////////////
void RenderWidget::setUpdateFrequency(double hz)
{
  data_->update_frequency = hz;
  data_->update_timer.start(1000.0 / hz);
}
double RenderWidget::getUpdateFrequency() const { return data_->update_frequency; }

/////////////////////////////////////////////////
// void MinimalScene::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
//{
//  RenderWindowItem *renderWindow =
//      PluginItem()->findChild<RenderWindowItem *>();
//  if (!renderWindow)
//  {
//    ignerr << "Unable to find Render Window item. "
//           << "Render window will not be created" << std::endl;
//    return;
//  }

//  if (title.empty())
//    title = "3D Scene";

//  std::string cmdRenderEngine = gz::gui::renderEngineName();
//  // Custom parameters
//  if (_pluginElem)
//  {
//    // Only pick engine from XML if none is set on the Window
//    if (cmdRenderEngine.empty())
//    {
//      auto elem = _pluginElem->FirstChildElement("engine");
//      if (nullptr != elem && nullptr != elem->GetText())
//      {
//        cmdRenderEngine = elem->GetText();
//      }
//    }

//    auto elem = _pluginElem->FirstChildElement("scene");
//    if (nullptr != elem && nullptr != elem->GetText())
//      renderWindow->SetSceneName(elem->GetText());

//    elem = _pluginElem->FirstChildElement("ambient_light");
//    if (nullptr != elem && nullptr != elem->GetText())
//    {
//      math::Color ambient;
//      std::stringstream colorStr;
//      colorStr << std::string(elem->GetText());
//      colorStr >> ambient;
//      renderWindow->SetAmbientLight(ambient);
//    }

//    elem = _pluginElem->FirstChildElement("background_color");
//    if (nullptr != elem && nullptr != elem->GetText())
//    {
//      math::Color bgColor;
//      std::stringstream colorStr;
//      colorStr << std::string(elem->GetText());
//      colorStr >> bgColor;
//      renderWindow->SetBackgroundColor(bgColor);
//    }

//    elem = _pluginElem->FirstChildElement("camera_pose");
//    if (nullptr != elem && nullptr != elem->GetText())
//    {
//      math::Pose3d pose;
//      std::stringstream poseStr;
//      poseStr << std::string(elem->GetText());
//      poseStr >> pose;
//      renderWindow->SetCameraPose(pose);
//    }

//    elem = _pluginElem->FirstChildElement("camera_clip");
//    if (nullptr != elem && !elem->NoChildren())
//    {
//      auto child = elem->FirstChildElement("near");
//      if (nullptr != child && nullptr != child->GetText())
//      {
//        double n;
//        std::stringstream nearStr;
//        nearStr << std::string(child->GetText());
//        nearStr >> n;
//        if (nearStr.fail())
//        {
//          ignerr << "Unable to set <near> to '" << nearStr.str()
//                 << "' using default near clip distance" << std::endl;
//        }
//        else
//        {
//          renderWindow->SetCameraNearClip(n);
//        }
//      }

//      child = elem->FirstChildElement("far");
//      if (nullptr != child && nullptr != child->GetText())
//      {
//        double f;
//        std::stringstream farStr;
//        farStr << std::string(child->GetText());
//        farStr >> f;
//        if (farStr.fail())
//        {
//          ignerr << "Unable to set <far> to '" << farStr.str()
//                 << "' using default far clip distance" << std::endl;
//        }
//        else
//        {
//          renderWindow->SetCameraFarClip(f);
//        }
//      }
//    }

//    elem = _pluginElem->FirstChildElement("sky");
//    if (nullptr != elem && nullptr != elem->GetText())
//    {
//      renderWindow->SetSkyEnabled(true);
//      if (!elem->NoChildren())
//        ignwarn << "Child elements of <sky> are not supported yet" << std::endl;
//    }
//  }

//  renderWindow->SetEngineName(cmdRenderEngine);
//  // there is a problem with displaying ogre2 render textures that are in
//  // sRGB format. Workaround for now is to apply gamma correction
//  // manually.
//  // There maybe a better way to solve the problem by making OpenGL calls.
//  if (cmdRenderEngine == std::string("ogre2"))
//    PluginItem()->setProperty("gammaCorrect", true);
//}

/////////////////////////////////////////////////
void RenderWidget::onHovered(int mouse_x, int mouse_y) { data_->renderer.newHoverEvent({ mouse_x, mouse_y }); }

/////////////////////////////////////////////////
void RenderWidget::onDropped(const QString& drop, int mouse_x, int mouse_y)
{
  data_->renderer.newDropEvent(drop.toStdString(), { mouse_x, mouse_y });
}

/////////////////////////////////////////////////

void RenderWidget::onFrameSwapped()
{
  // Weird issue between QFileDialog and update being called, QFileDialogs are launched as application modality and this
  // check was added to prevent the issue.
  if (QApplication::activeModalWidget() == nullptr)
    update();
}

/////////////////////////////////////////////////
void RenderWidget::mousePressEvent(QMouseEvent* event)
{
  data_->mouseEvent = convert(*event);
  data_->mouseEvent.SetPressPos(data_->mouseEvent.Pos());

  data_->renderer.newMouseEvent(data_->mouseEvent);
}

////////////////////////////////////////////////
void RenderWidget::keyPressEvent(QKeyEvent* event)
{
  if (event->isAutoRepeat())
    return;

  auto e = convert(*event);
  handleKeyPress(e);
}

////////////////////////////////////////////////
void RenderWidget::keyReleaseEvent(QKeyEvent* event)
{
  if (event->isAutoRepeat())
    return;

  auto e = convert(*event);
  handleKeyPress(e);
}

////////////////////////////////////////////////
void RenderWidget::mouseReleaseEvent(QMouseEvent* event)
{
  // Store values that depend on previous events
  auto pressPos = data_->mouseEvent.PressPos();
  auto dragging = data_->mouseEvent.Dragging();

  data_->mouseEvent = convert(*event);
  data_->mouseEvent.SetPressPos(pressPos);
  data_->mouseEvent.SetDragging(dragging);

  data_->renderer.newMouseEvent(data_->mouseEvent);
}

////////////////////////////////////////////////
void RenderWidget::mouseMoveEvent(QMouseEvent* event)
{
  // Store values that depend on previous events
  auto pressPos = data_->mouseEvent.PressPos();

  data_->mouseEvent = convert(*event);

  if (data_->mouseEvent.Dragging())
    data_->mouseEvent.SetPressPos(pressPos);

  data_->renderer.newMouseEvent(data_->mouseEvent);
}

////////////////////////////////////////////////
void RenderWidget::wheelEvent(QWheelEvent* event)
{
  //  forceActiveFocus();

  data_->mouseEvent = convert(*event);
  data_->renderer.newMouseEvent(data_->mouseEvent);
}

////////////////////////////////////////////////
void RenderWidget::handleKeyPress(const gz::common::KeyEvent& event) { data_->renderer.handleKeyPress(event); }

////////////////////////////////////////////////
void RenderWidget::handleKeyRelease(const gz::common::KeyEvent& event) { data_->renderer.handleKeyRelease(event); }

}  // namespace tesseract_gui
