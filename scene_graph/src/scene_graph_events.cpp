/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU Lesser General Public License Version 3, 29 June 2007
 * @par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * @par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * @par
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <tesseract_qt/scene_graph/scene_graph_events.h>
#include <tesseract_qt/scene_graph/scene_graph_link_visibility.h>

#include <tesseract_scene_graph/graph.h>
#include <tesseract_scene_graph/link.h>
#include <tesseract_scene_graph/joint.h>
#include <tesseract_scene_graph/scene_state.h>

#include <string>

namespace tesseract_gui::events
{
class SceneStateChanged::Implementation
{
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  tesseract_scene_graph::SceneState state;
};

SceneStateChanged::SceneStateChanged(const std::string& scene_name, tesseract_scene_graph::SceneState scene_state)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->state = std::move(scene_state);
}
SceneStateChanged::~SceneStateChanged() = default;

const tesseract_scene_graph::SceneState& SceneStateChanged::getState() const { return data_->state; }

//////////////////////////////////////////

SceneGraphClear::SceneGraphClear(const std::string& scene_name) : SceneEvent(scene_name, kType) {}
SceneGraphClear::~SceneGraphClear() = default;

//////////////////////////////////////////

class SceneGraphSet::Implementation
{
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  tesseract_scene_graph::SceneGraph::ConstPtr scene_graph;
};

SceneGraphSet::SceneGraphSet(const std::string& scene_name, tesseract_scene_graph::SceneGraph::ConstPtr scene_graph)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->scene_graph = std::move(scene_graph);
}
SceneGraphSet::~SceneGraphSet() = default;

tesseract_scene_graph::SceneGraph::ConstPtr SceneGraphSet::getSceneGraph() const { return data_->scene_graph; }

//////////////////////////////////////////

class SceneGraphAddLink::Implementation
{
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  tesseract_scene_graph::Link::ConstPtr link;
};

SceneGraphAddLink::SceneGraphAddLink(const std::string& scene_name, tesseract_scene_graph::Link::ConstPtr link)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->link = std::move(link);
}
SceneGraphAddLink::~SceneGraphAddLink() = default;

tesseract_scene_graph::Link::ConstPtr SceneGraphAddLink::getLink() const { return data_->link; }

//////////////////////////////////////////

class SceneGraphAddJoint::Implementation
{
public:
  tesseract_scene_graph::Joint::ConstPtr joint;
};

SceneGraphAddJoint::SceneGraphAddJoint(const std::string& scene_name, tesseract_scene_graph::Joint::ConstPtr joint)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->joint = std::move(joint);
}
SceneGraphAddJoint::~SceneGraphAddJoint() = default;

tesseract_scene_graph::Joint::ConstPtr SceneGraphAddJoint::getJoint() const { return data_->joint; }

//////////////////////////////////////////

class SceneGraphMoveLink::Implementation
{
public:
  tesseract_scene_graph::Joint::ConstPtr joint;
};

SceneGraphMoveLink::SceneGraphMoveLink(const std::string& scene_name, tesseract_scene_graph::Joint::ConstPtr joint)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->joint = std::move(joint);
}

SceneGraphMoveLink::~SceneGraphMoveLink() = default;

tesseract_scene_graph::Joint::ConstPtr SceneGraphMoveLink::getJoint() const { return data_->joint; }

//////////////////////////////////////////

class SceneGraphMoveJoint::Implementation
{
public:
  std::string joint_name;
  std::string parent_link;
};

SceneGraphMoveJoint::SceneGraphMoveJoint(const std::string& scene_name, std::string joint_name, std::string parent_link)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->joint_name = std::move(joint_name);
  data_->parent_link = std::move(parent_link);
}

SceneGraphMoveJoint::~SceneGraphMoveJoint() = default;

const std::string& SceneGraphMoveJoint::getJointName() const { return data_->joint_name; }
const std::string& SceneGraphMoveJoint::getParentLink() const { return data_->parent_link; }

