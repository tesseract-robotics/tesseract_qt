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

#include <tesseract_qt/rendering/render_events.h>
#include <string>

namespace tesseract_gui::events
{
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
};

class SpawnFromDescription::Implementation
{
public:
  /// \brief The string of the resource to be spawned.
  std::string description;
};

class SpawnFromPath::Implementation
{
public:
  /// \brief The path of file to be previewed.
  std::string file_path;
};

class HoverToScene::Implementation
{
public:
  /// \brief The 3D point over which the user is hovering.
  ignition::math::Vector3d point;
};

class HoverOnScene::Implementation
{
public:
  /// \brief The 2D point over which the user is hovering.
  ignition::common::MouseEvent mouse;
};

class LeftClickToScene::Implementation
{
public:
  /// \brief The 3D point that the user clicked within the scene.
  ignition::math::Vector3d point;
};

class RightClickToScene::Implementation
{
public:
  /// \brief The 3D point that the user clicked within the scene.
  ignition::math::Vector3d point;
};

class DropdownMenuEnabled::Implementation
{
public:
  /// \brief The boolean indicating whether the menu is disabled or not
  /// for this event.
  bool menu_enabled;
};

class LeftClickOnScene::Implementation
{
public:
  /// \brief Mouse event
  ignition::common::MouseEvent mouse;
};

class RightClickOnScene::Implementation
{
public:
  /// \brief Mouse event
  ignition::common::MouseEvent mouse;
};

class BlockOrbit::Implementation
{
public:
  bool block;
};

class KeyReleaseOnScene::Implementation
{
public:
  /// \brief Key event
  ignition::common::KeyEvent key;
};

class KeyPressOnScene::Implementation
{
public:
  /// \brief Key event
  ignition::common::KeyEvent key;
};

class SpawnCloneFromName::Implementation
{
public:
  /// \brief The name of the resource to be cloned
  std::string name;
};

class DropOnScene::Implementation
{
public:
  /// \brief The name of the dropped thing
  std::string drop_text;

  /// \brief X and Y position of the mouse
  ignition::math::Vector2i mouse;
};

class ScrollOnScene::Implementation
{
public:
  /// \brief Mouse event with scroll information.
  ignition::common::MouseEvent mouse;
};

class DragOnScene::Implementation
{
public:
  /// \brief Mouse event with drag information.
  ignition::common::MouseEvent mouse;
};

class MousePressOnScene::Implementation
{
public:
  /// \brief Mouse event with press information.
  ignition::common::MouseEvent mouse;
};

/////////////////////////////////////////////////
Render::Render(const std::string& scene_name) : SceneEvent(scene_name, kType) {}
Render::~Render() = default;

/////////////////////////////////////////////////
SnapIntervals::SnapIntervals(const ignition::math::Vector3d& _xyz,
                             const ignition::math::Vector3d& _rpy,
                             const ignition::math::Vector3d& _scale,
                             const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->xyz = _xyz;
  data_->rpy = _rpy;
  data_->scale = _scale;
}

SnapIntervals::~SnapIntervals() = default;

/////////////////////////////////////////////////
ignition::math::Vector3d SnapIntervals::getPosition() const { return data_->xyz; }

/////////////////////////////////////////////////
ignition::math::Vector3d SnapIntervals::getRotation() const { return data_->rpy; }

/////////////////////////////////////////////////
ignition::math::Vector3d SnapIntervals::getScale() const { return data_->scale; }

/////////////////////////////////////////////////
SpawnFromDescription::SpawnFromDescription(const std::string& description, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->description = description;
}
SpawnFromDescription::~SpawnFromDescription() = default;

/////////////////////////////////////////////////
const std::string& SpawnFromDescription::getDescription() const { return data_->description; }

/////////////////////////////////////////////////
SpawnFromPath::SpawnFromPath(const std::string& file_path, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->file_path = file_path;
}
SpawnFromPath::~SpawnFromPath() = default;

/////////////////////////////////////////////////
const std::string& SpawnFromPath::getFilePath() const { return data_->file_path; }

/////////////////////////////////////////////////
HoverToScene::HoverToScene(const ignition::math::Vector3d& point, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->point = point;
}
HoverToScene::~HoverToScene() = default;

/////////////////////////////////////////////////
ignition::math::Vector3d HoverToScene::getPoint() const { return data_->point; }

/////////////////////////////////////////////////
HoverOnScene::HoverOnScene(const ignition::common::MouseEvent& mouse, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->mouse = mouse;
}
HoverOnScene::~HoverOnScene() = default;

