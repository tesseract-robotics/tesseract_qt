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
#ifndef TESSERACT_QT_COMMON_CONTACT_RESULTS_RENDER_MANAGER_H
#define TESSERACT_QT_COMMON_CONTACT_RESULTS_RENDER_MANAGER_H

#include <memory>
#include <mutex>
#include <QObject>

namespace tesseract_gui::events
{
class ComponentEvent;
}

namespace tesseract_gui
{
struct ComponentInfo;

class ContactResultsRenderManager : public QObject
{
public:
  using Ptr = std::shared_ptr<ContactResultsRenderManager>;
  using ConstPtr = std::shared_ptr<const ContactResultsRenderManager>;
  using UPtr = std::unique_ptr<ContactResultsRenderManager>;
  using ConstUPtr = std::unique_ptr<const ContactResultsRenderManager>;

  ContactResultsRenderManager(std::shared_ptr<const ComponentInfo> component_info);
  ~ContactResultsRenderManager();

protected:
  std::shared_ptr<const ComponentInfo> component_info_;
  std::vector<std::unique_ptr<events::ComponentEvent>> events_;
  std::mutex mutex_;

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;

  virtual void render() = 0;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_COMMON_CONTACT_RESULTS_RENDER_MANAGER_H
