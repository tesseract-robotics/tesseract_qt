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
#include <tesseract_qt/common/models/status_log_model.h>

#include <tesseract/common/logging.h>

#include <QApplication>
#include <QByteArray>
#include <QCoreApplication>
#include <gtest/gtest.h>

#include <thread>

namespace tesseract::gui
{
TEST(StatusLogModelUnit, AddsStructuredLogRecords)
{
  StatusLogModel first_model;
  StatusLogModel second_model;

  tesseract::common::LogRecord record;
  record.level = spdlog::level::warn;
  record.message = "Structured warning";
  tesseract::common::emitLogRecord(record);

  ASSERT_EQ(first_model.rowCount(), 1);
  EXPECT_EQ(first_model.index(0, 1).data().toString(), QStringLiteral("Warn"));
  EXPECT_EQ(first_model.index(0, 2).data().toString(), QStringLiteral("Structured warning"));
  ASSERT_EQ(second_model.rowCount(), 1);
  EXPECT_EQ(second_model.index(0, 2).data().toString(), QStringLiteral("Structured warning"));
}

TEST(StatusLogModelUnit, QueuesRecordsFromOtherThreads)
{
  StatusLogModel model;

  std::thread worker([]() {
    tesseract::common::LogRecord record;
    record.level = spdlog::level::err;
    record.message = "Worker error";
    tesseract::common::emitLogRecord(record);
  });
  worker.join();

  EXPECT_EQ(model.rowCount(), 0);
  QCoreApplication::processEvents();
  ASSERT_EQ(model.rowCount(), 1);
  EXPECT_EQ(model.index(0, 1).data().toString(), QStringLiteral("Error"));
  EXPECT_EQ(model.index(0, 2).data().toString(), QStringLiteral("Worker error"));
}
}  // namespace tesseract::gui

int main(int argc, char** argv)
{
  qputenv("QT_QPA_PLATFORM", QByteArray("offscreen"));
  QApplication application(argc, argv);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}