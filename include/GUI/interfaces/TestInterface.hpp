#pragma once
#include "Interface.hpp"
#include "TestSolver.hpp"
#include "ui_TestInterface.h"

class TestInterface : public Interface {
  Q_OBJECT
 public:
  explicit TestInterface(QMutex &mutex, QWidget *parent = nullptr)
      : Interface(mutex, parent), ui_(new Ui::TestInterface()) {
    ui_->setupUi(this);
  }
  virtual void ActivateInProgress(const Solvable &solvable) override {
    auto ptr = dynamic_cast<const TestSolvable *>(&solvable);
    if (ptr == nullptr) {
      throw WrongInterface{};
    }
    ui_->spinBox->setValue(ptr->number);
    ui_->label->setText("In Progress...");
  }
  virtual void ActivateSuccess(const Solvable &solvable, const Solution &solution) override {
    auto solvable_ptr = dynamic_cast<const TestSolvable *>(&solvable);
    auto solution_ptr = dynamic_cast<const TestSolution *>(&solution);
    if (solvable_ptr == nullptr || solution_ptr == nullptr) {
      throw WrongInterface{};
    }
    ui_->spinBox->setValue(solvable_ptr->number);
    ui_->label->setText(QString::fromStdString("Result: " + solution_ptr->result));
  }
  virtual void ActivateError(const Solvable &solvable, const MachineError &error) override {
    auto solvable_ptr = dynamic_cast<const TestSolvable *>(&solvable);
    if (solvable_ptr == nullptr) {
      throw WrongInterface{};
    }
    ui_->spinBox->setValue(solvable_ptr->number);
    ui_->label->setText(QString::fromStdString(std::string("Error: ") + typeid(error).name() + ": " + error.what()));
  }
  virtual QPushButton *ExecuteButton() override {
    return ui_->pushButton;
  }
  virtual ~TestInterface() override {
    delete ui_;
  }
  virtual std::unique_ptr<Process> MakeProcess(QWidget *parent = nullptr) override {
    return std::make_unique<Process>(Process::ProcType::kTestProc,
                                     std::make_unique<TestSolvable>(ui_->spinBox->value()), *this, parent);
  }
  virtual void SetProgress(int16_t progress) override {
    ui_->progressBar->setValue(progress);
  }

 private:
  Ui::TestInterface *ui_;
};