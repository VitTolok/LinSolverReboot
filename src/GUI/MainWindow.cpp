#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "ui_Process.h"
#include "TestSolver.hpp"

void MainWindow::Deleter(Process *process) {
  delete process;
  procs_.erase(process);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), test_interface_(), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);
  auto scroll_content = new QWidget;
  auto stack = new QVBoxLayout(scroll_content);
  ui_->scrollArea->setWidget(scroll_content);
  for (size_t i = 0; i < 50; i++) {
    auto process = new Process(Process::ProcType::kTestProc, new TestSolvable(1), &test_interface_, this);
    stack->insertWidget(0, process);
    procs_.emplace(process);
    connect(process->Ui()->terminate_button, &QPushButton::clicked, [process, this]() -> void { Deleter(process); });
  }
}

MainWindow::~MainWindow() {
  for (auto process : procs_) {
    delete process;
  }
  delete ui_;
}