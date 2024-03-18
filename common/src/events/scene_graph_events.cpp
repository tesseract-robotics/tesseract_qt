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
#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/link_visibility.h>

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

SceneStateChanged::SceneStateChanged(std::shared_ptr<const tesseract_gui::ComponentInfo> component_info,
                                     tesseract_scene_graph::SceneState scene_state)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->state = std::move(scene_state);
}
SceneStateChanged::SceneStateChanged(const SceneStateChanged& other)
  : SceneStateChanged(other.getComponentInfo(), other.getState())
{
}

SceneStateChanged::~SceneStateChanged() = default;

const tesseract_scene_graph::SceneState& SceneStateChanged::getState() const { return data_->state; }

//////////////////////////////////////////

SceneGraphClear::SceneGraphClear(std::shared_ptr<const tesseract_gui::ComponentInfo> component_info)
  : ComponentEvent(std::move(component_info), kType)
{
}
SceneGraphClear::SceneGraphClear(const SceneGraphClear& other) : SceneGraphClear(other.getComponentInfo()) {}
SceneGraphClear::~SceneGraphClear() = default;

//////////////////////////////////////////

class SceneGraphSet::Implementation
{
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  tesseract_scene_graph::SceneGraph::ConstPtr scene_graph;
};

SceneGraphSet::SceneGraphSet(std::shared_ptr<const tesseract_gui::ComponentInfo> component_info,
                             tesseract_scene_graph::SceneGraph::ConstPtr scene_graph)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->scene_graph = std::move(scene_graph);
}
SceneGraphSet::SceneGraphSet(const SceneGraphSet& other)
  : SceneGraphSet(other.getComponentInfo(), other.getSceneGraph())
{
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

SceneGraphAddLink::SceneGraphAddLink(std::shared_ptr<const tesseract_gui::ComponentInfo> component_info,
                                     tesseract_scene_graph::Link::ConstPtr link)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->link = std::move(link);
}
SceneGraphAddLink::SceneGraphAddLink(const SceneGraphAddLink& other)
  : SceneGraphAddLink(other.getComponentInfo(), other.getLink())
{
}
SceneGraphAddLink::~SceneGraphAddLink() = default;

tesseract_scene_graph::Link::ConstPtr SceneGraphAddLink::getLink() const { return data_->link; }

//////////////////////////////////////////

class SceneGraphAddJoint::Implementation
{
public:
  tesseract_scene_graph::Joint::ConstPtr joint;
};

SceneGraphAddJoint::SceneGraphAddJoint(std::shared_ptr<const tesseract_gui::ComponentInfo> component_info,
                                       tesseract_scene_graph::Joint::ConstPtr joint)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->joint = std::move(joint);
}
SceneGraphAddJoint::SceneGraphAddJoint(const SceneGraphAddJoint& other)
  : SceneGraphAddJoint(other.getComponentInfo(), other.getJoint())
{
}
SceneGraphAddJoint::~SceneGraphAddJoint() = default;

tesseract_scene_graph::Joint::ConstPtr SceneGraphAddJoint::getJoint() const { return data_->joint; }

//////////////////////////////////////////

class SceneGraphMoveLink::Implementation
{
public:
  tesseract_scene_graph::Joint::ConstPtr joint;
};

