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
#ifndef TESSERACT_QT_COMMON_SCENE_GRAPH_EVENTS_H
#define TESSERACT_QT_COMMON_SCENE_GRAPH_EVENTS_H

#include <memory>
#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>

namespace tesseract_gui
{
enum class LinkVisibilityFlags;
}

namespace tesseract_scene_graph
{
class Link;
class Joint;
class SceneState;
class SceneGraph;
}  // namespace tesseract_scene_graph

namespace tesseract_gui::events
{
/** @brief Event called when scene state has changed */
class SceneStateChanged : public ComponentEvent
{
public:
  SceneStateChanged(std::shared_ptr<const ComponentInfo> component_info, tesseract_scene_graph::SceneState scene_state);
  SceneStateChanged(const SceneStateChanged& other);
  ~SceneStateChanged() override;

  const tesseract_scene_graph::SceneState& getState() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_STATE_CHANGED);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called when scene graph is clear */
class SceneGraphClear : public ComponentEvent
{
public:
  SceneGraphClear(std::shared_ptr<const ComponentInfo> component_info);
  SceneGraphClear(const SceneGraphClear& other);
  ~SceneGraphClear() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_CLEAR);
};

/** @brief Event called when scene graph is set */
class SceneGraphSet : public ComponentEvent
{
public:
  SceneGraphSet(std::shared_ptr<const ComponentInfo> component_info,
                std::shared_ptr<const tesseract_scene_graph::SceneGraph> scene_graph);
  SceneGraphSet(const SceneGraphSet& other);
  ~SceneGraphSet() override;

  std::shared_ptr<const tesseract_scene_graph::SceneGraph> getSceneGraph() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_SET);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to add link to scene graph */
class SceneGraphAddLink : public ComponentEvent
{
public:
  SceneGraphAddLink(std::shared_ptr<const ComponentInfo> component_info,
                    std::shared_ptr<const tesseract_scene_graph::Link> link);
  SceneGraphAddLink(const SceneGraphAddLink& other);
  ~SceneGraphAddLink() override;

  std::shared_ptr<const tesseract_scene_graph::Link> getLink() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_ADD_LINK);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to add joint to scene graph */
class SceneGraphAddJoint : public ComponentEvent
{
public:
  SceneGraphAddJoint(std::shared_ptr<const ComponentInfo> component_info,
                     std::shared_ptr<const tesseract_scene_graph::Joint> joint);
  SceneGraphAddJoint(const SceneGraphAddJoint& other);
  ~SceneGraphAddJoint() override;

  std::shared_ptr<const tesseract_scene_graph::Joint> getJoint() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_ADD_JOINT);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to move link to scene graph */
class SceneGraphMoveLink : public ComponentEvent
{
public:
  SceneGraphMoveLink(std::shared_ptr<const ComponentInfo> component_info,
                     std::shared_ptr<const tesseract_scene_graph::Joint> joint);
  SceneGraphMoveLink(const SceneGraphMoveLink& other);
  ~SceneGraphMoveLink() override;

  std::shared_ptr<const tesseract_scene_graph::Joint> getJoint() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_MOVE_LINK);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to move joint to scene graph */
class SceneGraphMoveJoint : public ComponentEvent
{
public:
  SceneGraphMoveJoint(std::shared_ptr<const ComponentInfo> component_info,
                      std::string joint_name,
                      std::string parent_link);
  SceneGraphMoveJoint(const SceneGraphMoveJoint& other);
  ~SceneGraphMoveJoint() override;

  const std::string& getJointName() const;
  const std::string& getParentLink() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_MOVE_JOINT);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to remove link to scene graph */
class SceneGraphRemoveLink : public ComponentEvent
{
public:
  SceneGraphRemoveLink(std::shared_ptr<const ComponentInfo> component_info, std::string link_name, bool recursive);
  SceneGraphRemoveLink(const SceneGraphRemoveLink& other);
  ~SceneGraphRemoveLink() override;

  const std::string& getLinkName() const;
  bool isRecursive() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_REMOVE_LINK);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to remove joint to scene graph */
class SceneGraphRemoveJoint : public ComponentEvent
{
public:
  SceneGraphRemoveJoint(std::shared_ptr<const ComponentInfo> component_info, std::string joint_name, bool recursive);
  SceneGraphRemoveJoint(const SceneGraphRemoveJoint& other);
  ~SceneGraphRemoveJoint() override;

  const std::string& getJointName() const;
  bool isRecursive() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_REMOVE_JOINT);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to replace joint to scene graph */
class SceneGraphReplaceJoint : public ComponentEvent
{
public:
  SceneGraphReplaceJoint(std::shared_ptr<const ComponentInfo> component_info,
                         std::shared_ptr<const tesseract_scene_graph::Joint> joint);
  SceneGraphReplaceJoint(const SceneGraphReplaceJoint& other);
  ~SceneGraphReplaceJoint() override;

  std::shared_ptr<const tesseract_scene_graph::Joint> getJoint() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_REPLACE_JOINT);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to modify link visibility */
class SceneGraphModifyLinkVisibility : public ComponentEvent
{
public:
  SceneGraphModifyLinkVisibility(std::shared_ptr<const ComponentInfo> component_info,
                                 std::vector<std::string> link_names,
                                 LinkVisibilityFlags flags,
                                 bool visible);
  SceneGraphModifyLinkVisibility(const SceneGraphModifyLinkVisibility& other);
  ~SceneGraphModifyLinkVisibility() override;

  const std::vector<std::string>& getLinkNames() const;
  LinkVisibilityFlags getVisibilityFlags() const;
  bool visible() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_MODIFY_LINK_VISIBILITY);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to modify all link visibility */
class SceneGraphModifyLinkVisibilityALL : public ComponentEvent
{
public:
  SceneGraphModifyLinkVisibilityALL(std::shared_ptr<const ComponentInfo> component_info,
                                    LinkVisibilityFlags flags,
                                    bool visible);
  SceneGraphModifyLinkVisibilityALL(const SceneGraphModifyLinkVisibilityALL& other);
  ~SceneGraphModifyLinkVisibilityALL() override;

  LinkVisibilityFlags getVisibilityFlags() const;
  bool visible() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_MODIFY_LINK_VISIBILITY_ALL);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to plot scene graph */
class SceneGraphPlot : public ComponentEvent
{
public:
  SceneGraphPlot(std::shared_ptr<const ComponentInfo> component_info);
  SceneGraphPlot(const SceneGraphPlot& other);
  ~SceneGraphPlot() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_PLOT);
};

}  // namespace tesseract_gui::events
#endif  // TESSERACT_QT_COMMON_SCENE_GRAPH_EVENTS_H
