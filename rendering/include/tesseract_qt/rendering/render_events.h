/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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
#ifndef TESSERACT_QT_RENDERING_RENDER_EVENTS_H
#define TESSERACT_QT_RENDERING_RENDER_EVENTS_H
#include <QEvent>
#include <QMap>
#include <QString>

#include <set>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <tesseract_qt/common/entity.h>
#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/render_events.h>

#include <gz/common/KeyEvent.hh>
#include <gz/common/MouseEvent.hh>
#include <gz/math/Vector2.hh>
#include <gz/math/Vector3.hh>

/** @brief Namespace for all events. Refer to the EventManager class for more information about events. */
namespace tesseract_gui::events
{
/**
 * @brief The class for sending and receiving custom snap value events.
 * This event is used in the Transform Control plugin tool when the user manually alters their snapping values.
 */
class SnapIntervals : public RenderEvent
{
public:
  /// \brief Constructor
  /// \param[in] _xyz XYZ snapping values.
  /// \param[in] _rpy RPY snapping values.
  /// \param[in] _scale Scale snapping values.
  SnapIntervals(const gz::math::Vector3d& _xyz,
                const gz::math::Vector3d& _rpy,
                const gz::math::Vector3d& _scale,
                std::string scene_name);

  ~SnapIntervals() override;

  /**
   * @brief Get the XYZ snapping values.
   * @return The XYZ snapping values.
   */
  gz::math::Vector3d getPosition() const;

  /**
   * @brief Get the RPY snapping values.
   * @return The RPY snapping values.
   */
  gz::math::Vector3d getRotation() const;

  /**
   * @brief Get the scale snapping values.
   * @return The scale snapping values.
   */
  gz::math::Vector3d getScale() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SNAP_INTERVALS);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to spawn a resource, given its description as a string.*/
class SpawnFromDescription : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param description The resource's description as a string, such as an SDF file.
   * @param scene_name The scene name
   */
  explicit SpawnFromDescription(const std::string& description, std::string scene_name);
  ~SpawnFromDescription() override;

  /**
   * @brief Get the string description of the resource.
   * @return The resource string
   */
  const std::string& getDescription() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SPAWN_FROM_DESCRIPTION);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to spawn a resource, which takes the path to its file. */
class SpawnFromPath : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param file_path The path to a file.
   * @param scene_name The scene name
   */
  explicit SpawnFromPath(const std::string& file_path, std::string scene_name);
  ~SpawnFromPath() override;

  /**
   * @brief Get the path of the file.
   * @return The file path.
   */
  const std::string& getFilePath() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SPAWN_FROM_PATH);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event which is called to broadcast the 3D coordinates of a user's mouse hover within the scene. */
class HoverToScene : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param point The point at which the mouse is hovering within
   * @param scene_name Then scene name
   */
  explicit HoverToScene(const gz::math::Vector3d& point, std::string scene_name);
  ~HoverToScene() override;

  /**
   * @brief Get the point within the scene over which the user is hovering.
   * @return The 3D point
   */
  gz::math::Vector3d getPoint() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::HOVER_TO_SCENE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event which is called to broadcast the 3D coordinates of a user's releasing the left button within the scene.
 */
class LeftClickToScene : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param point The point which the user has left clicked within
   * @param scene_name The scene name
   */
  explicit LeftClickToScene(const gz::math::Vector3d& point, std::string scene_name);
  ~LeftClickToScene() override;

  /**
   * @brief Get the point within the scene that the user clicked.
   * @return The 3D point.
   */
  gz::math::Vector3d getPoint() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::LEFT_CLICK_TO_SCENE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event which is called to broadcast the 3D coordinates of a user's releasing the right button within the
 * scene. */
class RightClickToScene : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param point The point which the user has right clicked within the scene
   */
  explicit RightClickToScene(const gz::math::Vector3d& point, std::string scene_name);
  ~RightClickToScene() override;

  /**
   * @brief Get the point within the scene that the user clicked.
   * @return The 3D point.
   */
  gz::math::Vector3d getPoint() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::RIGHT_CLICK_TO_SCENE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/**
 * @brief Event which is called to enable or disable the dropdown menu.
 * @details This is primarily used by plugins which also use the right click mouse event to cancel any actions currently
 * in progress.
 */