//////////////////////////////////////////

class SceneGraphRemoveLink::Implementation
{
public:
  std::string link_name;
  bool recursive;
};

SceneGraphRemoveLink::SceneGraphRemoveLink(const std::string& scene_name, std::string link_name, bool recursive)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->link_name = std::move(link_name);
  data_->recursive = recursive;
}

SceneGraphRemoveLink::~SceneGraphRemoveLink() = default;

const std::string& SceneGraphRemoveLink::getLinkName() const { return data_->link_name; }
bool SceneGraphRemoveLink::isRecursive() const { return data_->recursive; }

//////////////////////////////////////////

class SceneGraphRemoveJoint::Implementation
{
public:
  std::string joint_name;
  bool recursive;
};

SceneGraphRemoveJoint::SceneGraphRemoveJoint(const std::string& scene_name, std::string joint_name, bool recursive)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->joint_name = std::move(joint_name);
  data_->recursive = recursive;
}

SceneGraphRemoveJoint::~SceneGraphRemoveJoint() = default;

const std::string& SceneGraphRemoveJoint::getJointName() const { return data_->joint_name; }
bool SceneGraphRemoveJoint::isRecursive() const { return data_->recursive; }

//////////////////////////////////////////

class SceneGraphReplaceJoint::Implementation
{
public:
  tesseract_scene_graph::Joint::ConstPtr joint;
};

SceneGraphReplaceJoint::SceneGraphReplaceJoint(const std::string& scene_name,
                                               tesseract_scene_graph::Joint::ConstPtr joint)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->joint = std::move(joint);
}

SceneGraphReplaceJoint::~SceneGraphReplaceJoint() = default;

tesseract_scene_graph::Joint::ConstPtr SceneGraphReplaceJoint::getJoint() const { return data_->joint; }

//////////////////////////////////////////

class SceneGraphModifyLinkVisibility::Implementation
{
public:
  std::string link_name;
  LinkVisibilityFlags flags;
  bool visible;
};

SceneGraphModifyLinkVisibility::SceneGraphModifyLinkVisibility(const std::string& scene_name,
                                                               std::string link_name,
                                                               LinkVisibilityFlags flags,
                                                               bool visible)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->link_name = std::move(link_name);
  data_->flags = flags;
  data_->visible = visible;
}

SceneGraphModifyLinkVisibility::~SceneGraphModifyLinkVisibility() = default;

const std::string& SceneGraphModifyLinkVisibility::getLinkName() const { return data_->link_name; }
LinkVisibilityFlags SceneGraphModifyLinkVisibility::getVisibilityFlags() const { return data_->flags; }
bool SceneGraphModifyLinkVisibility::visible() { return data_->visible; }

//////////////////////////////////////////

class SceneGraphModifyLinkVisibilityALL::Implementation
{
public:
  LinkVisibilityFlags flags;
  bool visible;
};

SceneGraphModifyLinkVisibilityALL::SceneGraphModifyLinkVisibilityALL(const std::string& scene_name,
                                                                     LinkVisibilityFlags flags,
                                                                     bool visible)
  : SceneEvent(scene_name, kType), data_(std::make_unique<Implementation>())
{
  data_->flags = flags;
  data_->visible = visible;
}

SceneGraphModifyLinkVisibilityALL::~SceneGraphModifyLinkVisibilityALL() = default;

LinkVisibilityFlags SceneGraphModifyLinkVisibilityALL::getVisibilityFlags() const { return data_->flags; }
bool SceneGraphModifyLinkVisibilityALL::visible() { return data_->visible; }

//////////////////////////////////////////

SceneGraphPlot::SceneGraphPlot(const std::string& scene_name) : SceneEvent(scene_name, kType) {}

SceneGraphPlot::~SceneGraphPlot() = default;

}  // namespace tesseract_gui::events
