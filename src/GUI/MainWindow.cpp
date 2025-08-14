#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "TestSolver.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), test_interface_(), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);
  auto scroll_content = new QWidget;
  auto stack = new QVBoxLayout(scroll_content);
  ui_->scrollArea->setWidget(scroll_content);
  for (size_t i = 0; i < 50; i++) {
    auto process = std::make_unique<Process>(Process::ProcType::kTestProc, new TestSolvable(1), &test_interface_, this);
    stack->insertWidget(0, process.get());
    procs_.emplace(std::move(process));
  }
}

MainWindow::~MainWindow() {
  delete ui_;
}