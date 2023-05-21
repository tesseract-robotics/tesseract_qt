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
#ifndef TESSERACT_QT_COLLISION_CONTACT_RESULTS_MODEL_H
#define TESSERACT_QT_COLLISION_CONTACT_RESULTS_MODEL_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <QStandardItemModel>
#include <QMetaType>
#include <memory>

#include <tesseract_qt/common/contact_results_types.h>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

namespace tesseract_gui
{
struct ComponentInfo;
class ContactResultsModel : public QStandardItemModel
{
  Q_OBJECT
public:
  ContactResultsModel(QObject* parent = nullptr);
  ContactResultsModel(std::shared_ptr<const ComponentInfo> component_info, QObject* parent = nullptr);
  ~ContactResultsModel() override;

  std::shared_ptr<const ComponentInfo> getComponentInfo() const;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

protected:
  struct Implementation;
  std::unique_ptr<Implementation> data_;

  void setContactResults(const QString& ns, const ContactResultVector& contact_results);
  void setContactResults(const QString& ns, const ContactResultMap& contact_results);
  void removeNamespace(const QString& ns);
  void clear();

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_COLLISION_CONTACT_RESULTS_MODEL_H
