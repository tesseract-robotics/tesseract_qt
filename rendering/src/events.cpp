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

#include <tesseract_qt/rendering/events.h>
#include <string>

namespace tesseract_gui::events
{
class Render::Implementation
{
public:
  /// The scene the event is associated with
  std::string scene_name;
};

class SnapIntervals::Implementation
{
public:
  /// \brief XYZ snapping values in meters, these values must be positive.
  ignition::math::Vector3d xyz;

  /// \brief RPY snapping values in degrees, these values must be
  /// positive.
  ignition::math::Vector3d rpy;

  /// \brief Scale snapping values - a multiplier of the current size,
  /// these values must be positive.
  ignition::math::Vector3d scale;

  /// The scene the event is associated with
  std::string scene_name;
};

class SpawnFromDescription::Implementation
{
public:
  /// \brief The string of the resource to be spawned.
  std::string description;

  /// The scene the event is associated with
  std::string scene_name;
};

class SpawnFromPath::Implementation
{
public:
  /// \brief The path of file to be previewed.
  std::string file_path;

  /// The scene the event is associated with
  std::string scene_name;
};

class HoverToScene::Implementation
{
public:
  /// \brief The 3D point over which the user is hovering.
  ignition::math::Vector3d point;

  /// The scene the event is associated with
  std::string scene_name;
};

class HoverOnScene::Implementation
{
public:
  /// \brief The 2D point over which the user is hovering.
  ignition::common::MouseEvent mouse;

  /// The scene the event is associated with
  std::string scene_name;
};

class LeftClickToScene::Implementation
{
public:
  /// \brief The 3D point that the user clicked within the scene.
  ignition::math::Vector3d point;

  /// The scene the event is associated with
  std::string scene_name;
};

class RightClickToScene::Implementation
{
public:
  /// \brief The 3D point that the user clicked within the scene.
  ignition::math::Vector3d point;

  /// The scene the event is associated with
  std::string scene_name;
};

class DropdownMenuEnabled::Implementation
{
public:
  /// \brief The boolean indicating whether the menu is disabled or not
  /// for this event.
  bool menu_enabled;

  /// The scene the event is associated with
  std::string scene_name;
};

class LeftClickOnScene::Implementation
{
public:
  /// \brief Mouse event
  ignition::common::MouseEvent mouse;

  /// The scene the event is associated with
  std::string scene_name;
};

class RightClickOnScene::Implementation
{
public:
  /// \brief Mouse event
  ignition::common::MouseEvent mouse;

  /// The scene the event is associated with
  std::string scene_name;
};

class BlockOrbit::Implementation
{
public:
  bool block;

  /// The scene the event is associated with
  std::string scene_name;
};

class KeyReleaseOnScene::Implementation
{
public:
  /// \brief Key event
  ignition::common::KeyEvent key;

  /// The scene the event is associated with
  std::string scene_name;
};

class KeyPressOnScene::Implementation
{
public:
  /// \brief Key event
  ignition::common::KeyEvent key;

  /// The scene the event is associated with
  std::string scene_name;
};

class SpawnCloneFromName::Implementation
{
public:
  /// \brief The name of the resource to be cloned
  std::string name;

  /// The scene the event is associated with
  std::string scene_name;
};

class DropOnScene::Implementation
{
public:
  /// \brief The name of the dropped thing
  std::string drop_text;

  /// \brief X and Y position of the mouse
  ignition::math::Vector2i mouse;

  /// The scene the event is associated with
  std::string scene_name;
};

class ScrollOnScene::Implementation
{
public:
  /// \brief Mouse event with scroll information.
  ignition::common::MouseEvent mouse;

  /// The scene the event is associated with
  std::string scene_name;
};

class DragOnScene::Implementation
{
public:
  /// \brief Mouse event with drag information.
  ignition::common::MouseEvent mouse;

  /// The scene the event is associated with
  std::string scene_name;
};

class MousePressOnScene::Implementation
{
public:
  /// \brief Mouse event with press information.
  ignition::common::MouseEvent mouse;

  /// The scene the event is associated with
  std::string scene_name;
};

class PreRender::Implementation
{
public:
  /// The scene the event is associated with
  std::string scene_name;
};

/////////////////////////////////////////////////
Render::Render(const std::string& scene_name) : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->scene_name = scene_name;
}
Render::~Render() = default;

