#include "DecompInterface.hpp"
#include "ui_DecompInterface.h"
#include "DecompSolver.hpp"
#include "sstream"

using TypeIdentifyer = DecompSolvableBase::NumericType;

void DecompInterface::SetProgress(int16_t progress) {
  ui_->progressBar->setValue(progress);
}

DecompInterface::DecompInterface(QWidget *parent) : Interface(parent), ui_(new Ui::DecompInterface{}) {
  ui_->setupUi(this);
  ui_->type_choice_combo_box->addItem("double", static_cast<int32_t>(TypeIdentifyer::kDouble));
  ui_->type_choice_combo_box->addItem("rational", static_cast<int32_t>(TypeIdentifyer::kRational));
}

void DecompInterface::ActivateInProgress(const Solvable &solvable) {
  auto ptr = dynamic_cast<const DecompSolvableBase *>(&solvable);
  if (ptr == nullptr) {
    throw WrongInterface{};
  }
  switch (ptr->numeric_type) {
    case TypeIdentifyer::kDouble:
      ui_->type_choice_combo_box->setCurrentIndex(
          ui_->type_choice_combo_box->findData(static_cast<int32_t>(TypeIdentifyer::kDouble)));
      break;
    case TypeIdentifyer::kRational:
      ui_->type_choice_combo_box->setCurrentIndex(
          ui_->type_choice_combo_box->findData(static_cast<int32_t>(TypeIdentifyer::kRational)));
      break;
    default:
      throw WrongInterface{};
  }
  ui_->source_matrix_text_edit->setPlainText(QString::fromStdString(ptr->Print()));
  ui_->status->setText("Running...");
}

template <class Numeric>
void PrintSolution(Ui::DecompInterface *ui, const DecompSolution<Numeric> &sol) {
  ui->solved_text_browser->setPlainText(QString::fromStdString((std::ostringstream() << sol.decomp.simple).str()));
  ui->det_text_browser->setPlainText(
      QString::fromStdString(sol.decomp.det.has_value() ? (std::ostringstream() << *sol.decomp.det).str() : "DNE"));
  ui->lower_text_browser->setPlainText(
      QString::fromStdString(sol.decomp.lower.has_value() ? (std::ostringstream() << *sol.decomp.lower).str() : "DNE"));
  ui->upper_text_browser->setPlainText(
      QString::fromStdString(sol.decomp.upper.has_value() ? (std::ostringstream() << *sol.decomp.upper).str() : "DNE"));
}

void DecompInterface::ActivateSuccess(const Solvable &solvable, const Solution &solution) {
  auto ptr = dynamic_cast<const DecompSolvableBase *>(&solvable);
  if (ptr == nullptr) {
    throw WrongInterface{};
  }
  switch (ptr->numeric_type) {
    case TypeIdentifyer::kDouble:

      ui_->type_choice_combo_box->setCurrentIndex(
          ui_->type_choice_combo_box->findData(static_cast<int32_t>(TypeIdentifyer::kDouble)));
      PrintSolution(ui_, dynamic_cast<const DecompSolution<long double> &>(solution));
      break;
    case TypeIdentifyer::kRational:
      ui_->type_choice_combo_box->setCurrentIndex(
          ui_->type_choice_combo_box->findData(static_cast<int32_t>(TypeIdentifyer::kRational)));
      PrintSolution(ui_, dynamic_cast<const DecompSolution<Rational> &>(solution));
      break;
    default:
      throw WrongInterface{};
  }
  ui_->source_matrix_text_edit->setPlainText(QString::fromStdString(ptr->Print()));
  ui_->status->setText("Finished.");
}

void DecompInterface::ActivateError(const Solvable &solvable, const MachineError &error) {
  ActivateInProgress(solvable);
  ui_->status->setText(QString::fromStdString(std::string("Error: ") + typeid(error).name() + ": " + error.what()));
}

std::unique_ptr<Process> DecompInterface::MakeProcess(QWidget *parent) {
  switch (static_cast<TypeIdentifyer>(ui_->type_choice_combo_box->currentData().toInt())) {
    case TypeIdentifyer::kDouble:
      return std::make_unique<Process>(
          Process::ProcType::kDecompProc,
          std::make_unique<DecompSolvable<long double>>(ui_->source_matrix_text_edit->toPlainText().toStdString(),
                                                        TypeIdentifyer::kDouble),
          *this, parent);
    case TypeIdentifyer::kRational:
      try {
        return std::make_unique<Process>(
            Process::ProcType::kDecompProc,
            std::make_unique<DecompSolvable<Rational>>(ui_->source_matrix_text_edit->toPlainText().toStdString(),
                                                       TypeIdentifyer::kRational),
            *this, parent);
      } catch (std::exception &error) {
        ui_->status->setText(
            QString::fromStdString(std::string("Error: ") + typeid(error).name() + ": " + error.what()));
      }
    default:
      throw MachineError{};
  }
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