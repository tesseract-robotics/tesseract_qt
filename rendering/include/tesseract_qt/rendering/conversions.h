#ifndef TESSERACT_QT_RENDERING_CONVERSIONS_H
#define TESSERACT_QT_RENDERING_CONVERSIONS_H

#include <gz/common/KeyEvent.hh>
#include <gz/common/MouseEvent.hh>
#include <gz/math/Color.hh>
#include <gz/math/Vector2.hh>
#include <gz/math/Vector3.hh>

#include <QtCore>
#include <QColor>
#include <QVector3D>
#include <QInputEvent>

namespace tesseract_gui
{
/**
 * @brief Return the equivalent Qt color
 * @param color Ignition color to convert
 * @return Qt color value
 */
QColor convert(const gz::math::Color& color);

/**
 * @brief Return the equivalent Ignition color
 * @param color Qt color to convert
 * @return Ignition color value
 */
gz::math::Color convert(const QColor& color);

/**
 * @brief Return the equivalent QPointF.
 * @param pt Ignition vector to convert.
 * @return Qt point value
 */
QPointF convert(const gz::math::Vector2d& pt);

/**
 * @brief Return the equivalent Ignition vector.
 * @param pt Qt point to convert
 * @return Ignition Vector2d.
 */
gz::math::Vector2d convert(const QPointF& pt);

/**
 * @brief Return the equivalent Qt vector 3d.
 * @param vec Ignition vector 3d to convert.
 * @return Qt vector 3d value.
 */
QVector3D convert(const gz::math::Vector3d& vec);

/**
 * @brief Return the equivalent Ignition vector 3d.
 * @param vec Qt vector 3d to convert.
 * @return Ignition vector 3d value
 */
gz::math::Vector3d convert(const QVector3D& vec);

/**
 * @brief Return the equivalent Ignition mouse event.
 * @note Tthere isn't a 1-1 mapping between these types, so fields such as common::MouseEvent::PressPos need to be set
 * afterwards.
 * @param e Qt mouse event
 * @return Ignition mouse event
 */
gz::common::MouseEvent convert(const QMouseEvent& e);

/**
 * @brief Return the equivalent Ignition mouse event.
 * @note There isn't a 1-1 mapping between these types.
 * @param e Qt wheel event
 * @return Ignition mouse event
 */
gz::common::MouseEvent convert(const QWheelEvent& e);

/**
 * @brief Return the equivalent ignition key event.
 * @param e Qt key event
 * @return  Ignition key event
 */
gz::common::KeyEvent convert(const QKeyEvent& e);

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_RENDERING_CONVERSIONS_H
