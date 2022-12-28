#include <tesseract_qt/collision/contact_results_compute_widget.h>
#include <tesseract_qt/collision/contact_results_model.h>
#include <tesseract_qt/collision/contact_results_events.h>
#include "ui_contact_results_compute_widget.h"

#include <QTreeView>

namespace tesseract_gui
{
ContactResultsComputeWidget::ContactResultsComputeWidget(QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::ContactResultsComputeWidget>())
{
  ctor(ComponentInfo());
}

ContactResultsComputeWidget::ContactResultsComputeWidget(ComponentInfo component_info, QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::ContactResultsComputeWidget>())
{
  ctor(std::move(component_info));
}

ContactResultsComputeWidget::~ContactResultsComputeWidget() = default;

void ContactResultsComputeWidget::setComponentInfo(ComponentInfo component_info)
{
  ui->contact_results_widget->setComponentInfo(component_info);
}

const ComponentInfo& ContactResultsComputeWidget::getComponentInfo() const
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
  tesseract_collision::CollisionCheckConfig config;
  config.contact_manager_config = tesseract_collision::ContactManagerConfig(ui->contact_threshold->value());
  config.contact_request.calculate_distance = ui->calculate_distance->isChecked();
  config.contact_request.calculate_penetration = ui->calculate_penetration->isChecked();
  config.contact_request.type =
      static_cast<tesseract_collision::ContactTestType>(ui->contact_test_type->currentIndex());

  QApplication::sendEvent(qApp, new events::ContactResultsCompute(getComponentInfo(), config));

  //  tesseract_collision::ContactResultMap contacts;
  //  auto contact_manager = data_->env->getDiscreteContactManager();
  //  contact_manager->applyContactManagerConfig(config);
  //  contact_manager->contactTest(contacts, request);

  //  // Convert to tracked objects
  //  ContactResultMap tracked_object;
  //  for (const auto& contact : contacts)
  //  {
  //    ContactResultVector crv;
  //    for (const auto& result : contact.second)
  //      crv().push_back(ContactResult(result));

  //    tracked_object[contact.first] = crv;
  //  }

  //  QApplication::sendEvent(qApp, new events::ContactResultsSet(data_->model->getComponentInfo(), tracked_object));

  //  // This hides the root element
  //  ui->tree_view->setRootIndex(data_->model->index(0, 0));

  //  ui->tree_view->expandToDepth(1);
}

void ContactResultsComputeWidget::ctor(ComponentInfo component_info)
{
  ui->setupUi(this);

  ui->contact_results_widget->setComponentInfo(component_info);

  connect(ui->compute_push_button, SIGNAL(clicked()), this, SLOT(onComputeClicked()));
}

}  // namespace tesseract_gui
