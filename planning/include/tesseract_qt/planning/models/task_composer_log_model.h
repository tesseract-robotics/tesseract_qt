/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2024 Levi Armstrong <levi.armstrong@gmail.com>
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
#ifndef TESSERACT_QT_PLANNING_TASK_COMPOSER_LOG_MODEL_H
#define TESSERACT_QT_PLANNING_TASK_COMPOSER_LOG_MODEL_H

#ifndef Q_MOC_RUN
#include <memory>
#include <tesseract_task_composer/core/fwd.h>
#include <QStandardItemModel>
#endif

namespace tesseract::gui
{
class TaskComposerLogModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit TaskComposerLogModel(QObject* parent = nullptr);
  ~TaskComposerLogModel() override;

  /**
   * @brief Add log
   * @param log The log associated with the namespace
   * @param ns The namespace to store the log under
   * @return The key associated with added log for removal
   */
  QString add(tesseract::task_composer::TaskComposerLog log, std::string ns = "general");

  /**
   * @brief Remove the log
   * @param key The key associated with the log to be removed
   */
  void remove(const QString& key);

  /**
   * @brief Check if a log with the provided key exists
   * @param key The key associated with the log to find
   * @return True if a log exists under the provided key, otherwise false
   */
  bool has(const QString& key);

  /**
   * @brief Get the log associated with the row
   * @param row The row to get associated log
   * @return The log
   */
  const tesseract::task_composer::TaskComposerLog& get(const QModelIndex& row) const;

  /**
   * @brief Get the log namespace associated with the row
   * @param row The row to get associated log
   * @return The namespace
   */
  const QString& getNamespace(const QModelIndex& row) const;

  /** @brief Clear the model */
  void clear();

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;
};

}  // namespace tesseract::gui

#endif  // TESSERACT_QT_PLANNING_TASK_COMPOSER_LOG_MODEL_H
