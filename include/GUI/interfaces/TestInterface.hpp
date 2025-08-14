#pragma once
#include "Interface.hpp"

class TestInterface : public Interface {
  Q_OBJECT
 public:
  explicit TestInterface(QWidget *parent = nullptr) : Interface(parent) {
  }
  virtual void ActivateInProgress(const Solvable *) override {
  }
  virtual void ActivateSuccess(const Solvable *, const Solution *) override {
  }
  virtual void ActivateError(const Solvable *, const std::exception *) override {
  }
  virtual QProgressBar *ProgressBar() override {
    return nullptr;
  }
  virtual ~TestInterface() override {
  }

 private:
};