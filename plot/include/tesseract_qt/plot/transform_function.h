/**
 * @author Davide Faconti <davide.faconti@gmail.com>
 *
 * @copyright Copyright (C) 2015-2018 Davide Faconti <davide.faconti@gmail.com>
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
#ifndef TESSERACT_QT_PLOT_TRANSFORM_FUNCTION_H
#define TESSERACT_QT_PLOT_TRANSFORM_FUNCTION_H

#include <QtCore>
#include <set>
#include <vector>
#include <functional>
#include <tesseract_qt/plot/plot_data.h>

namespace tesseract_gui
{
/** @brief Generic interface for a multi input - multi output transformation function.
 * Contrariwise to other plugins, multiple instances of the this class might be created.
 * For this reason, a TransformFactory is also defined
 */
class TransformFunction : public QObject
{
  Q_OBJECT
public:
  using Ptr = std::shared_ptr<TransformFunction>;

  TransformFunction();

  virtual ~TransformFunction() = default;

  virtual const char* name() const = 0;

  /** Number of inputs. Return -1 if it is not a constant.
   *
   * When numInputs() > 0, then the data will be initialized using
   * the method:
   *     setDataSource(const std::vector<const PlotData*>& src_data)
   *
   * When  numInputs() == -1, then the number of inputs is undefined and the
   * data will be initialized using the method_
   *     setDataSource( PlotDataMapRef* data )
   */
  virtual int numInputs() const = 0;

  /** Number of outputs. Define the size of the vector used in:
   *     calculate(std::vector<PlotData*>& dst_data)
   */
  virtual int numOutputs() const = 0;

  /** Clear the cache, state and any stored data */
  virtual void reset() {}

  PlotDataMapRef* plotData() { return _data; }

  std::vector<const PlotData*>& dataSources();

  virtual void setData(PlotDataMapRef* data,
                       const std::vector<const PlotData*>& src_vect,
                       std::vector<PlotData*>& dst_vect);

  virtual void calculate() = 0;

  unsigned order() const { return _order; }

  virtual QWidget* optionsWidget() { return nullptr; }

Q_SIGNALS:
  void parametersChanged();

protected:
  std::vector<const PlotData*> _src_vector;
  std::vector<PlotData*> _dst_vector;
  PlotDataMapRef* _data;

  unsigned _order;
};

using TransformsMap = std::unordered_map<std::string, std::shared_ptr<TransformFunction>>;

/// Simplified version with Single input and Single output
class TransformFunction_SISO : public TransformFunction
{
  Q_OBJECT
public:
  TransformFunction_SISO() = default;

  void reset() override;

  int numInputs() const override { return 1; }

  int numOutputs() const override { return 1; }

  void calculate() override;

  /// Method to be implemented by the user to apply a statefull function to each point.
  /// Index will increase monotonically, unless reset() is used.
  virtual std::optional<PlotData::Point> calculateNextPoint(size_t index) = 0;

  const PlotData* dataSource() const;

protected:
  double _last_timestamp = -std::numeric_limits<double>::max();
};

class TransformFactory : public QObject
{
public:
  TransformFactory() {}

private:
  TransformFactory(const TransformFactory&) = delete;
  TransformFactory& operator=(const TransformFactory&) = delete;

  std::map<std::string, std::function<TransformFunction::Ptr()>> creators_;
  std::set<std::string> names_;

  static TransformFactory* instance();

public:
  static const std::set<std::string>& registeredTransforms();

  template <typename T>
  static void registerTransform()
  {
    T temp;
    std::string name = temp.name();
    instance()->names_.insert(name);
    instance()->creators_[name] = []() { return std::make_shared<T>(); };
  }

  static TransformFunction::Ptr create(const std::string& name);
};

}  // namespace tesseract_gui

Q_DECLARE_OPAQUE_POINTER(tesseract_gui::TransformFactory*)
Q_DECLARE_METATYPE(tesseract_gui::TransformFactory*)
Q_GLOBAL_STATIC(tesseract_gui::TransformFactory, transform_factory_ptr_from_macro_)

#endif  // TESSERACT_QT_PLOT_TRANSFORM_FUNCTION_H
