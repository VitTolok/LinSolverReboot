#include "Process.hpp"
#include "DecompSolver.hpp"
#include "TestSolver.hpp"
#include "ui_Process.h"

ProgressReport::ProgressReport(QProgressBar *ptr) : progress_bar_(ptr) {
}

void ProgressReport::operator=(int16_t) {
}

void Execute(Solver *solver, const Solvable *ptr) {
  (*solver)(ptr);
}

void Finisher(Process *process, Solution *solution) {
  process->solution_ = solution;
  process->finished_ = true;
}
void Terminator(Process *process, std::exception *exception) {
  process->error_ = exception;
  process->finished_ = true;
}

Process::Process(ProcType proc_type, Solvable *ptr, QProgressBar *progress_bar, QWidget *parent)
    : QWidget(parent)
    , progress_(progress_bar)
    , solvable_(ptr)
    , type_(proc_type)
    , start_(std::chrono::steady_clock::now())
    , finished_(false)
    , error_(nullptr)
    , solution_(nullptr) {
  switch (proc_type) {
    case ProcType::kTestProc:
      solver_ = std::make_unique<TestSolver>(
          &progress_, [this](Solution *s) -> void { Finisher(this, s); },
          [this](std::exception *e) -> void { Terminator(this, e); });
  }
  thread_ = std::thread(Execute, solver_.get(), ptr);
  ui_ = new Ui::Process;
  ui_->setupUi(this);
}

Process::ProcType Process::GetProcType() const {
  return type_;
}

bool Process::operator<(const Process &other) const {
  return start_ < other.start_;
}

Process::~Process() {
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