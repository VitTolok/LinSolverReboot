#pragma once
#include "Process.hpp"
#include "ui_MainWindow.h"
#include "TestInterface.hpp"
#include <memory>
#include <qt6/QtWidgets/QMainWindow>
#include <set>

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
  std::set<std::unique_ptr<Process>> procs_;
  TestInterface test_interface_;
};