class DropdownMenuEnabled : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param menu_enabled The boolean indicating whether the dropdown menu should be enabled or disabled.
   * @param scene_name The scene name
   */
  explicit DropdownMenuEnabled(bool menu_enabled, std::string scene_name);
  ~DropdownMenuEnabled() override;

  /**
   * @brief Gets whether the menu is enabled or not for this event.
   * @return True if enabling the menu, false if disabling the menu
   */
  bool getMenuEnabled() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::DROPDOWN_MENU_ENABLED);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event which is called to broadcast the key release within the scene. */
class KeyReleaseOnScene : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param key The key released event within the scene
   * @param scene_name The scene name
   */
  explicit KeyReleaseOnScene(const gz::common::KeyEvent& key, std::string scene_name);
  ~KeyReleaseOnScene() override;

  /**
   * @brief Get the released key within the scene that the user released.
   * @return The key code.
   */
  gz::common::KeyEvent getKey() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::KEY_RELEASE_ON_SCENE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event which is called to broadcast the key press within the scene. */
class KeyPressOnScene : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param key The pressed key within the scene
   * @param scene_name The scene name
   */
  explicit KeyPressOnScene(const gz::common::KeyEvent& key, std::string scene_name);
  ~KeyPressOnScene() override;

  /**
   * @brief Get the key within the scene that the user pressed
   * @return The key code.
   */
  gz::common::KeyEvent getKey() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::KEY_PRESS_ON_SCENE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/**
 * @brief Event which is called to broadcast information about left mouse releases on the scene.
 * @details For the 3D coordinates of that point on the scene, see `LeftClickToScene`.
 * @sa LeftClickToScene
 */
class LeftClickOnScene : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param mouse The left mouse event on the scene
   * @param scene_name The scene name
   */
  explicit LeftClickOnScene(const gz::common::MouseEvent& mouse, std::string scene_name);
  ~LeftClickOnScene() override;

  /** @brief Return the left mouse event */
  const gz::common::MouseEvent& getMouse() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::LEFT_CLICK_ON_SCENE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/**
 * @brief Event which is called to broadcast information about right mouse releases on the scene.
 * @details For the 3D coordinates of that point on the scene, see `RightClickToScene`.
 * @sa RightClickToScene
 */
class RightClickOnScene : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param mouse The right mouse event on the scene
   * @param scene_name The scene name
   */
  RightClickOnScene(const gz::common::MouseEvent& mouse, std::string scene_name);
  ~RightClickOnScene() override;

  /** @brief Return the right mouse event */
  const gz::common::MouseEvent& getMouse() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::RIGHT_CLICK_ON_SCENE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/**
 * @brief Event that block the Interactive View control when some of the other plugins require it.
 * @details For example: When the transform control is active we should block the movements of the camera.
 */
class BlockOrbit : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param block True to block otherwise False
   * @param scene_name The scene name
   */
  explicit BlockOrbit(bool block, std::string scene_name);
  ~BlockOrbit() override;

  /**
   * @brief Get the if the event should block the Interactive view controller
   * @return True to block otherwise False.
   */
  bool getBlock() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::BLOCK_ORBIT);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event which is called to broadcast the 2D coordinates of a user's mouse hover within the scene. */
class HoverOnScene : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param mouse he hover mouse event on the scene
   * @param scene_name The scene name
   */
  explicit HoverOnScene(const gz::common::MouseEvent& mouse, std::string scene_name);
  ~HoverOnScene() override;

  /** @brief Get the point within the scene over which the user is hovering. */
  gz::common::MouseEvent getMouse() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::HOVER_ON_SCENE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to clone a resource, given its name as a string. */
class SpawnCloneFromName : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param name The name of the resource to clone
   * @param scene_name The scene name
   */
  explicit SpawnCloneFromName(const std::string& name, std::string scene_name);
  ~SpawnCloneFromName() override;

  /**
   * @brief Get the name of the resource to be cloned
   * @return The name of the resource to be cloned
   */
  const std::string& getName() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SPAWN_CLONE_FROM_NAME);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to clone a resource, given its name as a string. */
