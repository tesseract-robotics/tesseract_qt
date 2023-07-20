/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

#include <string>
#include <mutex>
#include <QGuiApplication>

#include <gz/common/MouseEvent.hh>
#include <gz/common/Console.hh>

#include <gz/rendering/Camera.hh>
#include <gz/rendering/OrbitViewController.hh>
#include <gz/rendering/OrthoViewController.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/RayQuery.hh>
#include <gz/rendering/Utils.hh>

#include <tesseract_qt/rendering/render_events.h>
#include <tesseract_qt/rendering/gazebo_utils.h>
#include <tesseract_qt/rendering/interactive_view_control.h>

/** @brief Private data class for InteractiveViewControl */
class tesseract_gui::InteractiveViewControlPrivate
{
public:
  /** @brief Perform rendering calls in the rendering thread */
  void onRender();

  /** @brief Update the reference visual. Adjust scale based on distance from camera to target point so it remains the
   * same size on screen. */
  void updateReferenceVisual();

  /** @brief The scene key the control is associated with */
  std::string scene_name;

  /** @brief Flag to indicate if mouse event is dirty */
  bool mouse_dirty = false;

  /** @brief Flag to indicate if hover event is dirty */
  bool hover_dirty = false;

  /** @brief Flag to indicate if mouse press event is dirty */
  bool mouse_press_dirty = false;

  /** @brief True to block orbiting with the mouse */
  bool block_orbit = false;

  /** @brief Mouse event */
  gz::common::MouseEvent mouse_event;

  /** @brief Mouse move distance since last event */
  gz::math::Vector2d drag;

  /** @brief User camera */
  gz::rendering::CameraPtr camera{ nullptr };

  /** @brief View control focus target */
  gz::math::Vector3d target;

  /** @brief Orbit view controller */
  gz::rendering::OrbitViewController orbit_view_control;

  /** @brief Ortho view controller */
  gz::rendering::OrthoViewController ortho_view_control;

  /** @brief Camera view controller */
  gz::rendering::ViewController* view_control{ nullptr };

  /** @brief View controller */
  ViewControlType view_control_type{ ViewControlType::ORBIT };

  /** @brief Enable / disable reference visual */
  bool enable_ref_visual{ true };

  /** @brief Reference visual for visualizing the target point */
  gz::rendering::VisualPtr ref_visual{ nullptr };

  /** @brief Ray query for mouse clicks */
  gz::rendering::RayQueryPtr ray_query{ nullptr };

  /** @brief Pointer to the rendering scene */
  gz::rendering::ScenePtr scene{ nullptr };

  /** @brief Mutex to protect View Controllers */
  std::mutex mutex;
};

using namespace tesseract_gui;

/////////////////////////////////////////////////
void InteractiveViewControlPrivate::onRender()
{
  if (!scene)
  {
    scene = sceneFromFirstRenderEngine(scene_name);
    if (!scene)
      return;

    for (unsigned int i = 0; i < scene->NodeCount(); ++i)
    {
      auto cam = std::dynamic_pointer_cast<gz::rendering::Camera>(scene->NodeByIndex(i));
      if (cam)
      {
        bool isUserCamera = false;
        try
        {
          isUserCamera = std::get<bool>(cam->UserData("user-camera"));
        }
        catch (std::bad_variant_access&)
        {
          continue;
        }
        if (isUserCamera)
        {
          camera = cam;
          igndbg << "InteractiveViewControl plugin is moving camera [" << camera->Name() << "]" << std::endl;
          break;
        }
      }
    }

    if (!camera)
    {
      ignerr << "InteractiveViewControl camera is not available" << std::endl;
      return;
    }
    ray_query = camera->Scene()->CreateRayQuery();
  }

  if (block_orbit)
  {
    drag = { 0, 0 };
    return;
  }

  if (!camera)
    return;

  // hover
  if (hover_dirty)
  {
    if (ref_visual)
      ref_visual->SetVisible(false);
    hover_dirty = false;
  }

  if (!mouse_dirty)
    return;

  std::lock_guard<std::mutex> lock(mutex);

  if (view_control_type == ViewControlType::ORTHO)
    view_control = &ortho_view_control;
  else
    view_control = &orbit_view_control;

  view_control->SetCamera(camera);

  if (enable_ref_visual)
  {
    if (!ref_visual)
    {
      // create ref visual
      ref_visual = scene->CreateVisual();
      gz::rendering::GeometryPtr sphere = scene->CreateSphere();
      ref_visual->AddGeometry(sphere);
      ref_visual->SetLocalScale(gz::math::Vector3d(0.2, 0.2, 0.1));
      ref_visual->SetVisibilityFlags(GZ_VISIBILITY_GUI & ~GZ_VISIBILITY_SELECTABLE);

      // create material
      gz::math::Color diffuse(1.0f, 1.0f, 0.0f, 1.0f);
      gz::math::Color specular(1.0f, 1.0f, 0.0f, 1.0f);
      double transparency = 0.3;
      gz::rendering::MaterialPtr material = scene->CreateMaterial();
      material->SetDiffuse(diffuse);
      material->SetSpecular(specular);
      material->SetTransparency(transparency);
      material->SetCastShadows(false);
      ref_visual->SetMaterial(material);
      scene->DestroyMaterial(material);
    }
    ref_visual->SetVisible(true);
  }

  if (mouse_event.Type() == gz::common::MouseEvent::SCROLL)
  {
    target = gz::rendering::screenToScene(mouse_event.Pos(), camera, ray_query);

    view_control->SetTarget(target);
    const double distance = camera->WorldPosition().Distance(target);
    const double amount = -drag.Y() * distance / 20.0;
    view_control->Zoom(amount);
    updateReferenceVisual();
  }
  else if (mouse_event.Type() == gz::common::MouseEvent::PRESS)
  {
    // Do not updated reference target on middle button because it is annoying when trying to orbit around an object
    if (mouse_event.Button() != gz::common::MouseEvent::MIDDLE)
    {
      target = gz::rendering::screenToScene(mouse_event.PressPos(), camera, ray_query);
      view_control->SetTarget(target);
      updateReferenceVisual();
    }
    mouse_press_dirty = false;
  }
  else
  {
    // Pan with left button
    if (mouse_event.Buttons() & gz::common::MouseEvent::LEFT)
    {
      if (Qt::ShiftModifier == QGuiApplication::queryKeyboardModifiers())
        view_control->Orbit(drag);
      else
        view_control->Pan(drag);

      updateReferenceVisual();
    }
    // Orbit with middle button
    else if (mouse_event.Buttons() & gz::common::MouseEvent::MIDDLE)
    {
      view_control->Orbit(drag);
      updateReferenceVisual();
    }
    // Zoom with right button
    else if (mouse_event.Buttons() & gz::common::MouseEvent::RIGHT)
    {
      double hfov = camera->HFOV().Radian();
      double vfov = 2.0f * atan(tan(hfov / 2.0F) / camera->AspectRatio());
      double distance = camera->WorldPosition().Distance(target);
      double amount = ((-drag.Y() / static_cast<double>(camera->ImageHeight())) * distance * tan(vfov / 2.0) * 6.0);
      view_control->Zoom(amount);
      updateReferenceVisual();
    }
  }
  drag = 0;
  mouse_dirty = false;
}

