#ifndef TESSERACT_QT_PLOT_PLOT_WIDGET_TRANSFORMS_H
#define TESSERACT_QT_PLOT_PLOT_WIDGET_TRANSFORMS_H

#include <QDialog>
#include <tesseract_qt/plot/plot_widget.h>

namespace Ui
{
class PlotWidgetTransforms;
}

namespace tesseract_gui
{
class DialogTransformEditor : public QDialog
{
  Q_OBJECT

public:
  explicit DialogTransformEditor(PlotWidget* plotwidget);
  ~DialogTransformEditor();

private slots:

  void on_listCurves_itemSelectionChanged();

  void on_listTransforms_itemSelectionChanged();

  void on_pushButtonCancel_clicked();

  void on_pushButtonSave_clicked();

  void on_lineEditAlias_editingFinished();

private:
  std::unique_ptr<Ui::PlotWidgetTransforms> ui;

  std::unique_ptr<PlotWidget> _plotwidget;
  PlotWidget* _plotwidget_origin;

  std::set<QWidget*> _connected_transform_widgets;

  void setupTable();

  class RowWidget : public QWidget
  {
  public:
    RowWidget(QString text, QColor color);

    QString text() const;
    QColor color() const;

  private:
    QLabel* _text;
    QColor _color;
  };
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_PLOT_PLOT_WIDGET_TRANSFORMS_H