/////////////////////////////////////////////////
const std::string& Render::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
SnapIntervals::SnapIntervals(const ignition::math::Vector3d& _xyz,
                             const ignition::math::Vector3d& _rpy,
                             const ignition::math::Vector3d& _scale,
                             const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->xyz = _xyz;
  data_->rpy = _rpy;
  data_->scale = _scale;
  data_->scene_name = scene_name;
}

SnapIntervals::~SnapIntervals() = default;

/////////////////////////////////////////////////
const std::string& SnapIntervals::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
ignition::math::Vector3d SnapIntervals::getPosition() const { return data_->xyz; }

/////////////////////////////////////////////////
ignition::math::Vector3d SnapIntervals::getRotation() const { return data_->rpy; }

/////////////////////////////////////////////////
ignition::math::Vector3d SnapIntervals::getScale() const { return data_->scale; }

/////////////////////////////////////////////////
SpawnFromDescription::SpawnFromDescription(const std::string& description, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->description = description;
  data_->scene_name = scene_name;
}
SpawnFromDescription::~SpawnFromDescription() = default;

/////////////////////////////////////////////////
const std::string& SpawnFromDescription::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
const std::string& SpawnFromDescription::getDescription() const { return data_->description; }

/////////////////////////////////////////////////
SpawnFromPath::SpawnFromPath(const std::string& file_path, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->file_path = file_path;
  data_->scene_name = scene_name;
}
SpawnFromPath::~SpawnFromPath() = default;

/////////////////////////////////////////////////
const std::string& SpawnFromPath::getFilePath() const { return data_->file_path; }

/////////////////////////////////////////////////
HoverToScene::HoverToScene(const ignition::math::Vector3d& point, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->point = point;
  data_->scene_name = scene_name;
}
HoverToScene::~HoverToScene() = default;

/////////////////////////////////////////////////
const std::string& HoverToScene::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
ignition::math::Vector3d HoverToScene::getPoint() const { return data_->point; }

/////////////////////////////////////////////////
HoverOnScene::HoverOnScene(const ignition::common::MouseEvent& mouse, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->mouse = mouse;
  data_->scene_name = scene_name;
}
HoverOnScene::~HoverOnScene() = default;

/////////////////////////////////////////////////
const std::string& HoverOnScene::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
ignition::common::MouseEvent HoverOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
LeftClickToScene::LeftClickToScene(const ignition::math::Vector3d& point, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->point = point;
  data_->scene_name = scene_name;
}
LeftClickToScene::~LeftClickToScene() = default;

/////////////////////////////////////////////////
const std::string& LeftClickToScene::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
ignition::math::Vector3d LeftClickToScene::getPoint() const { return data_->point; }

/////////////////////////////////////////////////
RightClickToScene::RightClickToScene(const ignition::math::Vector3d& point, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->point = point;
  data_->scene_name = scene_name;
}
RightClickToScene::~RightClickToScene() = default;

/////////////////////////////////////////////////
const std::string& RightClickToScene::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
ignition::math::Vector3d RightClickToScene::getPoint() const { return data_->point; }

/////////////////////////////////////////////////
DropdownMenuEnabled::DropdownMenuEnabled(bool menu_enabled, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->menu_enabled = menu_enabled;
  data_->scene_name = scene_name;
}
DropdownMenuEnabled::~DropdownMenuEnabled() = default;

/////////////////////////////////////////////////
const std::string& DropdownMenuEnabled::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
bool DropdownMenuEnabled::getMenuEnabled() const { return data_->menu_enabled; }

/////////////////////////////////////////////////
RightClickOnScene::RightClickOnScene(const ignition::common::MouseEvent& mouse, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->mouse = mouse;
  data_->scene_name = scene_name;
}
RightClickOnScene::~RightClickOnScene() = default;

/////////////////////////////////////////////////
const std::string& RightClickOnScene::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
const ignition::common::MouseEvent& RightClickOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
LeftClickOnScene::LeftClickOnScene(const ignition::common::MouseEvent& mouse, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->mouse = mouse;
  data_->scene_name = scene_name;
}
LeftClickOnScene::~LeftClickOnScene() = default;

