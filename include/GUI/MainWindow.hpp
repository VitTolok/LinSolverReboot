#pragma once
#include "Process.hpp"
#include "ui_MainWindow.h"
#include <memory>
#include <qt6/QtWidgets/QMainWindow>
#include <set>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

private slots:
  void dummySlot() {}

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private:
  Ui::MainWindow *ui;
  std::set<Process> procs_;
};