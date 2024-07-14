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
#ifndef TESSERACT_QT_PLANNING_TASK_COMPOSER_PROBLEM_MODEL_H
#define TESSERACT_QT_PLANNING_TASK_COMPOSER_PROBLEM_MODEL_H

#ifndef Q_MOC_RUN
#include <memory>
#include <tesseract_task_composer/core/fwd.h>
#include <QStandardItemModel>
#endif

namespace tesseract_gui
{
class TaskComposerDataStorageModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit TaskComposerDataStorageModel(QObject* parent = nullptr);
  ~TaskComposerDataStorageModel() override;

  /**
   * @brief Add data storage
   * @param data_storage The data storage associated with the namespace
   * @param ns The namespace to store the data storage under
   * @return The key associated with added data storage for removal
   */
  QString add(std::unique_ptr<tesseract_planning::TaskComposerDataStorage> data_storage, std::string ns = "general");

  /**
   * @brief Remove the data storage
   * @param key The key associated with the data storage to be removed
   */
  void remove(const QString& key);

  /**
   * @brief Check if a data storage with the provided key exists
   * @param key The key associated with the data storage to find
   * @return True if a data storage exists under the provided key, otherwise false
   */
  bool has(const QString& key);

  /**
   * @brief Get the data storage associated with the row
   * @param row The row to get associated data storage
   * @return The data storage
   */
  const tesseract_planning::TaskComposerDataStorage& get(const QModelIndex& row) const;

  /**
   * @brief Get the data storage namespace associated with the row
   * @param row The row to get associated data storage
   * @return The namespace
   */
  const QString& getNamespace(const QModelIndex& row) const;

  /** @brief Clear the model */
  void clear();

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_PLANNING_TASK_COMPOSER_PROBLEM_MODEL_H
