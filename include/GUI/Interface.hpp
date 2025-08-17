#pragma once
#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QMutex>
#include "Solver.hpp"
#include <mutex>
#include "Process.hpp"

class WrongInterface : public MachineError {};

class Interface : public QWidget {
  Q_OBJECT

 public:
  explicit Interface(QMutex &mutex, QWidget *parent = nullptr) : QWidget(parent), mutex(mutex) {
  }
  virtual void ActivateInProgress(const Solvable &) = 0;
  virtual void ActivateSuccess(const Solvable &, const Solution &) = 0;
  virtual void ActivateError(const Solvable &, const MachineError &) = 0;
  virtual std::unique_ptr<Process> MakeProcess(QWidget *parent) = 0;
  virtual QPushButton *ExecuteButton() = 0;
  virtual QProgressBar *ProgressBar() = 0;
  virtual ~Interface() {
  }
  QMutex &mutex;
};
