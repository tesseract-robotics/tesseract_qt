#ifndef TESSERACT_QT_RENDERING_SIMPLE_RENDER_WIDGET_H
#define TESSERACT_QT_RENDERING_SIMPLE_RENDER_WIDGET_H

#include <ignition/common/KeyEvent.hh>
#include <ignition/common/MouseEvent.hh>
#include <ignition/math/Color.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector2.hh>

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

namespace tesseract_gui
{
class RendererImpl;

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

  /** @brief Destroy camera associated with this renderer */
  void destroy();

  /**
   * @brief New mouse event triggered
   * @param event New mouse event
   */
  void newMouseEvent(const ignition::common::MouseEvent& event);

  /**
   * @brief New hover event triggered.
   * @param hover_pos Mouse hover screen position
   */
  void newHoverEvent(const ignition::math::Vector2i& hover_pos);

  /**
   * @brief New hover event triggered.
   * @param drop_text Text dropped on the scene
   * @param drop_pos Mouse drop screen position
   */
  void newDropEvent(const std::string& drop_text, const ignition::math::Vector2i& drop_pos);

  /**
   * @brief Handle key release event for snapping
   * @param event The key event to process.
   */
  void handleKeyPress(const ignition::common::KeyEvent& event);

  /**
   * @brief Handle key release event for snapping
   * @param event The key event to process.
   */
  void handleKeyRelease(const ignition::common::KeyEvent& event);

  /** @brief Render engine to use */
  std::string engine_name{ "ogre2" };

  /** @brief Unique scene name */
  std::string scene_name{ "scene" };

  /** @brief Initial Camera pose */
  ignition::math::Pose3d camera_pose{ -6, 0, 6, 0, 0.5, 0 };

  /** @briefDefault camera near clipping plane distance */
  double camera_near_clip{ 0.01 };

  /** @brief Default camera far clipping plane distance */
  double camera_far_clip{ 1000.0 };

  /** @brief Scene background color */
  ignition::math::Color background_color{ 0.8f, 0.8f, 0.8f, 1.0f };

  /** @brief Ambient color */
  ignition::math::Color ambient_light{ 0.0f, 0.0f, 1.0f, 1.0f };

  /** @brief True if engine has been initialized */
  bool initialized{ false };

  /** @brief Render texture size */
  QSize texture_size{ 1920, 1200 };

  /** @brief Render texture opengl id */
  unsigned int texture_id{ 0 };

  /** @brief Flag to indicate texture size has changed. */
  bool texture_dirty{ false };

  /** @brief True if sky is enabled */
  bool sky_enable{ false };

  /** @brief True if grid is enabled */
  bool grid_enable{ true };

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
  ignition::math::Vector3d screenToScene(const ignition::math::Vector2i& screen_pos) const;

  /** @brief Pointer to private data */
  std::unique_ptr<RendererImpl> data_;
};

class RenderWidgetImpl;

class RenderWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT
public:
  /**
   * @brief Constructor
   * @param scene_name The name of the scene to render
   * @param parent Parent item
   */
  explicit RenderWidget(const std::string& scene_name, QWidget* parent = nullptr);

  ~RenderWidget();

  /**
   * @brief Set background color of render window
   * @param color olor of render window background
   */
  void setBackgroundColor(const ignition::math::Color& color);

  /**
   * @brief Set ambient light of render window
   * @param ambient Color of ambient light
   */
  void setAmbientLight(const ignition::math::Color& ambient);

  /**
   * @brief Set engine name used to create the render window
   * @param name Name of render engine
   */
  void setEngineName(const std::string& name);

  /**
   * @brief Set name of scene created inside the render window
   * @param name Name of scene
   */
  void setSceneName(const std::string& name);

  /**
   * @brief Set the initial pose the render window camera
   * @param pose Initial camera pose
   */
  void setCameraPose(const ignition::math::Pose3d& pose);

  /**
   * @brief Set the render window camera's near clipping plane distance
   * @param near Near clipping plane distance
   */
  void setCameraNearClip(double near);

  /**
   * @brief Set the render window camera's far clipping plane distance
   * @param far Far clipping plane distance
   */
  void setCameraFarClip(double far);

  /**
   * @brief Set if sky is enabled
   * @param True to enable the sky, false otherwise.
   */
  void setSkyEnabled(bool enabled);

  /**
   * @brief Show grid view in the scene
   * @param enabled True to enable grid view, false otherwise.
   */
  void setGridEnabled(bool enabled);

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
  void handleKeyPress(const ignition::common::KeyEvent& event);

  /**
   * @brief Handle key release event for snapping
   * @param event The key event to process
   */
  void handleKeyRelease(const ignition::common::KeyEvent& event);

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
  std::unique_ptr<RenderWidgetImpl> data_;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_RENDERING_SIMPLE_RENDER_WIDGET_H
