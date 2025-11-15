#pragma once
#include "Interface.hpp"

namespace Ui {
class MatAriInterface;
}

class MatAriInterface : public Interface {
  Q_OBJECT
 public:
  virtual void SetProgress(int16_t) override;
  explicit MatAriInterface(QWidget *parent = nullptr);
  virtual void ActivateInProgress(const Solvable &) override;
  virtual void ActivateSuccess(const Solvable &, const Solution &) override;
  virtual void ActivateError(const Solvable &, const MachineError &) override;
  virtual std::unique_ptr<Process> MakeProcess(QWidget *parent) override;
  virtual QPushButton *ExecuteButton() override;
  virtual QString Name() const override;
  virtual ~MatAriInterface() override;

 private:
  Ui::MatAriInterface *ui_;
};