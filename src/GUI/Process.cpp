#include "Process.hpp"
#include "DecompSolver.hpp"
#include "TestSolver.hpp"
#include "Interface.hpp"
#include "ui_Process.h"

ProgressReport::ProgressReport(Interface &interface)
    : interface_(interface), active_(false), terminate_signal_(false), progress_(0) {
}

void ProgressReport::operator=(int16_t progress) {
  if (terminate_signal_) {
    throw TerminationBreak{};
  }
  if (active_) {
    interface_.ProgressBar()->setValue(progress);
  }
  progress_ = progress;
}

void ProgressReport::Report() {
  (*this) = progress_;
}

void ProgressReport::Terminate() {
  terminate_signal_ = true;
}

void Execute(Solver *solver, const Solvable *solvable) {
  (*solver)(*solvable);
}

void Finisher(Process &process, Solution &&solution) {
  process.progress_ = 100;
  std::lock_guard lock(process.mutex_);
  process.solution_ = std::make_unique<Solution>(std::move(solution));
  process.end_ = std::chrono::steady_clock::now();
  process.ui_->status->setText("Finished.");
}
void Terminator(Process &process, std::exception &&exception) {
  if (&dynamic_cast<const TerminationBreak &>(exception)) {
    return;
  }
  process.progress_.Report();
  std::lock_guard lock(process.mutex_);
  process.ui_->status->setText("Error.");
  process.error_ = std::make_unique<std::exception>(std::move(exception));
  process.end_ = std::chrono::steady_clock::now();
}

Process::Process(ProcType proc_type, std::unique_ptr<Solvable> &&ptr, Interface &interface, QWidget *parent)
    : QWidget(parent)
    , progress_(interface)
    , solvable_(std::move(ptr))
    , type_(proc_type)
    , start_(std::chrono::steady_clock::now())
    , error_(nullptr)
    , solution_(nullptr)
    , mutex_() {
  std::lock_guard lock(mutex_);
  switch (proc_type) {
    case ProcType::kTestProc:
      solver_ = std::make_unique<TestSolver>(
          progress_, [this](Solution &&s) -> void { Finisher(*this, std::move(s)); },
          [this](std::exception &&e) -> void { Terminator(*this, std::move(e)); });
  }
  ui_ = new Ui::Process;
  ui_->setupUi(this);
  ui_->status->setText("Running...");
  thread_ = std::thread(Execute, solver_.get(), solvable_.get());
}

const Ui::Process *Process::Ui() const {
  return ui_;
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