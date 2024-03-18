/*
 * Copyright (C) 2017 Open Source Robotics Foundation
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

#include <tesseract_qt/rendering/conversions.h>

#include <gz/common/KeyEvent.hh>
#include <gz/common/MouseEvent.hh>
#include <gz/math/Color.hh>

#include <QtCore>
#include <QColor>
#include <QVector3D>
#include <QInputEvent>

#include <QPointF>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

namespace tesseract_gui
{
//////////////////////////////////////////////////
QColor convert(const gz::math::Color& color)
{
  return QColor(color.R() * 255.0, color.G() * 255.0, color.B() * 255.0, color.A() * 255.0);
}

//////////////////////////////////////////////////
gz::math::Color convert(const QColor& color)
{
  return gz::math::Color(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0, color.alpha() / 255.0);
}

//////////////////////////////////////////////////
QPointF convert(const gz::math::Vector2d& pt) { return QPointF(pt.X(), pt.Y()); }

//////////////////////////////////////////////////
gz::math::Vector2d convert(const QPointF& pt) { return gz::math::Vector2d(pt.x(), pt.y()); }

//////////////////////////////////////////////////
QVector3D convert(const gz::math::Vector3d& vec) { return QVector3D(vec.X(), vec.Y(), vec.Z()); }

//////////////////////////////////////////////////
gz::math::Vector3d convert(const QVector3D& vec) { return gz::math::Vector3d(vec.x(), vec.y(), vec.z()); }

//////////////////////////////////////////////////
gz::common::MouseEvent convert(const QMouseEvent& e)
{
  gz::common::MouseEvent event;
  event.SetPos(e.pos().x(), e.pos().y());

  // Button
  if (e.button() == Qt::LeftButton)
    event.SetButton(gz::common::MouseEvent::LEFT);
  else if (e.button() == Qt::RightButton)
    event.SetButton(gz::common::MouseEvent::RIGHT);
  else if (e.button() == Qt::MiddleButton)
    event.SetButton(gz::common::MouseEvent::MIDDLE);

  // Buttons
  if (e.buttons() & Qt::LeftButton)
    event.SetButtons(event.Buttons() | gz::common::MouseEvent::LEFT);

  if (e.buttons() & Qt::RightButton)
    event.SetButtons(event.Buttons() | gz::common::MouseEvent::RIGHT);

  if (e.buttons() & Qt::MiddleButton)
    event.SetButtons(event.Buttons() | gz::common::MouseEvent::MIDDLE);

  // Type
  if (e.type() == QEvent::MouseButtonPress)
    event.SetType(gz::common::MouseEvent::PRESS);
  else if (e.type() == QEvent::MouseButtonRelease)
    event.SetType(gz::common::MouseEvent::RELEASE);
  else if (e.type() == QEvent::MouseMove)
  {
    event.SetType(gz::common::MouseEvent::MOVE);

    // Dragging
    if (e.buttons() || e.button())
      event.SetDragging(true);
  }

  // Modifiers
  if (e.modifiers() & Qt::ShiftModifier)
    event.SetShift(true);

  if (e.modifiers() & Qt::ControlModifier)
    event.SetControl(true);

  if (e.modifiers() & Qt::AltModifier)
    event.SetAlt(true);

  return event;
}

//////////////////////////////////////////////////
gz::common::MouseEvent convert(const QWheelEvent& e)
{
  gz::common::MouseEvent event;

  event.SetType(gz::common::MouseEvent::SCROLL);
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
  event.SetPos(e.x(), e.y());
#else
  event.SetPos(e.position().x(), e.position().y());
#endif
  double scroll = (e.angleDelta().y() > 0) ? -1.0 : 1.0;
  event.SetScroll(scroll, scroll);

  // Buttons
  if (e.buttons() & Qt::LeftButton)
    event.SetButtons(event.Buttons() | gz::common::MouseEvent::LEFT);

  if (e.buttons() & Qt::RightButton)
    event.SetButtons(event.Buttons() | gz::common::MouseEvent::RIGHT);

  if (e.buttons() & Qt::MiddleButton)
    event.SetButtons(event.Buttons() | gz::common::MouseEvent::MIDDLE);

  // Modifiers
  if (e.modifiers() & Qt::ShiftModifier)
    event.SetShift(true);

  if (e.modifiers() & Qt::ControlModifier)
    event.SetControl(true);

  if (e.modifiers() & Qt::AltModifier)
    event.SetAlt(true);

  return event;
}

//////////////////////////////////////////////////
gz::common::KeyEvent convert(const QKeyEvent& e)
{
  gz::common::KeyEvent event;
  event.SetKey(e.key());
  event.SetText(e.text().toStdString());

  if (e.type() == QEvent::KeyPress)
  {
    event.SetType(gz::common::KeyEvent::PRESS);
  }
  else if (e.type() == QEvent::KeyRelease)
  {
    event.SetType(gz::common::KeyEvent::RELEASE);
  }
  else
  {
    event.SetType(gz::common::KeyEvent::NO_EVENT);
  }

  event.SetControl(e.modifiers() & Qt::ControlModifier);
  event.SetShift(e.modifiers() & Qt::ShiftModifier);
  event.SetAlt(e.modifiers() & Qt::AltModifier);

  return event;
}

}  // namespace tesseract_gui
