#ifndef TESSERACT_QT_RENDERING_SIMPLE_RENDER_WIDGET_H
#define TESSERACT_QT_RENDERING_SIMPLE_RENDER_WIDGET_H

#include <gz/common/KeyEvent.hh>
#include <gz/common/MouseEvent.hh>
#include <gz/math/Color.hh>
#include <gz/math/Pose3.hh>
#include <gz/math/Vector2.hh>

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

namespace tesseract_gui
{
/**
 *  @brief Tesseract Simple renderer.
 *  @details All rendering calls should be performed inside this class as it makes
 *  sure that opengl calls in the underlying render engine do not interfere
 *  with QtQuick's opengl render operations. The main Render function will
 *  render to an offscreen texture and notify via signal and slots when it's
 *  ready to be displayed.
 */
class Renderer
{
public:
  /** @brief Constructor */
  Renderer();

  /**
   * @brief Render scene
   * @param render_sync RenderSync to safely synchronize Qt and worker thread (this)
   */
  void render();

  /**
   * @brief Resize the camera
   * @param width The camera width
   * @param height The camera height
   */
  void resize(int width, int height);

  /** @brief Initialize the render engine */
  void initialize();

  /** @brief Check if initialized */
  bool isInitialized() const;

  /** @brief Destroy camera associated with this renderer */
  void destroy();

  /**
   * @brief New mouse event triggered
   * @param event New mouse event
   */
  void newMouseEvent(const gz::common::MouseEvent& event);

  /**
   * @brief New hover event triggered.
   * @param hover_pos Mouse hover screen position
   */
  void newHoverEvent(const gz::math::Vector2i& hover_pos);

  /**
   * @brief New hover event triggered.
   * @param drop_text Text dropped on the scene
   * @param drop_pos Mouse drop screen position
   */
  void newDropEvent(const std::string& drop_text, const gz::math::Vector2i& drop_pos);

  /**
   * @brief Handle key release event for snapping
   * @param event The key event to process.
   */
  void handleKeyPress(const gz::common::KeyEvent& event);

  /**
   * @brief Handle key release event for snapping
   * @param event The key event to process.
   */
  void handleKeyRelease(const gz::common::KeyEvent& event);

  /**
   * @brief Set engine name used to create the render window
   * @param name Name of render engine
   */
  void setEngineName(const std::string& name);
  const std::string& getEngineName() const;

  /**
   * @brief Set name of scene created inside the render window
   * @param name Name of scene
   */
  void setSceneName(const std::string& name);
  const std::string& getSceneName() const;

  /**
   * @brief Set background color of render window
   * @param color olor of render window background
   */
  void setBackgroundColor(const gz::math::Color& color);
  gz::math::Color getBackgroundColor() const;

  /**
   * @brief Set ambient light of render window
   * @param ambient Color of ambient light
   */
  void setAmbientLight(const gz::math::Color& color);
  gz::math::Color getAmbientLight() const;

  /** @brief Initial Camera pose */
  void setInitialCameraPose(const gz::math::Pose3d& camera_pose);
  gz::math::Pose3d getInitialCameraPose() const;
  gz::math::Pose3d getCameraPose() const;

  /** @briefDefault camera near clipping plane distance */
  void setCameraNearClipDistance(double near);
  double getCameraNearClipDistance() const;

  /** @brief Default camera far clipping plane distance */
  void setCameraFarClipDistance(double far);
  double getCameraFarClipDistance() const;

  /**
   * @brief Set the level of anti-aliasing used during rendering.
   * @details If a value of 0 is given, no anti-aliasing will be performed. Higher values can significantly
   * slow-down rendering times, depending on the underlying render engine.
   */
  void setCameraAntiAliasing(unsigned int value);
  unsigned int getCameraAntiAliasing() const;

  /** @brief Control sky is enabled */
  void setSkyEnabled(bool enabled);
  bool skyEnabled() const;

  /** @brief Control grid is enabled */
  void setGridEnabled(bool enabled);
  bool gridEnabled() const;

  /** @brief Control shadows is enabled */
  void setShadowsEnabled(bool enabled);
  bool shadowsEnabled() const;

  /**
   * @brief True if hover event is enabled
   * @details This is an expensive operation so only enable if needed
   */
  void setHoverEventEnabled(bool enabled);
  bool hoverEventEnabled() const;

  /** @brief Render texture size */
  const QSize& getTextureSize() const;

  /** @brief Render texture opengl id */
  unsigned int getTextureId() const;

private:
  /** @brief Handle mouse event for view control */
  void handleMouseEvent();

  /** @brief Broadcasts the currently hovered 3d scene location. */
  void broadcastHoverPos();

  /** @brief Broadcasts drag events. */
  void broadcastDrag();

  /** @brief Broadcasts a left click (release) within the scene */
  void broadcastLeftClick();

  /** @brief Broadcasts a right click (release) within the scene */
  void broadcastRightClick();

  /** @brief Broadcasts a mouse press within the scene */
  void broadcastMousePress();

  /** @brief Broadcasts a scroll event within the scene */
  void broadcastScroll();

