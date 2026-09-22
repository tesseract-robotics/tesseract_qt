/*
 * @copyright Copyright (C) 2026 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU Lesser General Public License Version 3, 29 June 2007
 */
#ifndef TESSERACT_QT_CONFIG_EDITOR_MAIN_WINDOW_H
#define TESSERACT_QT_CONFIG_EDITOR_MAIN_WINDOW_H

#include <QMainWindow>

#include <memory>
#include <string>
#include <vector>

class QCloseEvent;

namespace tesseract::gui
{
/** @brief Standalone single-document shell for ConfigEditorWidget. */
class ConfigEditorMainWindow : public QMainWindow
{
public:
  ConfigEditorMainWindow(const std::string& initial_type,
                         const QString& initial_path,
                         std::vector<std::string> additional_search_paths,
                         std::vector<std::string> additional_search_libraries,
                         QWidget* parent = nullptr);
  ~ConfigEditorMainWindow() override;

  ConfigEditorMainWindow(const ConfigEditorMainWindow&) = delete;
  ConfigEditorMainWindow& operator=(const ConfigEditorMainWindow&) = delete;
  ConfigEditorMainWindow(ConfigEditorMainWindow&&) = delete;
  ConfigEditorMainWindow& operator=(ConfigEditorMainWindow&&) = delete;

protected:
  void closeEvent(QCloseEvent* event) override;

private:
  bool maybeSave();
  bool save();
  bool saveAs();
  void exportDraft();
  void openFile(const QString& path = {});
  void loadCurrentDefinition(const QString& path = {});
  void updateWindowState();
  void addRecentFile(const QString& path);
  void rebuildRecentFilesMenu();

  struct Implementation;
  std::unique_ptr<Implementation> data_;
};
}  // namespace tesseract::gui

#endif  // TESSERACT_QT_CONFIG_EDITOR_MAIN_WINDOW_H