/////////////////////////////////////////////////
void InteractiveViewControlPrivate::updateReferenceVisual()
{
  if (!ref_visual || !enable_ref_visual)
    return;
  ref_visual->SetWorldPosition(this->target);
  // Update the size of the reference visual based on the distance to the target point.
  double distance = camera->WorldPosition().Distance(this->target);
  double scale = distance * atan(GZ_DTOR(1.0));
  ref_visual->SetLocalScale(gz::math::Vector3d(scale, scale, scale * 0.5));
}

/////////////////////////////////////////////////
InteractiveViewControl::InteractiveViewControl(const std::string& scene_name, ViewControlType type)
  : data_(std::make_unique<InteractiveViewControlPrivate>())
{
  data_->scene_name = scene_name;
  data_->view_control_type = type;

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

/////////////////////////////////////////////////
InteractiveViewControl::~InteractiveViewControl() = default;

/////////////////////////////////////////////////
void InteractiveViewControl::setViewController(ViewControlType type)
{
  std::lock_guard<std::mutex> lock(data_->mutex);
  data_->view_control_type = type;
}

/////////////////////////////////////////////////
bool InteractiveViewControl::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::Render::kType)
  {
    assert(dynamic_cast<events::Render*>(event) != nullptr);
    auto* e = static_cast<events::Render*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->onRender();
    }
  }
  else if (event->type() == events::LeftClickOnScene::kType)
  {
    assert(dynamic_cast<events::LeftClickOnScene*>(event) != nullptr);
    auto* e = static_cast<events::LeftClickOnScene*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->mouse_dirty = true;
      data_->drag = gz::math::Vector2d::Zero;
      data_->mouse_event = e->getMouse();
    }
  }
  else if (event->type() == events::MousePressOnScene::kType)
  {
    assert(dynamic_cast<events::MousePressOnScene*>(event) != nullptr);
    auto* e = static_cast<events::MousePressOnScene*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->mouse_dirty = true;
      data_->mouse_press_dirty = true;
      data_->drag = gz::math::Vector2d::Zero;
      data_->mouse_event = e->getMouse();
    }
  }
  else if (event->type() == events::DragOnScene::kType)
  {
    if (data_->mouse_press_dirty)
      return QObject::eventFilter(obj, event);

    assert(dynamic_cast<events::DragOnScene*>(event) != nullptr);
    auto* e = reinterpret_cast<events::DragOnScene*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->mouse_dirty = true;

      auto dragStart = data_->mouse_event.Pos();
      auto dragInt = e->getMouse().Pos() - dragStart;
      auto dragDistance = gz::math::Vector2d(dragInt.X(), dragInt.Y());

      data_->drag += dragDistance;

      data_->mouse_event = e->getMouse();
    }
  }
  else if (event->type() == events::ScrollOnScene::kType)
  {
    assert(dynamic_cast<events::ScrollOnScene*>(event) != nullptr);
    auto* e = static_cast<events::ScrollOnScene*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->mouse_dirty = true;
      data_->drag += gz::math::Vector2d(e->getMouse().Scroll().X(), e->getMouse().Scroll().Y());
      data_->mouse_event = e->getMouse();
    }
  }
  else if (event->type() == events::BlockOrbit::kType)
  {
    assert(dynamic_cast<events::BlockOrbit*>(event) != nullptr);
    auto* e = static_cast<events::BlockOrbit*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->block_orbit = e->getBlock();
    }
  }
  else if (event->type() == events::HoverOnScene::kType)
  {
    data_->hover_dirty = true;
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