class DropOnScene : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param drop_text Dropped string.
   * @param drop_mouse x and y  coordinate of mouse position.
   * @param scene_name The scene name
   */
  explicit DropOnScene(const std::string& drop_text, const gz::math::Vector2i& drop_mouse, std::string scene_name);
  ~DropOnScene() override;

  /**
   * @brief Get the text of the dropped thing on the scene
   * @return The name of the dropped thing on the scene
   */
  const std::string& getDropText() const;

  /**
   * @brief Get X and Y position
   * @return Get X and Y position
   */
  const gz::math::Vector2i& getMouse() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::DROP_ON_SCENE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event which is called to broadcast information about mouse scrolls on the scene. */
class ScrollOnScene : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param mouse The scroll mouse event on the scene
   * @param scene_name The scene name
   */
  explicit ScrollOnScene(const gz::common::MouseEvent& mouse, std::string scene_name);
  ~ScrollOnScene() override;

  /** @brief Return the scroll mouse event */
  const gz::common::MouseEvent& getMouse() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::SCROLL_ON_SCENE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event which is called to broadcast information about mouse drags on the scene. */
class DragOnScene : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param mouse The drag mouse event on the scene
   * @param scene_name The scene name
   */
  explicit DragOnScene(const gz::common::MouseEvent& mouse, std::string scene_name);
  ~DragOnScene() override;

  /** @brief Return the drag on mouse event */
  gz::common::MouseEvent getMouse() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::DRAG_ON_SCENE);

public:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event which is called to broadcast information about mouse presses on the scene, with right, left or middle
 * buttons. */
class MousePressOnScene : public RenderEvent
{
public:
  /**
   * @brief Constructor
   * @param mouse The mouse event on the scene
   * @param scene_name The scene name
   */
  MousePressOnScene(const gz::common::MouseEvent& mouse, std::string scene_name);
  ~MousePressOnScene() override;

  /** @brief Return the button press mouse event */
  const gz::common::MouseEvent& getMouse() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::MOUSE_PRESS_ON_SCENE);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

//  /// \brief Event which is called to share WorldControl information.
//  class WorldControl : public QEvent
//  {
//  public:
//    /// \brief Constructor
//    /// \param[in] _worldControl The WorldControl information
//    explicit WorldControl(const gz::msgs::WorldControl &_worldControl);

//    /// \brief Get the WorldControl information
//    /// \return The WorldControl information
//    const gz::msgs::WorldControl &WorldControlInfo() const;

//    /// \brief Unique type for this event.
//    static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser - 19);
//  private:

//    /// \internal
//    /// \brief Private data pointer
//    class Implementation;
//    std::unique_ptr<Implementation> data_;
//  };

//  /// \brief Event that notifies when new entities have been selected.
//  class EntitiesSelected : public QEvent
//  {
//    /// \brief Constructor
//    /// \param[in] _entities All the selected entities
//    /// \param[in] _fromUser True if the event was directly generated by the
//    /// user, false in case it's been propagated through a different mechanism.
//    public: explicit EntitiesSelected(
//        const std::vector<Entity> &_entities,  // NOLINT
//        bool _fromUser = false)
//        : QEvent(kType), entities(_entities), fromUser(_fromUser)
//    {
//    }

//    /// \brief Get the data sent with the event.
//    /// \return The entities being selected.
//    public: std::vector<Entity> Data() const
//    {
//      return this->entities;
//    }

//    /// \brief Get whether the event was generated by the user.
//    /// \return True for the user.
//    public: bool FromUser() const
//    {
//      return this->fromUser;
//    }

//    /// \brief Unique type for this event.
//    static const QEvent::Type kType = QEvent::Type(QEvent::User + 1);

//    /// \brief The selected entities.
//    private: std::vector<Entity> entities;

//    /// \brief Whether the event was generated by the user,
//    private: bool fromUser{false};
//  };

//  /// \brief Event that notifies when all entities have been deselected.
//  class DeselectAllEntities : public QEvent
//  {
//    /// \brief Constructor
//    /// \param[in] _fromUser True if the event was directly generated by the
//    /// user, false in case it's been propagated through a different mechanism.
//    public: explicit DeselectAllEntities(bool _fromUser = false)
//        : QEvent(kType), fromUser(_fromUser)
//    {
//    }

//    /// \brief Get whether the event was generated by the user.
//    /// \return True for the user.
//    public: bool FromUser() const
//    {
//      return this->fromUser;
//    }