/////////////////////////////////////////////////
const std::string& LeftClickOnScene::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
const ignition::common::MouseEvent& LeftClickOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
BlockOrbit::BlockOrbit(bool block, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->block = block;
  data_->scene_name = scene_name;
}
BlockOrbit::~BlockOrbit() = default;

/////////////////////////////////////////////////
const std::string& BlockOrbit::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
bool BlockOrbit::getBlock() const { return data_->block; }

/////////////////////////////////////////////////
KeyReleaseOnScene::KeyReleaseOnScene(const ignition::common::KeyEvent& key, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->key = key;
  data_->scene_name = scene_name;
}
KeyReleaseOnScene::~KeyReleaseOnScene() = default;

/////////////////////////////////////////////////
const std::string& KeyReleaseOnScene::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
ignition::common::KeyEvent KeyReleaseOnScene::getKey() const { return data_->key; }

/////////////////////////////////////////////////
KeyPressOnScene::KeyPressOnScene(const ignition::common::KeyEvent& key, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->key = key;
  data_->scene_name = scene_name;
}
KeyPressOnScene::~KeyPressOnScene() = default;

/////////////////////////////////////////////////
const std::string& KeyPressOnScene::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
ignition::common::KeyEvent KeyPressOnScene::getKey() const { return data_->key; }

/////////////////////////////////////////////////
SpawnCloneFromName::SpawnCloneFromName(const std::string& name, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->name = name;
  data_->scene_name = scene_name;
}

SpawnCloneFromName::~SpawnCloneFromName() = default;

/////////////////////////////////////////////////
const std::string& SpawnCloneFromName::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
const std::string& SpawnCloneFromName::getName() const { return data_->name; }

/////////////////////////////////////////////////
DropOnScene::DropOnScene(const std::string& drop_text,
                         const ignition::math::Vector2i& drop_mouse,
                         const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->drop_text = drop_text;
  data_->mouse = drop_mouse;
  data_->scene_name = scene_name;
}
DropOnScene::~DropOnScene() = default;

/////////////////////////////////////////////////
const std::string& DropOnScene::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
const std::string& DropOnScene::getDropText() const { return data_->drop_text; }

/////////////////////////////////////////////////
const ignition::math::Vector2i& DropOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
ScrollOnScene::ScrollOnScene(const ignition::common::MouseEvent& mouse, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->mouse = mouse;
  data_->scene_name = scene_name;
}
ScrollOnScene::~ScrollOnScene() = default;

/////////////////////////////////////////////////
const std::string& ScrollOnScene::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
const ignition::common::MouseEvent& ScrollOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
DragOnScene::DragOnScene(const ignition::common::MouseEvent& mouse, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->mouse = mouse;
  data_->scene_name = scene_name;
}
DragOnScene::~DragOnScene() = default;

/////////////////////////////////////////////////
const std::string& DragOnScene::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
ignition::common::MouseEvent DragOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
MousePressOnScene::MousePressOnScene(const ignition::common::MouseEvent& mouse, const std::string& scene_name)
  : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->mouse = mouse;
  data_->scene_name = scene_name;
}
MousePressOnScene::~MousePressOnScene() = default;

/////////////////////////////////////////////////
const std::string& MousePressOnScene::getSceneName() const { return data_->scene_name; }

/////////////////////////////////////////////////
const ignition::common::MouseEvent& MousePressOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
// WorldControl::WorldControl(const ignition::msgs::WorldControl &_worldControl)
//  : QEvent(kType)
//  , data_(std::make_unique<Implementation>())
//{
//  data_->worldControl = _worldControl;
//}

/////////////////////////////////////////////////
// const ignition::msgs::WorldControl &WorldControl::WorldControlInfo() const
//{
//  return data_->worldControl;
//}

/////////////////////////////////////////////////
PreRender::PreRender(const std::string& scene_name) : QEvent(kType), data_(std::make_unique<Implementation>())
{
  data_->scene_name = scene_name;
}
PreRender::~PreRender() = default;

/////////////////////////////////////////////////
const std::string& PreRender::getSceneName() const { return data_->scene_name; }
}  // namespace tesseract_gui::events