  /** @brief Broadcasts a key release event within the scene */
  void broadcastKeyRelease();

  /** @brief Broadcasts a drop event within the scene */
  void broadcastDrop();

  /** @brief Broadcasts a key press event within the scene */
  void broadcastKeyPress();

  /**
   * @brief Retrieve the first point on a surface in the 3D scene hit by a ray cast from the given 2D screen
   * coordinates.
   * @param screen_pos 2D coordinates on the screen, in pixels.
   * @return 3D coordinates of a point in the 3D scene.
   */
  gz::math::Vector3d screenToScene(const gz::math::Vector2i& screen_pos) const;

  /** @brief Pointer to private data */
  struct Implementation;
  std::unique_ptr<Implementation> data_;
};

class RenderWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT
public:
  /**
   * @brief Constructor
   * @param scene_name The name of the scene to render
   * @param parent Parent item
   */
  explicit RenderWidget(const std::string& scene_name,
                        const std::string& engine_name = "ogre2",
                        QWidget* parent = nullptr);

  ~RenderWidget();

  /**
   * @brief Set background color of render window
   * @param color olor of render window background
   */
  void setBackgroundColor(const gz::math::Color& color);
  gz::math::Color getBackgroundColor() const;

  /**
   * @brief Set ambient light of render window
   * @param ambient Color of ambient light
   */
  void setAmbientLight(const gz::math::Color& ambient);
  gz::math::Color getAmbientLight() const;

  /**
   * @brief Set engine name used to create the render window
   * @param name Name of render engine
   */
  void setEngineName(const std::string& name);
  const std::string& getEngineName() const;

  /**
   * @brief Set name of scene created inside the render window
   * @param name Name of scene
   */
  void setSceneName(const std::string& name);
  const std::string& getSceneName() const;

  /**
   * @brief Set the initial pose the render window camera
   * @param pose Initial camera pose
   */
  void setInitialCameraPose(const gz::math::Pose3d& pose);
  gz::math::Pose3d getInitialCameraPose() const;

  /**
   * @brief Set the render window camera's near clipping plane distance
   * @param near Near clipping plane distance
   */
  void setCameraNearClip(double near);
  double getCameraNearClip() const;

  /**
   * @brief Set the render window camera's far clipping plane distance
   * @param far Far clipping plane distance
   */
  void setCameraFarClip(double far);
  double getCameraFarClip() const;

  /**
   * @brief Set the level of anti-aliasing used during rendering.
   * @details If a value of 0 is given, no anti-aliasing will be performed. Higher values can significantly
   * slow-down rendering times, depending on the underlying render engine.
   */
  void setCameraAntiAliasing(unsigned int value);
  unsigned int getCameraAntiAliasing() const;

  /**
   * @brief Set if sky is enabled
   * @param True to enable the sky, false otherwise.
   */
  void setSkyEnabled(bool enabled);
  bool skyEnabled() const;

  /**
   * @brief Show grid view in the scene
   * @param enabled True to enable grid view, false otherwise.
   */
  void setGridEnabled(bool enabled);
  bool gridEnabled() const;

  /**
   * @brief Show shadows view in the scene
   * @param enabled True to enable shadows view, false otherwise.
   */
  void setShadowsEnabled(bool enabled);
  bool shadowsEnabled() const;

  /** @brief The desired update frequency for rendering, default is 60hz */
  void setUpdateFrequency(double hz);
  double getUpdateFrequency() const;

  /**
   * @brief Called when the mouse hovers to a new position.
   * @param mouse_x The hovered mouse x position on the render window.
   * @param mouse_y The hovered mouse y position on the render window.
   */
  void onHovered(int mouse_x, int mouse_y);

  /**
   * @brief Callback when receives a drop event
   * @param drop Dropped string
   * @param mouse_x Drop coordinate of mouse x position
   * @param mouse_y Drop coordinate of mouse x position
   */
  void onDropped(const QString& drop, int mouse_x, int mouse_y);

  /**
   * @brief Handle key press event for snapping
   * @param event The key event to process
   */
  void handleKeyPress(const gz::common::KeyEvent& event);

  /**
   * @brief Handle key release event for snapping
   * @param event The key event to process
   */
  void handleKeyRelease(const gz::common::KeyEvent& event);

private Q_SLOTS:
  void onFrameSwapped();

protected:
  // Documentation inherited
  void mousePressEvent(QMouseEvent* event) override;

  // Documentation inherited
  void mouseReleaseEvent(QMouseEvent* event) override;

  // Documentation inherited
  void mouseMoveEvent(QMouseEvent* event) override;

  // Documentation inherited
  void keyPressEvent(QKeyEvent* event) override;

  // Documentation inherited
  void keyReleaseEvent(QKeyEvent* event) override;

  // Documentation inherited
  void wheelEvent(QWheelEvent* event) override;

  // Documentation inherited
  void initializeGL() override;

  // Documentation inherited
  void resizeGL(int w, int h) override;

  // Documentation inherited
  void paintGL() override;

  /** @brief Pointer to private data */
  struct Implementation;
  std::unique_ptr<Implementation> data_;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_RENDERING_SIMPLE_RENDER_WIDGET_H
