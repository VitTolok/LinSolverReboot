#include "MatAriInterface.hpp"
#include "ui_MatAriInterface.h"
#include "MatAriSolver.hpp"

void MatAriInterface::SetProgress(int16_t) {
}

MatAriInterface::MatAriInterface(QWidget *parent) : Interface(parent), ui_(new Ui::MatAriInterface{}) {
  ui_->setupUi(this);
  ChoiceBoxFillOperator(ui_->operator_choice);
  ChoiceBoxFillNumeric(ui_->type_choice_1);
  ChoiceBoxFillNumeric(ui_->type_choice_2);
  ui_->plain_1->setChecked(false);
  ui_->plain_2->setChecked(false);
  ui_->status->setText("Idle");
}

void MatAriInterface::ActivateInProgress(const Solvable &solvable) {
  auto ptr = dynamic_cast<const MatAriSolvable *>(&solvable);
  if (ptr == nullptr) {
    throw WrongInterface{};
  }
  ui_->text_edit_1->setPlainText(ptr->first.c_str());
  ui_->text_edit_2->setPlainText(ptr->second.c_str());
  ui_->text_browser_result->clear();
  ui_->operator_choice->setCurrentIndex(ui_->operator_choice->findData(static_cast<int32_t>(ptr->operator_type)));
  ui_->type_choice_1->setCurrentIndex(ui_->type_choice_1->findData(static_cast<int32_t>(ptr->first_type)));
  ui_->type_choice_2->setCurrentIndex(ui_->type_choice_2->findData(static_cast<int32_t>(ptr->second_type)));
  ui_->plain_1->setChecked(ptr->first_plain);
  ui_->plain_2->setChecked(ptr->second_plain);
  ui_->result_type_label->setText("Result Type:");
  ui_->status->setText("In Progress...");
}

void MatAriInterface::ActivateSuccess(const Solvable &solvable, const ISolution &solution) {
  auto ptr = dynamic_cast<const MatAriSolutionBase *>(&solution);
  if (ptr == nullptr) {
    throw WrongInterface{};
  }
  ActivateInProgress(solvable);
  ui_->text_browser_result->setPlainText(QString::fromStdString(ptr->Print()));
  ui_->result_type_label->setText((ptr->TypeName()).data());
  ui_->status->setText("Finished");
}
void MatAriInterface::ActivateError(const Solvable &solvable, const IMachineError &error) {
  ActivateInProgress(solvable);
  ui_->status->setText(QString("Error: ").append(error.ErrorName()).append(' ').append(error.what()));
  ui_->text_browser_result->setText("Error");
}
std::unique_ptr<Process> MatAriInterface::MakeProcess(QWidget *parent) {
  return std::make_unique<Process>(
      Process::ProcType::kMatAriProc,
      std::make_unique<MatAriSolvable>(ui_->text_edit_1->toPlainText().toStdString(),
                                       ui_->text_edit_2->toPlainText().toStdString(),
                                       static_cast<NumericType>(ui_->type_choice_1->currentData().toInt()),
                                       static_cast<NumericType>(ui_->type_choice_2->currentData().toInt()),
                                       static_cast<OperatorType>(ui_->operator_choice->currentData().toInt()),
                                       ui_->plain_1->isChecked(), ui_->plain_2->isChecked()),
      *this, parent);
}
QPushButton *MatAriInterface::ExecuteButton() {
  return ui_->exec_button;
}
QString MatAriInterface::Name() const {
  return "Matrix Arithmetics";
}
MatAriInterface::~MatAriInterface() {
  delete ui_;
}