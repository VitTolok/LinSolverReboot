#pragma once
#include <QWidget>
#include <QProgressBar>
#include "Solver.hpp"

class Interface : public QWidget {
  Q_OBJECT

 public:
  explicit Interface(QWidget *parent = nullptr) : QWidget(parent) {
  }
  virtual void ActivateInProgress(const Solvable *) = 0;
  virtual void ActivateSuccess(const Solvable *, const Solution *) = 0;
  virtual void ActivateError(const Solvable *, const std::exception *) = 0;
  virtual QProgressBar *ProgressBar() = 0;
  virtual ~Interface() {
  }
};
