/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2025 Levi Armstrong <levi.armstrong@gmail.com>
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

#include <tesseract_qt/common/widgets/collapsible_group_box.h>
#include <qlayout.h>
#include <qstyleoption.h>
#include <qframe.h>

namespace tesseract::gui
{
CollapsibleGroupBox::CollapsibleGroupBox(QWidget* parent) : CollapsibleGroupBox("", parent) {}

CollapsibleGroupBox::CollapsibleGroupBox(const QString& title, QWidget* parent)
  : QGroupBox(title, parent), toggle_button_(new QToolButton(this))
{
  // Configure the toggle button
  toggle_button_->setToolButtonStyle(Qt::ToolButtonIconOnly);
  toggle_button_->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
  toggle_button_->setCheckable(true);
  toggle_button_->setChecked(false);
  toggle_button_->setAutoRaise(true);
  toggle_button_->setFocusPolicy(Qt::NoFocus);
  toggle_button_->setStyleSheet(R"(
    QToolButton {
        border: none;
        outline: none;
    }
)");
  // When the button is clicked, collapse/expand
  connect(toggle_button_, &QToolButton::toggled, this, &CollapsibleGroupBox::onToggled);
}

void CollapsibleGroupBox::setCollapsed(bool collapse)
{
  toggle_button_->setChecked(collapse);
  emit collapsed(collapse);
}

void CollapsibleGroupBox::resizeEvent(QResizeEvent* event)
{
  QGroupBox::resizeEvent(event);
  updateToggleButtonPosition();
}

void CollapsibleGroupBox::updateToggleButtonPosition()
{
  // Prepare style option
  QStyleOptionGroupBox opt;
  opt.initFrom(this);
  opt.text = title();
  opt.subControls = QStyle::SC_GroupBoxLabel;
  opt.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &opt, this);

  // Query style for label rectangle
  QRect label_rect = style()->subControlRect(QStyle::CC_GroupBox, &opt, QStyle::SC_GroupBoxLabel, this);

  // Place button at the right edge of the label
  QSize btn_size = toggle_button_->sizeHint();
  const int gap = 4;
  int x = label_rect.right() + gap;
  int y = label_rect.center().y() - btn_size.height() / 2;
  toggle_button_->setGeometry(x, y, btn_size.width(), btn_size.height());
}

void CollapsibleGroupBox::onToggled(bool checked)
{
  // flip arrow
  if (checked)
  {
    toggle_button_->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    setStyleSheet("QGroupBox { "
                  "  border: none; "
                  "}");
  }
  else
  {
    toggle_button_->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
    setStyleSheet("");
  }

  // hide/show all children except the toggle button
  const auto children = findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
  for (auto* w : children)
  {
    if (w != toggle_button_)
      w->setVisible(!checked);
  }
  if (layout())
    layout()->invalidate();

  emit collapsed(checked);
}
}  // namespace tesseract::gui
