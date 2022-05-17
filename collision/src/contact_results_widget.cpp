#include <tesseract_qt/collision/contact_results_widget.h>
#include <tesseract_qt/collision/contact_results_model.h>
#include <tesseract_environment/environment.h>
#include "ui_contact_results_widget.h"

namespace tesseract_gui
{
struct ContactResultsWidgetImpl
{
  std::shared_ptr<const tesseract_environment::Environment> env;
  ContactResultsModel* model;
};

ContactResultsWidget::ContactResultsWidget(QWidget* parent)
  : QWidget(parent)
  , ui(std::make_unique<Ui::ContactResultsWidget>())
  , data_(std::make_unique<ContactResultsWidgetImpl>())
{
  ui->setupUi(this);

  connect(ui->compute_push_button, SIGNAL(clicked()), this, SLOT(onComputeClicked()));
  connect(ui->tree_view,
          SIGNAL(showContactResults(tesseract_collision::ContactResultVector)),
          this,
          SIGNAL(showContactResults(tesseract_collision::ContactResultVector)));
}

ContactResultsWidget::~ContactResultsWidget() = default;

void ContactResultsWidget::setModel(ContactResultsModel* model)
{
  ui->tree_view->setModel(model);
  data_->model = model;
}

void ContactResultsWidget::setEnvironment(std::shared_ptr<const tesseract_environment::Environment> env)
{
  data_->env = std::move(env);
}

void ContactResultsWidget::onComputeClicked()
{
  if (data_->env == nullptr || !data_->env->isInitialized())
    return;

  tesseract_collision::ContactManagerConfig config(ui->contact_threshold->value());

  tesseract_collision::ContactRequest request;
  request.calculate_distance = ui->calculate_distance->isChecked();
  request.calculate_penetration = ui->calculate_penetration->isChecked();
  request.type = static_cast<tesseract_collision::ContactTestType>(ui->contact_test_type->currentIndex());

  tesseract_collision::ContactResultMap contacts;
  auto contact_manager = data_->env->getDiscreteContactManager();
  contact_manager->applyContactManagerConfig(config);
  contact_manager->contactTest(contacts, request);

  data_->model->setContactResults("Computed", contacts);
  ui->tree_view->expandToDepth(1);
}

}  // namespace tesseract_gui
