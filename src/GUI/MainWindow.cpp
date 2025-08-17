#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "ui_Process.h"
#include "TestSolver.hpp"

void MainWindow::Deleter(Process *ptr) {
  QMutexLocker lock(&mutex_);
  ptr->Ui()->terminate_button->setEnabled(false);
  procs_.erase(ptr);
}

void MainWindow::ActivateProcess(Process *process) {
  process->Ui()->show_button->setEnabled(false);
  if (procs_.find(process) == procs_.end()) {
    return;
  }
  if (procs_.find(active_process_) != procs_.end()) {
    active_process_->ReleaseInterface();
  }
  active_process_ = process;
  QMutexLocker lock(&mutex_);
  switch (process->GetProcType()) {
    case Process::ProcType::kTestProc:
      if (active_interface_ != &test_interface_) {
        ui_->horizontalLayout->removeWidget(active_interface_);
        ui_->horizontalLayout->insertWidget(0, &test_interface_);
        active_interface_ = &test_interface_;
      }
      break;
  }
  process->ReportToInterface();
  process->Ui()->show_button->setEnabled(true);
}

void MainWindow::AddProcess(Interface &interface) {
  Process *ptr = nullptr;
  {
    if (procs_.find(active_process_) != procs_.end()) {
      active_process_->ReleaseInterface();
      active_process_ = nullptr;
    }
    interface.ExecuteButton()->setEnabled(false);
    auto process = interface.MakeProcess(this);
    QMutexLocker lock(&interface.mutex);
    ptr = process.get();
    procs_[ptr] = std::move(process);
    connect(ptr->Ui()->terminate_button, &QPushButton::clicked, [this, ptr]() -> void { Deleter(ptr); });
    connect(ptr->Ui()->show_button, &QPushButton::clicked, [this, ptr]() -> void { ActivateProcess(ptr); });
    stack_->insertWidget(0, ptr);
    ptr->show();
  }
  ActivateProcess(ptr);
  interface.ExecuteButton()->setEnabled(true);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
    , test_interface_(mutex_, this)
    , active_process_(nullptr)
    , active_interface_(&test_interface_) {
  ui_->setupUi(this);
  ui_->horizontalLayout->insertWidget(0, &test_interface_);
  auto scroll_content = new QWidget;
  stack_ = new QVBoxLayout(scroll_content);
  ui_->scrollArea->setWidget(scroll_content);
  connect(test_interface_.ExecuteButton(), &QPushButton::clicked, [this]() -> void { AddProcess(test_interface_); });
  // following two lines are to be removed when interface summoning is added
  ui_->horizontalLayout->insertWidget(0, &test_interface_);
  active_interface_ = &test_interface_;
}

MainWindow::~MainWindow() {
  delete ui_;
}