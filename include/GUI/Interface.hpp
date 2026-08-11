#pragma once
#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QComboBox>
#include <QMutex>
#include "Solver.hpp"
#include <mutex>
#include "Process.hpp"

class WrongInterface : public MachineError<WrongInterface> {
 public:
  WrongInterface() : MachineError("Wrong Interface") {
  }
};

class Interface : public QWidget {
  Q_OBJECT

 public slots:
  virtual void SetProgress(int16_t) = 0;

 public:
  explicit Interface(QWidget *parent = nullptr) : QWidget(parent) {
  }
  virtual void ActivateInProgress(const Solvable &) = 0;
  virtual void ActivateSuccess(const Solvable &, const ISolution &) = 0;
  virtual void ActivateError(const Solvable &, const IMachineError &) = 0;
  virtual std::unique_ptr<Process> MakeProcess(QWidget *parent) = 0;
  virtual QPushButton *ExecuteButton() = 0;
  virtual QString Name() const = 0;
  ~Interface() override = default;
};

inline void ChoiceBoxFillNumeric(QComboBox *ptr) {
  ptr->addItem("long double", static_cast<int32_t>(NumericType::kLongDouble));
  ptr->addItem("Rational", static_cast<int32_t>(NumericType::kRational));
}

inline void ChoiceBoxFillOperator(QComboBox *ptr) {
  ptr->addItem("+", static_cast<int32_t>(OperatorType::kAdd));
  ptr->addItem("-", static_cast<int32_t>(OperatorType::kSub));
  ptr->addItem("*", static_cast<int32_t>(OperatorType::kMul));
  ptr->addItem("/", static_cast<int32_t>(OperatorType::kDiv));
  ptr->addItem("%", static_cast<int32_t>(OperatorType::kMod));
}