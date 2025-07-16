#pragma once
#include "Process.hpp"
#include <qt6/QtWidgets/QtWidgets>
#include <set>

class MainWindow {
public:
  MainWindow();

private:
  std::set<Process> procs_;
};