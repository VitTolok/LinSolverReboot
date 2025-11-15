#include "DecompInterface.hpp"
#include "ui_DecompInterface.h"
#include "DecompSolver.hpp"
#include <sstream>

void DecompInterface::SetProgress(int16_t progress) {
  ui_->progressBar->setValue(progress);
}

DecompInterface::DecompInterface(QWidget *parent) : Interface(parent), ui_(new Ui::DecompInterface{}) {
  ui_->setupUi(this);
  ui_->type_choice_combo_box->addItem("double", static_cast<int32_t>(NumericType::kLongDouble));
  ui_->type_choice_combo_box->addItem("rational", static_cast<int32_t>(NumericType::kRational));
}

void DecompInterface::ActivateInProgress(const Solvable &solvable) {
  auto ptr = dynamic_cast<const DecompSolvable *>(&solvable);
  if (ptr == nullptr) {
    throw WrongInterface{};
  }
  switch (ptr->numeric_type) {
    case NumericType::kLongDouble:
      ui_->type_choice_combo_box->setCurrentIndex(
          ui_->type_choice_combo_box->findData(static_cast<int32_t>(NumericType::kLongDouble)));
      break;
    case NumericType::kRational:
      ui_->type_choice_combo_box->setCurrentIndex(
          ui_->type_choice_combo_box->findData(static_cast<int32_t>(NumericType::kRational)));
      break;
    default:
      throw WrongInterface{};
  }
  ui_->source_matrix_text_edit->setPlainText(QString::fromStdString(ptr->input));
  ui_->status->setText("Running...");
}

template <class Numeric>
void PrintSolution(Ui::DecompInterface *ui, const DecompSolution<Numeric> &sol) {
  ui->solved_text_browser->setPlainText(QString::fromStdString(sol.solved));
  ui->det_text_browser->setPlainText(QString::fromStdString(sol.det));
  ui->lower_text_browser->setPlainText(QString::fromStdString(sol.lower));
  ui->upper_text_browser->setPlainText(QString::fromStdString(sol.upper));
}

void DecompInterface::ActivateSuccess(const Solvable &solvable, const Solution &solution) {
  auto ptr = dynamic_cast<const DecompSolvable *>(&solvable);
  if (ptr == nullptr) {
    throw WrongInterface{};
  }
  switch (ptr->numeric_type) {
    case NumericType::kLongDouble:
      ui_->type_choice_combo_box->setCurrentIndex(
          ui_->type_choice_combo_box->findData(static_cast<int32_t>(NumericType::kLongDouble)));
      PrintSolution(ui_, dynamic_cast<const DecompSolution<long double> &>(solution));
      break;
    case NumericType::kRational:
      ui_->type_choice_combo_box->setCurrentIndex(
          ui_->type_choice_combo_box->findData(static_cast<int32_t>(NumericType::kRational)));
      PrintSolution(ui_, dynamic_cast<const DecompSolution<Rational> &>(solution));
      break;
    default:
      throw WrongInterface{};
  }
  ui_->source_matrix_text_edit->setPlainText(QString::fromStdString(ptr->input));
  ui_->status->setText("Finished.");
}

void DecompInterface::ActivateError(const Solvable &solvable, const MachineError &error) {
  ActivateInProgress(solvable);
  ui_->status->setText(QString::fromStdString(std::string("Error: ") + typeid(error).name() + ": " + error.what()));
}

std::unique_ptr<Process> DecompInterface::MakeProcess(QWidget *parent) {
  return std::make_unique<Process>(
      Process::ProcType::kDecompProc,
      std::make_unique<DecompSolvable>(ui_->source_matrix_text_edit->toPlainText().toStdString(),
                                       static_cast<NumericType>(ui_->type_choice_combo_box->currentData().toInt())),
      *this, parent);
}

QPushButton *DecompInterface::ExecuteButton() {
  return ui_->execute_button;
}

QString DecompInterface::Name() const {
  return "Decomposition";
}

DecompInterface::~DecompInterface() {
  delete ui_;
}