SceneGraphMoveLink::SceneGraphMoveLink(std::shared_ptr<const tesseract_gui::ComponentInfo> component_info,
                                       tesseract_scene_graph::Joint::ConstPtr joint)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->joint = std::move(joint);
}
SceneGraphMoveLink::SceneGraphMoveLink(const SceneGraphMoveLink& other)
  : SceneGraphMoveLink(other.getComponentInfo(), other.getJoint())
{
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

SceneGraphMoveJoint::SceneGraphMoveJoint(std::shared_ptr<const tesseract_gui::ComponentInfo> component_info,
                                         std::string joint_name,
                                         std::string parent_link)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->joint_name = std::move(joint_name);
  data_->parent_link = std::move(parent_link);
}
SceneGraphMoveJoint::SceneGraphMoveJoint(const SceneGraphMoveJoint& other)
  : SceneGraphMoveJoint(other.getComponentInfo(), other.getJointName(), other.getParentLink())
{
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

SceneGraphRemoveLink::SceneGraphRemoveLink(std::shared_ptr<const tesseract_gui::ComponentInfo> component_info,
                                           std::string link_name,
                                           bool recursive)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->link_name = std::move(link_name);
  data_->recursive = recursive;
}
SceneGraphRemoveLink::SceneGraphRemoveLink(const SceneGraphRemoveLink& other)
  : SceneGraphRemoveLink(other.getComponentInfo(), other.getLinkName(), other.isRecursive())
{
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

SceneGraphRemoveJoint::SceneGraphRemoveJoint(std::shared_ptr<const tesseract_gui::ComponentInfo> component_info,
                                             std::string joint_name,
                                             bool recursive)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->joint_name = std::move(joint_name);
  data_->recursive = recursive;
}
SceneGraphRemoveJoint::SceneGraphRemoveJoint(const SceneGraphRemoveJoint& other)
  : SceneGraphRemoveJoint(other.getComponentInfo(), other.getJointName(), other.isRecursive())
{
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

SceneGraphReplaceJoint::SceneGraphReplaceJoint(std::shared_ptr<const tesseract_gui::ComponentInfo> component_info,
                                               tesseract_scene_graph::Joint::ConstPtr joint)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->joint = std::move(joint);
}
SceneGraphReplaceJoint::SceneGraphReplaceJoint(const SceneGraphReplaceJoint& other)
  : SceneGraphReplaceJoint(other.getComponentInfo(), other.getJoint())
{
}

SceneGraphReplaceJoint::~SceneGraphReplaceJoint() = default;

tesseract_scene_graph::Joint::ConstPtr SceneGraphReplaceJoint::getJoint() const { return data_->joint; }

//////////////////////////////////////////

class SceneGraphModifyLinkVisibility::Implementation
{
public:
  std::vector<std::string> link_names;
  LinkVisibilityFlags flags;
  bool visible;
};

SceneGraphModifyLinkVisibility::SceneGraphModifyLinkVisibility(
    std::shared_ptr<const tesseract_gui::ComponentInfo> component_info,
    std::vector<std::string> link_names,
    LinkVisibilityFlags flags,
    bool visible)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->link_names = std::move(link_names);
  data_->flags = flags;
  data_->visible = visible;
}
SceneGraphModifyLinkVisibility::SceneGraphModifyLinkVisibility(const SceneGraphModifyLinkVisibility& other)
  : SceneGraphModifyLinkVisibility(other.getComponentInfo(),
                                   other.getLinkNames(),
                                   other.getVisibilityFlags(),
                                   other.visible())
{
}
SceneGraphModifyLinkVisibility::~SceneGraphModifyLinkVisibility() = default;

const std::vector<std::string>& SceneGraphModifyLinkVisibility::getLinkNames() const { return data_->link_names; }
LinkVisibilityFlags SceneGraphModifyLinkVisibility::getVisibilityFlags() const { return data_->flags; }
bool SceneGraphModifyLinkVisibility::visible() const { return data_->visible; }

//////////////////////////////////////////

class SceneGraphModifyLinkVisibilityALL::Implementation
{
public:
  LinkVisibilityFlags flags;
  bool visible;
};

SceneGraphModifyLinkVisibilityALL::SceneGraphModifyLinkVisibilityALL(
    std::shared_ptr<const tesseract_gui::ComponentInfo> component_info,
    LinkVisibilityFlags flags,
    bool visible)
  : ComponentEvent(std::move(component_info), kType), data_(std::make_unique<Implementation>())
{
  data_->flags = flags;
  data_->visible = visible;
}
SceneGraphModifyLinkVisibilityALL::SceneGraphModifyLinkVisibilityALL(const SceneGraphModifyLinkVisibilityALL& other)
  : SceneGraphModifyLinkVisibilityALL(other.getComponentInfo(), other.getVisibilityFlags(), other.visible())
{
}
SceneGraphModifyLinkVisibilityALL::~SceneGraphModifyLinkVisibilityALL() = default;

LinkVisibilityFlags SceneGraphModifyLinkVisibilityALL::getVisibilityFlags() const { return data_->flags; }
bool SceneGraphModifyLinkVisibilityALL::visible() const { return data_->visible; }

//////////////////////////////////////////

SceneGraphPlot::SceneGraphPlot(std::shared_ptr<const ComponentInfo> component_info)
  : ComponentEvent(std::move(component_info), kType)
{
}
SceneGraphPlot::SceneGraphPlot(const SceneGraphPlot& other) : SceneGraphPlot(other.getComponentInfo()) {}
SceneGraphPlot::~SceneGraphPlot() = default;

}  // namespace tesseract_gui::events
