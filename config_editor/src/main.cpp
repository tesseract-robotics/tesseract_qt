/*
 * @copyright Copyright (C) 2026 Levi Armstrong <levi.armstrong@gmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 */
#include "config_editor_main_window.h"

#include <QApplication>
#include <QMessageBox>

#include <exception>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
  QApplication application(argc, argv);
  QApplication::setOrganizationName(QStringLiteral("Tesseract Robotics"));
  QApplication::setApplicationName(QStringLiteral("Tesseract Configuration Editor"));

  try
  {
    const std::string initial_type = (argc > 1) ? argv[1] : "kinematics";
    const QString initial_path = (argc > 2) ? QString::fromLocal8Bit(argv[2]) : QString();
    std::vector<std::string> additional_search_paths;
    if (argc > 3)
      additional_search_paths.emplace_back(argv[3]);
    std::vector<std::string> additional_search_libraries;
    if (argc > 4)
    {
      for (const auto& library : QString::fromLocal8Bit(argv[4]).split(':', Qt::SkipEmptyParts))
        additional_search_libraries.push_back(library.toStdString());
    }

    tesseract::gui::ConfigEditorMainWindow window(
        initial_type, initial_path, std::move(additional_search_paths), std::move(additional_search_libraries));
    window.show();
    return QApplication::exec();
  }
  catch (const std::exception& exception)
  {
    QMessageBox::critical(nullptr, QObject::tr("Configuration Error"), QString::fromStdString(exception.what()));
    return 1;
  }
}
