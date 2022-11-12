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

#ifndef TESSERACT_QT_RENDERING_INTERACTIVE_VIEW_CONTROL_H
#define TESSERACT_QT_RENDERING_INTERACTIVE_VIEW_CONTROL_H

#include <memory>
#include <QObject>

namespace tesseract_gui
{
/**  @brief The types of view controllers available  */
enum class ViewControlType
{
  ORBIT,
  ORTHO
};

/**
 * @brief Convert view control type to string
 * @param type The type to convert to string
 * @return The string represenation of the view control type
 */
inline std::string toString(ViewControlType type) { return ((type == ViewControlType::ORBIT) ? "orbit" : "ortho"); }

/** @brief The InteractiveViewControl private data structure */
class InteractiveViewControlPrivate;

/**
 *  @brief This Plugin allows to control a user camera with the mouse:
 *  @details
 *    * Drag left button to pan
 *    * Drag middle button to orbit
 *    * Drag right button or scroll wheel to zoom
 *
 *    Supported options are:
 *
 *    * `orbit`: perspective projection
 *    * `ortho`: orthographic projection
 */
class InteractiveViewControl : public QObject
{
  Q_OBJECT
public:
  using Ptr = std::shared_ptr<InteractiveViewControl>;
  using ConstPtr = std::shared_ptr<const InteractiveViewControl>;

  /** @brief Constructor */
  InteractiveViewControl(const std::string& scene_name, ViewControlType type = ViewControlType::ORBIT);

  /** @brief Destructor */
  virtual ~InteractiveViewControl();

  /**
   * @brief Set the view controller to use
   * @param type The view controller type
   */
  void setViewController(ViewControlType type);

private:
  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;

  /** @brief Pointer to private data. */
  std::unique_ptr<InteractiveViewControlPrivate> data_;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_RENDERING_INTERACTIVE_VIEW_CONTROL_H
