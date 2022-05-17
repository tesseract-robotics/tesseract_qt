#ifndef TESSERACT_QT_PLOT_PLOT_WIDGET_EDITOR_H
#define TESSERACT_QT_PLOT_PLOT_WIDGET_EDITOR_H

#include <QDialog>
#include <QColorDialog>
#include <QKeyEvent>
#include <tesseract_qt/plot/plot_widget.h>
#include <tesseract_qt/plot/transform_function.h>

namespace Ui
{
class PlotWidgetEditor;
}

namespace tesseract_gui
{
class EditorRowWidget : public QWidget
{
  Q_OBJECT

public:
  EditorRowWidget(QString text, QColor color);

  void enterEvent(QEvent* ev) override;
  void leaveEvent(QEvent* ev) override;

  QString text() const;

  void setColor(QColor color);
  QColor color() const;

signals:

  void deleteRow(QWidget* _this);

private:
  QLabel* _text;
  QColor _color;
  QPushButton* _delete_button;
  QWidget* _empty_spacer;
};

class PlotWidgetEditor : public QDialog
{
  Q_OBJECT

public:
  explicit PlotWidgetEditor(PlotWidget* plotwidget, QWidget* parent = nullptr);
  ~PlotWidgetEditor();

public slots:
  void onColorChanged(QColor c);

private slots:

  void on_editColorButton_clicked();

  void on_editColorText_textChanged(const QString& arg1);

  void on_radioLines_toggled(bool checked);

  void on_radioPoints_toggled(bool checked);

  void on_radioBoth_toggled(bool checked);

  void on_checkBoxMax_toggled(bool checked);

  void on_checkBoxMin_toggled(bool checked);

  void on_pushButtonReset_clicked();

  void on_pushButtonCancel_pressed();

  void on_pushButtonSave_pressed();

  void on_listWidget_itemSelectionChanged();

  void on_lineLimitMin_textChanged(const QString& text);

  void on_lineLimitMax_textChanged(const QString& text);

  void on_radioSticks_toggled(bool checked);

private:
  std::unique_ptr<Ui::PlotWidgetEditor> ui;

  QColor _color{ Qt::blue };
  std::unique_ptr<PlotWidget> _plotwidget;
  PlotWidget* _plotwidget_origin;
  QRectF _bounding_rect_original;

  std::set<QWidget*> _connected_transform_widgets;

  void setupTable();
  void updateLimits();
  void onDeleteRow(QWidget* w);
  void disableWidgets();

  std::unordered_map<std::string, std::shared_ptr<TransformFunction>> _transforms;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_PLOT_PLOT_WIDGET_EDITOR_H
