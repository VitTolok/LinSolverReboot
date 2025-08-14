#include "Process.hpp"
#include "DecompSolver.hpp"
#include "TestSolver.hpp"
#include "ui_Process.h"

ProgressReport::ProgressReport(QProgressBar *ptr) : progress_bar_(ptr) {
}

void ProgressReport::operator=(int16_t progress) {
  if (progress_bar_) {
    progress_bar_->setValue(progress);
  }
  progress_ = progress;
}

void ProgressReport::Report() {
  if (progress_bar_) {
    progress_bar_->setValue(progress_);
  }
}

void Execute(Solver *solver, const Solvable *ptr) {
  (*solver)(ptr);
}

void Finisher(Process *process, Solution *solution) {
  std::lock_guard lock(process->mutex_);
  process->solution_ = solution;
  process->finished_ = true;
  process->end_ = std::chrono::steady_clock::now();
  process->ui_->status->setText("Finished.");
}
void Terminator(Process *process, std::exception *exception) {
  std::lock_guard lock(process->mutex_);
  process->error_ = exception;
  process->finished_ = true;
  process->end_ = std::chrono::steady_clock::now();
}

Process::Process(ProcType proc_type, Solvable *ptr, Interface *interface, QWidget *parent)
    : QWidget(parent)
    , progress_(nullptr)
    , solvable_(ptr)
    , type_(proc_type)
    , start_(std::chrono::steady_clock::now())
    , finished_(false)
    , error_(nullptr)
    , solution_(nullptr)
    , mutex_() {
  std::lock_guard lock(mutex_);
  switch (proc_type) {
    case ProcType::kTestProc:
      solver_ = std::make_unique<TestSolver>(
          &progress_, [this](Solution *s) -> void { Finisher(this, s); },
          [this](std::exception *e) -> void { Terminator(this, e); });
  }
  ui_ = new Ui::Process;
  ui_->setupUi(this);
  ui_->status->setText("Running...");
  thread_ = std::thread(Execute, solver_.get(), ptr);
}

bool Process::operator<(const Process &other) const {
  return start_ < other.start_;
}

Process::~Process() {
  std::lock_guard lock(mutex_);
  if (thread_.joinable()) {
    thread_.join();
  }
  if (ui_) {
    delete ui_;
  }
  if (solvable_) {
    delete solvable_;
  }
  if (solution_) {
    delete solution_;
  }
  if (error_) {
    delete error_;
  }
}