//    /// \brief Unique type for this event.
//    static const QEvent::Type kType = QEvent::Type(QEvent::User + 2);

//    /// \brief Whether the event was generated by the user,
//    private: bool fromUser{false};
//  };

//  /// \brief Event that contains entities newly created or removed from the
//  /// GUI, but that aren't present on the server yet.
//  /// \sa NewRemovedEntities
//  class GuiNewRemovedEntities : public QEvent
//  {
//    /// \brief Constructor
//    /// \param[in] _newEntities Set of newly created entities
//    /// \param[in] _removedEntities Set of recently removed entities
//    public: GuiNewRemovedEntities(const std::set<Entity> &_newEntities,
//                const std::set<Entity> &_removedEntities);

//    /// \brief Get the set of newly created entities
//    public: const std::set<Entity> &NewEntities() const;

//    /// \brief Get the set of recently removed entities
//    public: const std::set<Entity> &RemovedEntities() const;

//    /// \brief Unique type for this event.
//    static const QEvent::Type kType = QEvent::Type(QEvent::User + 3);

//    /// \internal
//    /// \brief Private data pointer
//    class Implementation;
//    std::unique_ptr<Implementation> data_;
//  };

//  /// \brief Event that notifies when new entities have been created or removed
//  /// on the server. This is a duplication of what `GuiSystem`s would get from
//  /// `EachNew` / `EachRemoved` ECM calls.
//  /// \sa GuiNewRemovedEntities
//  class NewRemovedEntities : public QEvent
//  {
//    /// \brief Constructor
//    /// \param[in] _newEntities Set of newly created entities
//    /// \param[in] _removedEntities Set of recently removed entities
//    public: NewRemovedEntities(const std::set<Entity> &_newEntities,
//                const std::set<Entity> &_removedEntities);

//    /// \brief Get the set of newly created entities
//    public: const std::set<Entity> &NewEntities() const;

//    /// \brief Get the set of recently removed entities
//    public: const std::set<Entity> &RemovedEntities() const;

//    /// \brief Unique type for this event.
//    static const QEvent::Type kType = QEvent::Type(QEvent::User + 4);

//    /// \internal
//    /// \brief Private data pointer
//    class Implementation;
//    std::unique_ptr<Implementation> data_;
//  };

//  /// \brief True if a transform control is currently active (translate /
//  /// rotate / scale). False if we're in selection mode.
//  class TransformControlModeActive : public QEvent
//  {
//    /// \brief Constructor
//    /// \param[in] _tranformModeActive is the transform control mode active
//    public: explicit TransformControlModeActive(const bool _tranformModeActive)
//        : QEvent(kType), tranformModeActive(_tranformModeActive)
//    {
//    }

//    /// \brief Unique type for this event.
//    static const QEvent::Type kType = QEvent::Type(QEvent::User + 6);

//    /// \brief Get the event's value.
//    public: bool TransformControlActive()
//    {
//      return this->tranformModeActive;
//    }

//    /// \brief True if a transform mode is active.
//    private: bool tranformModeActive;
//  };

//  /// \brief Event that notifies an entity is to be added to the model editor
//  class ModelEditorAddEntity : public QEvent
//  {
//    /// \brief Constructor
//    /// \param[in] _entity Entity added
//    /// \param[in] _type Entity type
//    /// \param[in] _parent Parent entity.
//    public: explicit ModelEditorAddEntity(QString _entity, QString _type, tesseract_gui::Entity _parent);

//    /// \brief Get the entity to add
//    public: QString Entity() const;

//    /// \brief Get the entity type
//    public: QString EntityType() const;

//    /// \brief Get the parent entity to add the entity to
//    public: tesseract_gui::Entity ParentEntity() const;

//    /// \brief Get the data map.
//    /// \return the QMap of string, string holding custom data.
//    public: QMap<QString, QString> &Data();

//    static const QEvent::Type kType = QEvent::Type(QEvent::User + 7);

//    /// \internal
//    /// \brief Private data pointer
//    class Implementation;
//    std::unique_ptr<Implementation> data_;
//  };

}  // namespace tesseract_gui::events
#endif  // TESSERACT_QT_RENDERING_RENDER_EVENTS_H
