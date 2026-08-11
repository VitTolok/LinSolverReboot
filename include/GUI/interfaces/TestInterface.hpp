#pragma once
#include "Interface.hpp"
#include "TestSolver.hpp"
#include "ui_TestInterface.h"

class TestInterface : public Interface {
  Q_OBJECT
 public:
  explicit TestInterface(QWidget *parent = nullptr) : Interface(parent), ui_(new Ui::TestInterface()) {
    ui_->setupUi(this);
  }
  void ActivateInProgress(const Solvable &solvable) override {
    auto ptr = dynamic_cast<const TestSolvable *>(&solvable);
    if (ptr == nullptr) {
      throw WrongInterface{};
    }
    ui_->spinBox->setValue(ptr->number);
    ui_->label->setText("In Progress...");
  }
  void ActivateSuccess(const Solvable &solvable, const ISolution &solution) override {
    auto solvable_ptr = dynamic_cast<const TestSolvable *>(&solvable);
    auto solution_ptr = dynamic_cast<const TestSolution *>(&solution);
    if (solvable_ptr == nullptr || solution_ptr == nullptr) {
      throw WrongInterface{};
    }
    ui_->spinBox->setValue(solvable_ptr->number);
    ui_->label->setText(QString::fromStdString("Result: " + solution_ptr->result));
  }
  void ActivateError(const Solvable &solvable, const IMachineError &error) override {
    auto solvable_ptr = dynamic_cast<const TestSolvable *>(&solvable);
    if (solvable_ptr == nullptr) {
      throw WrongInterface{};
    }
    ui_->spinBox->setValue(solvable_ptr->number);
    ui_->label->setText(QString::fromStdString(std::string("Error: ") + error.ErrorName() + ": " + error.what()));
  }
  QPushButton *ExecuteButton() override {
    return ui_->pushButton;
  }
  ~TestInterface() override {
    delete ui_;
  }
  std::unique_ptr<Process> MakeProcess(QWidget *parent = nullptr) override {
    return std::make_unique<Process>(Process::ProcType::kTestProc,
                                     std::make_unique<TestSolvable>(ui_->spinBox->value()), *this, parent);
  }
  void SetProgress(int16_t progress) override {
    ui_->progressBar->setValue(progress);
  }

  QString Name() const override {
    return "Test";
  }

 private:
  Ui::TestInterface *ui_;
};