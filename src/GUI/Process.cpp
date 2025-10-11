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
    emit ReportProgress(progress);
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
  emit ReportProgress(progress_);
}

void ProgressReport::ReleaseInterface() {
  std::lock_guard lock(mutex_);
  active_ = false;
}

bool ProgressReport::Active() const {
  return active_;
}

void ProgressReport::SetSolver(Solver *solver, const Solvable *solvable) {
  solver_ = solver;
  solvable_ = solvable;
}

void ProgressReport::Execute() {
  if (active_) {
    emit ReportProgress(0);
  }
  try {
    (*solver_)(*solvable_);
  } catch (MachineError &exception) {
    solver_->Terminate(std::move(exception));
  } catch (...) {
    solver_->Terminate(MachineError{});
  }
  emit EndThread();
}

void Finisher(Process &process, Solution &&solution) {
  *process.progress_ = 100;
  std::lock_guard lock(process.mutex_);
  process.solution_ = solution.Detach();
  process.end_ = std::chrono::steady_clock::now();
  process.ui_->status->setText("Finished.");
}
void Terminator(Process &process, MachineError &&exception) {
  if (dynamic_cast<const TerminationBreak *>(&exception)) {
    return;
  }
  process.progress_->Report();
  std::lock_guard lock(process.mutex_);
  process.ui_->status->setText("Error.");
  process.error_ = exception.Detach();
  process.end_ = std::chrono::steady_clock::now();
}

Process::Process(ProcType proc_type, std::unique_ptr<Solvable> &&solvable, Interface &interface, QWidget *parent)
    : QWidget(parent)
    , progress_(std::make_unique<ProgressReport>(interface))
    , thread_(new QThread(this))
    , interface_(interface)
    , solvable_(std::move(solvable))
    , solver_(nullptr)
    , start_(std::chrono::steady_clock::now())
    , error_(nullptr)
    , solution_(nullptr)
    , mutex_() {
  std::lock_guard lock(mutex_);
  switch (proc_type) {
    case ProcType::kTestProc:
      solver_ = std::make_unique<TestSolver>(
          *progress_, [this](Solution &&s) -> void { Finisher(*this, std::move(s)); },
          [this](MachineError &&e) -> void { Terminator(*this, std::move(e)); });
      break;
    case ProcType::kDecompProc:
      solver_ = MakeDecompSolver(
          *progress_, [this](Solution &&solution) -> void { Finisher(*this, std::move(solution)); },
          [this](MachineError &&error) -> void { Terminator(*this, std::move(error)); }, *solvable_);
      break;
  }
  ui_ = new Ui::Process;
  ui_->setupUi(this);
  ui_->status->setText("Running...");
  progress_->SetSolver(solver_.get(), solvable_.get());
  progress_->moveToThread(thread_);
  connect(thread_, &QThread::started, progress_.get(), &ProgressReport::Execute);
  connect(progress_.get(), &ProgressReport::ReportProgress, &interface_, &Interface::SetProgress);
  connect(progress_.get(), &ProgressReport::EndThread, this, [this]() -> void { thread_->quit(); });
  connect(progress_.get(), &ProgressReport::EndThread, this, &Process::SoftReportToInterface);
  thread_->start();
}

const Ui::Process *Process::Ui() const {
  return ui_;
}

Interface *Process::GetInterface() {
  return &interface_;
}

Process::~Process() {
  std::lock_guard lock(mutex_);
  progress_->Terminate();
  delete ui_;
}

void Process::ReleaseInterface() {
  progress_->ReleaseInterface();
}

void Process::ReportToInterface() {
  progress_->ReportToInterface();
  if (solution_) {
    interface_.ActivateSuccess(*solvable_, *solution_);
  } else if (error_) {
    interface_.ActivateError(*solvable_, *error_);
  } else {
    interface_.ActivateInProgress(*solvable_);
  }
}

void Process::SoftReportToInterface() {
  if (progress_->Active()) {
    ReportToInterface();
  }
}