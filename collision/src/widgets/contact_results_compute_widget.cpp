#include <tesseract_qt/collision/widgets/contact_results_compute_widget.h>
#include <tesseract_qt/collision/models/contact_results_model.h>
#include <tesseract_qt/common/events/contact_results_events.h>
#include "ui_contact_results_compute_widget.h"

#include <QTreeView>

namespace tesseract_gui
{
ContactResultsComputeWidget::ContactResultsComputeWidget(QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::ContactResultsComputeWidget>())
{
  ctor(nullptr);
}

ContactResultsComputeWidget::ContactResultsComputeWidget(std::shared_ptr<const ComponentInfo> component_info,
                                                         QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::ContactResultsComputeWidget>())
{
  ctor(std::move(component_info));
}

ContactResultsComputeWidget::~ContactResultsComputeWidget() = default;

void ContactResultsComputeWidget::setComponentInfo(std::shared_ptr<const ComponentInfo> component_info)
{
  ui->contact_results_widget->setComponentInfo(component_info);
}

std::shared_ptr<const ComponentInfo> ContactResultsComputeWidget::getComponentInfo() const
{
  return ui->contact_results_widget->getComponentInfo();
}

void ContactResultsComputeWidget::setModel(std::shared_ptr<ContactResultsModel> model)
{
  ui->contact_results_widget->setModel(model);
}
std::shared_ptr<ContactResultsModel> ContactResultsComputeWidget::getModel()
{
  return ui->contact_results_widget->getModel();
}
std::shared_ptr<const ContactResultsModel> ContactResultsComputeWidget::getModel() const
{
  return ui->contact_results_widget->getModel();
}

QItemSelectionModel& ContactResultsComputeWidget::getSelectionModel()
{
  return ui->contact_results_widget->getSelectionModel();
}
const QItemSelectionModel& ContactResultsComputeWidget::getSelectionModel() const
{
  return ui->contact_results_widget->getSelectionModel();
}

void ContactResultsComputeWidget::onComputeClicked()
{
  tesseract_collision::ContactManagerConfig contact_manager_config(ui->contact_threshold->value());
  tesseract_collision::CollisionCheckConfig collision_check_config;
  collision_check_config.contact_request.calculate_distance = ui->calculate_distance->isChecked();
  collision_check_config.contact_request.calculate_penetration = ui->calculate_penetration->isChecked();
  collision_check_config.contact_request.type =
      static_cast<tesseract_collision::ContactTestType>(ui->contact_test_type->currentIndex());

  events::ContactResultsCompute event(getComponentInfo(),
                                      contact_manager_config,
                                      collision_check_config,
                                      events::ContactResultsCompute::StateType::CURRENT_STATE,
                                      "Contact Results");
  QApplication::sendEvent(qApp, &event);
}

void ContactResultsComputeWidget::ctor(std::shared_ptr<const ComponentInfo> component_info)
{
  ui->setupUi(this);

  ui->contact_results_widget->setComponentInfo(std::move(component_info));

  connect(ui->compute_push_button, SIGNAL(clicked()), this, SLOT(onComputeClicked()));
}

}  // namespace tesseract_gui
