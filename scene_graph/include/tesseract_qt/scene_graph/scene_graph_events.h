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
#ifndef TESSERACT_QT_SCENE_GRAPH_SCENE_GRAPH_EVENTS_H
#define TESSERACT_QT_SCENE_GRAPH_SCENE_GRAPH_EVENTS_H

#include <memory>
#include <tesseract_qt/common/event_type.h>
#include <tesseract_qt/common/scene_events.h>

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
class SceneStateChanged : public SceneEvent
{
public:
  SceneStateChanged(const std::string& scene_name, tesseract_scene_graph::SceneState scene_state);
  ~SceneStateChanged() override;

  const tesseract_scene_graph::SceneState& getState() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_STATE_CHANGED);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called when scene graph is set */
class SceneGraphSet : public SceneEvent
{
public:
  SceneGraphSet(const std::string& scene_name, std::shared_ptr<const tesseract_scene_graph::SceneGraph> scene_graph);
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
class SceneGraphAddLink : public SceneEvent
{
public:
  SceneGraphAddLink(const std::string& scene_name, std::shared_ptr<const tesseract_scene_graph::Link> link);
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
class SceneGraphAddJoint : public SceneEvent
{
public:
  SceneGraphAddJoint(const std::string& scene_name, std::shared_ptr<const tesseract_scene_graph::Joint> joint);
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
class SceneGraphMoveLink : public SceneEvent
{
public:
  SceneGraphMoveLink(const std::string& scene_name, std::shared_ptr<const tesseract_scene_graph::Joint> joint);
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
class SceneGraphMoveJoint : public SceneEvent
{
public:
  SceneGraphMoveJoint(const std::string& scene_name, std::string joint_name, std::string parent_link);
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
class SceneGraphRemoveLink : public SceneEvent
{
public:
  SceneGraphRemoveLink(const std::string& scene_name, std::string link_name, bool recursive);
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
class SceneGraphRemoveJoint : public SceneEvent
{
public:
  SceneGraphRemoveJoint(const std::string& scene_name, std::string joint_name, bool recursive);
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
class SceneGraphReplaceJoint : public SceneEvent
{
public:
  SceneGraphReplaceJoint(const std::string& scene_name, std::shared_ptr<const tesseract_scene_graph::Joint> joint);
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
class SceneGraphModifyLinkVisibility : public SceneEvent
{
public:
  SceneGraphModifyLinkVisibility(const std::string& scene_name,
                                 std::string link_name,
                                 LinkVisibilityFlags flags,
                                 bool visible);
  ~SceneGraphModifyLinkVisibility() override;

  const std::string& getLinkName() const;
  LinkVisibilityFlags getVisibilityFlags() const;
  bool visible();

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_MODIFY_LINK_VISIBILITY);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to modify all link visibility */
class SceneGraphModifyLinkVisibilityALL : public SceneEvent
{
public:
  SceneGraphModifyLinkVisibilityALL(const std::string& scene_name, LinkVisibilityFlags flags, bool visible);
  ~SceneGraphModifyLinkVisibilityALL() override;

  LinkVisibilityFlags getVisibilityFlags() const;
  bool visible();

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_MODIFY_LINK_VISIBILITY_ALL);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to plot scene graph */
class SceneGraphPlot : public SceneEvent
{
public:
  SceneGraphPlot(const std::string& scene_name);
  ~SceneGraphPlot() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCENE_GRAPH_PLOT);
};

}  // namespace tesseract_gui::events
#endif  // TESSERACT_QT_SCENE_GRAPH_SCENE_GRAPH_EVENTS_H
