#include "Process.hpp"
#include "DecompSolver.hpp"
#include "TestSolver.hpp"
#include "Interface.hpp"
#include "ui_Process.h"

ProgressReport::ProgressReport(Interface &interface)
    : interface_(interface), active_(false), terminate_signal_(false), progress_(0), mutex_() {
}

void ProgressReport::operator=(int16_t progress) {
  std::lock_guard lock(mutex_);
  if (terminate_signal_) {
    throw TerminationBreak{};
  }
  if (active_) {
    QMutexLocker int_lock(&interface_.mutex);
    interface_.ProgressBar()->setValue(progress);
  }
  progress_ = progress;
}

void ProgressReport::Report() {
  (*this) = progress_;
}

void ProgressReport::Terminate() {
  std::lock_guard lock(mutex_);
  terminate_signal_ = true;
}

void ProgressReport::ReportToInterface() {
  std::lock_guard lock(mutex_);
  active_ = true;
  interface_.ProgressBar()->setValue(progress_);
}

void ProgressReport::ReleaseInterface() {
  std::lock_guard lock(mutex_);
  active_ = false;
}

bool ProgressReport::Active() const {
  return active_;
}

void Execute(Solver *solver, const Solvable *solvable) {
  try {
    (*solver)(*solvable);
  } catch (MachineError &exception) {
    solver->Terminate(std::move(exception));
  } catch (...) {
    solver->Terminate(MachineError{});
  }
}

void Finisher(Process &process, Solution &&solution) {
  process.progress_ = 100;
  std::lock_guard lock(process.mutex_);
  QMutexLocker int_lock(&process.interface_.mutex);
  process.solution_ = solution.Clone();
  process.end_ = std::chrono::steady_clock::now();
  process.ui_->status->setText("Finished.");
  if (process.progress_.Active()) {
    process.ReportToInterface();
  }
}
void Terminator(Process &process, MachineError &&exception) {
  if (dynamic_cast<const TerminationBreak *>(&exception)) {
    return;
  }
  process.progress_.Report();
  std::lock_guard lock(process.mutex_);
  QMutexLocker int_lock(&process.interface_.mutex);
  process.ui_->status->setText("Error.");
  process.error_ = exception.Clone();
  process.end_ = std::chrono::steady_clock::now();
  if (process.progress_.Active()) {
    process.ReportToInterface();
  }
}

Process::Process(ProcType proc_type, std::unique_ptr<Solvable> &&ptr, Interface &interface, QWidget *parent)
    : QWidget(parent)
    , progress_(interface)
    , interface_(interface)
    , solvable_(std::move(ptr))
    , type_(proc_type)
    , start_(std::chrono::steady_clock::now())
    , error_(nullptr)
    , solution_(nullptr)
    , mutex_() {
  std::lock_guard lock(mutex_);
  QMutexLocker int_lock(&interface.mutex);
  switch (proc_type) {
    case ProcType::kTestProc:
      solver_ = std::make_unique<TestSolver>(
          progress_, [this](Solution &&s) -> void { Finisher(*this, std::move(s)); },
          [this](MachineError &&e) -> void { Terminator(*this, std::move(e)); });
  }
  ui_ = new Ui::Process;
  ui_->setupUi(this);
  ui_->status->setText("Running...");
  thread_ = std::thread(Execute, solver_.get(), solvable_.get());
}

const Ui::Process *Process::Ui() const {
  return ui_;
}

Process::ProcType Process::GetProcType() const {
  return type_;
}

Process::~Process() {
  std::lock_guard lock(mutex_);
  progress_.Terminate();
  if (thread_.joinable()) {
    thread_.join();
  }
  if (ui_) {
    delete ui_;
  }
}

void Process::ReleaseInterface() {
  progress_.ReleaseInterface();
}

void Process::ReportToInterface() {
  if (solution_) {
    interface_.ActivateSuccess(*solvable_, *solution_);
  } else if (error_) {
    interface_.ActivateError(*solvable_, *error_);
  } else {
    interface_.ActivateInProgress(*solvable_);
  }
  progress_.ReportToInterface();
}