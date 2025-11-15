#include "ui_MainWindow.h"
#include "MainWindow.hpp"
#include "ui_Process.h"
#include "TestSolver.hpp"

void MainWindow::Deleter(Process *ptr) {
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
  SummonInterface(process->GetInterface());
  process->ReportToInterface();
  process->Ui()->show_button->setEnabled(true);
}

void MainWindow::AddProcess(Interface &interface) {
  Process *ptr = nullptr;
  if (procs_.find(active_process_) != procs_.end()) {
    active_process_->ReleaseInterface();
    active_process_ = nullptr;
  }
  interface.ExecuteButton()->setEnabled(false);
  auto process = interface.MakeProcess(this);
  ptr = process.get();
  procs_[ptr] = std::move(process);
  connect(ptr->Ui()->terminate_button, &QPushButton::clicked, [this, ptr]() -> void { Deleter(ptr); });
  connect(ptr->Ui()->show_button, &QPushButton::clicked, [this, ptr]() -> void { ActivateProcess(ptr); });
  stack_->insertWidget(0, ptr);
  ptr->show();
  ActivateProcess(ptr);
  interface.ExecuteButton()->setEnabled(true);
}

void MainWindow::RegisterInterface(Interface *interface) {
  auto action = new QAction(ui_->menuinterface);
  action->setText(interface->Name());
  interface->hide();
  ui_->menuinterface->addAction(action);
  connect(action, &QAction::triggered, [this, interface]() -> void { SummonInterface(interface); });
  connect(interface->ExecuteButton(), &QPushButton::clicked, [this, interface]() -> void { AddProcess(*interface); });
}

void MainWindow::SummonInterface(Interface *interface) {
  if (active_interface_ == interface) {
    return;
  }
  if (active_interface_ != nullptr) {
    if (procs_.find(active_process_) != procs_.end()) {
      active_process_->ReleaseInterface();
    }
    active_interface_->hide();
    ui_->horizontalLayout->replaceWidget(active_interface_, interface);
  } else {
    ui_->horizontalLayout->insertWidget(0, interface);
  }
  interface->show();
  active_interface_ = interface;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
    , test_interface_(this)
    , decomp_interface_(this)
    , active_process_(nullptr)
    , active_interface_(nullptr) {
  ui_->setupUi(this);
  auto scroll_content = new QWidget;
  stack_ = new QVBoxLayout(scroll_content);
  ui_->scrollArea->setWidget(scroll_content);
  Interface *interfaces[]{&test_interface_, &decomp_interface_, &mat_ari_interface_};
  for (auto interface : interfaces) {
    RegisterInterface(interface);
  }
  SummonInterface(&test_interface_);
}

MainWindow::~MainWindow() {
  delete ui_;
}