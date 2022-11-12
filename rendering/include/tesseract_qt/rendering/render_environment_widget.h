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
#ifndef TESSERACT_QT_RENDERING_RENDER_ENVIRONMENT_WIDGET_H
#define TESSERACT_QT_RENDERING_RENDER_ENVIRONMENT_WIDGET_H

#include <tesseract_qt/environment/environment_widget.h>
#include <tesseract_qt/common/entity_manager.h>
#include <tesseract_qt/common/entity_container.h>
#include <unordered_map>

namespace tesseract_gui
{
class RenderEnvironmentWidget : public EnvironmentWidget
{
  Q_OBJECT
public:
  explicit RenderEnvironmentWidget(std::string scene_name, EntityManager& entity_manager, QWidget* parent = nullptr);
  ~RenderEnvironmentWidget() override;

  tesseract_gui::EnvironmentWidget* clone() const override;

private Q_SLOTS:
  void onEnvironmentSet(const std::shared_ptr<tesseract_environment::Environment>& env);
  void onEnvironmentChanged(const tesseract_environment::Environment& env);
  void onEnvironmentCurrentStateChanged(const tesseract_environment::Environment& env);
  void onLinkVisibilityChanged(const std::vector<std::string>& links);
  void onShowContactResults(const tesseract_collision::ContactResultVector& contact_results);

protected:
  std::string container_name_;
  std::string scene_name_;
  EntityContainer::Ptr entity_container_;
  long render_revision_{ 0 };
  std::chrono::system_clock::time_point render_state_timestamp_{ std::chrono::system_clock::now() };
  std::vector<std::string> render_link_names_;
  bool render_dirty_{ true };
  bool render_reset_{ true };
  bool render_state_dirty_{ true };
  std::vector<std::string> link_visibility_properties_changed_;

  /** @brief The contact results to show */
  bool render_contact_results_dirty_{ false };
  tesseract_collision::ContactResultVector contact_results_changed_;

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_RENDERING_RENDER_ENVIRONMENT_WIDGET_H
