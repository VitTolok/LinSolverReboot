#pragma once
#include "Interface.hpp"

namespace Ui {
class DecompInterface;
}

class DecompInterface : public Interface {
  Q_OBJECT
 public:
  virtual void SetProgress(int16_t) override;
  explicit DecompInterface(QWidget *parent = nullptr);
  virtual void ActivateInProgress(const Solvable &) override;
  virtual void ActivateSuccess(const Solvable &, const ISolution &) override;
  virtual void ActivateError(const Solvable &, const IMachineError &) override;
  virtual std::unique_ptr<Process> MakeProcess(QWidget *parent) override;
  virtual QPushButton *ExecuteButton() override;
  virtual QString Name() const override;
  virtual ~DecompInterface() override;

 private:
  Ui::DecompInterface *ui_;
};