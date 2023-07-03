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

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_task_composer/core/task_composer_problem.h>
#include <QStandardItemModel>
#include <memory>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

namespace tesseract_gui
{
struct TaskComposerProblemModelPrivate;
class TaskComposerProblemModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit TaskComposerProblemModel(QObject* parent = nullptr);
  ~TaskComposerProblemModel() override;

  /**
   * @brief Add problem
   * @param problem The problem associated with the key
   * @param ns The namespace to store the problem under
   * @return The key associated with added problem for removal
   */
  QString addProblem(tesseract_planning::TaskComposerProblem::UPtr problem, std::string ns = "general");

  /**
   * @brief Remove the problem
   * @param key The key associated with the problem to be removed
   */
  void removeProblem(const QString& key);

  /**
   * @brief Check if a problem with the provided key exists
   * @param key The key associated with the problem to find
   * @return True if a problem exists under the provided key, otherwise false
   */
  bool hasProblem(const QString& key);

  /**
   * @brief Get the problem associated with the row
   * @param row The row to get associated problem
   * @return The problem
   */
  const tesseract_planning::TaskComposerProblem& getProblem(const QModelIndex& row) const;

  /**
   * @brief Get the problem namespace associated with the row
   * @param row The row to get associated problem
   * @return The namespace
   */
  const QString& getProblemNamespace(const QModelIndex& row) const;

  /** @brief Clear the model */
  void clear();

private:
  std::unique_ptr<TaskComposerProblemModelPrivate> data_;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_PLANNING_TASK_COMPOSER_PROBLEM_MODEL_H
