#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "TestSolver.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);
  auto scroll_content = new QWidget;
  auto stack = new QVBoxLayout(scroll_content);
  ui_->scrollArea->setWidget(scroll_content);
  for (size_t i = 0; i < 500; i++) {
    auto solvable = std::make_unique<TestSolvable>(1);
    auto process = std::make_unique<Process>(Process::ProcType::kTestProc, std::move(solvable), this);
    stack->insertWidget(0, process.get());
    process->setMinimumHeight(90);
    process->setMinimumWidth(100);
    procs_.emplace(std::move(process));
  }
}

MainWindow::~MainWindow() {
  delete ui_;
}