#pragma once
#include "Process.hpp"
#include "ui_MainWindow.h"
#include "TestInterface.hpp"
#include <memory>
#include <qt6/QtWidgets/QMainWindow>
#include <QMutex>
#include <map>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 private slots:
  void dummySlot() {
  }

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

 private:
  Ui::MainWindow *ui_;
  std::map<const Process *, std::unique_ptr<Process>> procs_;
  TestInterface test_interface_;
  Process *active_process_;
  Interface *active_interface_;
  QVBoxLayout *stack_;

  void Deleter(Process *);
  void ActivateProcess(Process *);
  void AddProcess(Interface &);
  void SummonInterface(Interface *);
  void RegisterInterface(Interface *);
};