/////////////////////////////////////////////////
ignition::common::MouseEvent HoverOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
LeftClickToScene::LeftClickToScene(const ignition::math::Vector3d& point, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->point = point;
}
LeftClickToScene::~LeftClickToScene() = default;

/////////////////////////////////////////////////
ignition::math::Vector3d LeftClickToScene::getPoint() const { return data_->point; }

/////////////////////////////////////////////////
RightClickToScene::RightClickToScene(const ignition::math::Vector3d& point, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->point = point;
}
RightClickToScene::~RightClickToScene() = default;

/////////////////////////////////////////////////
ignition::math::Vector3d RightClickToScene::getPoint() const { return data_->point; }

/////////////////////////////////////////////////
DropdownMenuEnabled::DropdownMenuEnabled(bool menu_enabled, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->menu_enabled = menu_enabled;
}
DropdownMenuEnabled::~DropdownMenuEnabled() = default;

/////////////////////////////////////////////////
bool DropdownMenuEnabled::getMenuEnabled() const { return data_->menu_enabled; }

/////////////////////////////////////////////////
RightClickOnScene::RightClickOnScene(const ignition::common::MouseEvent& mouse, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->mouse = mouse;
}
RightClickOnScene::~RightClickOnScene() = default;

/////////////////////////////////////////////////
const ignition::common::MouseEvent& RightClickOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
LeftClickOnScene::LeftClickOnScene(const ignition::common::MouseEvent& mouse, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->mouse = mouse;
}
LeftClickOnScene::~LeftClickOnScene() = default;

/////////////////////////////////////////////////
const ignition::common::MouseEvent& LeftClickOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
BlockOrbit::BlockOrbit(bool block, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->block = block;
}
BlockOrbit::~BlockOrbit() = default;

/////////////////////////////////////////////////
bool BlockOrbit::getBlock() const { return data_->block; }

/////////////////////////////////////////////////
KeyReleaseOnScene::KeyReleaseOnScene(const ignition::common::KeyEvent& key, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->key = key;
}
KeyReleaseOnScene::~KeyReleaseOnScene() = default;

/////////////////////////////////////////////////
ignition::common::KeyEvent KeyReleaseOnScene::getKey() const { return data_->key; }

/////////////////////////////////////////////////
KeyPressOnScene::KeyPressOnScene(const ignition::common::KeyEvent& key, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->key = key;
}
KeyPressOnScene::~KeyPressOnScene() = default;

/////////////////////////////////////////////////
ignition::common::KeyEvent KeyPressOnScene::getKey() const { return data_->key; }

/////////////////////////////////////////////////
SpawnCloneFromName::SpawnCloneFromName(const std::string& name, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->name = name;
}

SpawnCloneFromName::~SpawnCloneFromName() = default;

/////////////////////////////////////////////////
const std::string& SpawnCloneFromName::getName() const { return data_->name; }

/////////////////////////////////////////////////
DropOnScene::DropOnScene(const std::string& drop_text,
                         const ignition::math::Vector2i& drop_mouse,
                         const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->drop_text = drop_text;
  data_->mouse = drop_mouse;
}
DropOnScene::~DropOnScene() = default;

/////////////////////////////////////////////////
const std::string& DropOnScene::getDropText() const { return data_->drop_text; }

/////////////////////////////////////////////////
const ignition::math::Vector2i& DropOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
ScrollOnScene::ScrollOnScene(const ignition::common::MouseEvent& mouse, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->mouse = mouse;
}
ScrollOnScene::~ScrollOnScene() = default;

/////////////////////////////////////////////////
const ignition::common::MouseEvent& ScrollOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
DragOnScene::DragOnScene(const ignition::common::MouseEvent& mouse, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->mouse = mouse;
}
DragOnScene::~DragOnScene() = default;

/////////////////////////////////////////////////
ignition::common::MouseEvent DragOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
MousePressOnScene::MousePressOnScene(const ignition::common::MouseEvent& mouse, const std::string& scene_name)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->mouse = mouse;
}
MousePressOnScene::~MousePressOnScene() = default;

/////////////////////////////////////////////////
const ignition::common::MouseEvent& MousePressOnScene::getMouse() const { return data_->mouse; }

/////////////////////////////////////////////////
// WorldControl::WorldControl(const ignition::msgs::WorldControl &_worldControl)
//  :SceneEvent(scene_name, kType)
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
PreRender::PreRender(const std::string& scene_name) : SceneEvent(scene_name, kType) {}
PreRender::~PreRender() = default;

}  // namespace tesseract_gui::events
