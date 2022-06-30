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
#include <tesseract_qt/acm/add_allowed_collision_entry_dialog.h>
#include "ui_add_allowed_collision_entry_dialog.h"
#include <QRegExpValidator>
namespace tesseract_gui
{
AddAllowedCollisionEntryDialog::AddAllowedCollisionEntryDialog(QWidget* parent)
  : QDialog(parent), ui_(std::make_unique<Ui::AddAllowedCollisionEntryDialog>())
{
  ui_->setupUi(this);
  ui_->linkName1LineEdit->setValidator(new QRegExpValidator(QRegExp("\\S*")));
  ui_->linkName2LineEdit->setValidator(new QRegExpValidator(QRegExp("\\S*")));
  connect(ui_->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(ui_->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

AddAllowedCollisionEntryDialog::~AddAllowedCollisionEntryDialog() = default;

QString AddAllowedCollisionEntryDialog::getLinkName1() const { return ui_->linkName1LineEdit->text(); }

QString AddAllowedCollisionEntryDialog::getLinkName2() const { return ui_->linkName2LineEdit->text(); }

QString AddAllowedCollisionEntryDialog::getReason() const { return ui_->reasonLineEdit->text(); }

}  // namespace tesseract_gui
