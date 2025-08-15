#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "ui_Process.h"
#include "TestSolver.hpp"

void MainWindow::Deleter(Process *ptr) {
  procs_.erase(ptr);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), test_interface_(), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);
  auto scroll_content = new QWidget;
  auto stack = new QVBoxLayout(scroll_content);
  ui_->scrollArea->setWidget(scroll_content);
  for (size_t i = 0; i < 50; i++) {
    auto process = std::make_unique<Process>(Process::ProcType::kTestProc, std::make_unique<TestSolvable>(1),
                                             test_interface_, this);
    stack->insertWidget(0, process.get());
    Process *ptr = process.get();
    procs_[ptr] = std::move(process);
    connect(ptr->Ui()->terminate_button, &QPushButton::clicked, [ptr, this]() -> void { Deleter(ptr); });
  }
}

MainWindow::~MainWindow() {
  delete ui_;
}