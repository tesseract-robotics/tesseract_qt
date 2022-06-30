/**
 * @author Davide Faconti <davide.faconti@gmail.com>
 *
 * @copyright Copyright (C) 2015-2018 Davide Faconti <davide.faconti@gmail.com>
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
#ifndef TESSERACT_QT_PLOT_TRANSFORMS_TRANSFORM_SELECTOR_H
#define TESSERACT_QT_PLOT_TRANSFORMS_TRANSFORM_SELECTOR_H

#include <QDialog>
#include <memory>

namespace Ui
{
class transform_selector;
}

namespace tesseract_gui
{
class TransformSelector : public QDialog
{
  Q_OBJECT

public:
  TransformSelector(QStringList builtin_transform,
                    QStringList available_transforms,
                    QString* default_tansform,
                    std::map<std::string, QString>* curve_transforms,
                    QWidget* parent = nullptr);
  ~TransformSelector();

private slots:

  void on_buttonApplyDefault_clicked();

  void on_buttonResetAll_clicked();

  void on_transform_selector_accepted();

private:
  std::unique_ptr<Ui::transform_selector> ui;

  std::map<std::string, QString>* _curves_trans;

  QString* _default_trans;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_PLOT_TRANSFORMS_TRANSFORM_SELECTOR_H
