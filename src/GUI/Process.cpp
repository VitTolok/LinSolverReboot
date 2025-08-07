#include "Process.hpp"
#include "DecompSolver.hpp"
#include "TestSolver.hpp"
#include "ui_Process.h"

void Execute(Solver *solver, std::unique_ptr<Solvable> &&ptr) {
  try {
    (*solver)(std::move(ptr));
  } catch (const std::exception &e) {
    qCritical() << "Solver error:" << e.what();
  }
}

Process::Process(ProcType proc_type, std::unique_ptr<Solvable> &&ptr, QWidget *parent) : QWidget(parent) {
  type_ = proc_type;
  switch (proc_type) {
    case ProcType::kDecompProc:
      solver_ = MakeDecompSolver(ptr.get());
      break;
    case ProcType::kTestProc:
      solver_ = std::make_unique<TestSolver>();
      break;
    default:
      throw InvalidSolutionType();
  }
  thread_ = std::thread(Execute, solver_.get(), std::move(ptr));
  ui = new Ui::Process;
  ui->setupUi(this);
}

int16_t Process::Progress() const {
  return solver_->Progress();
}

bool Process::Finished() const {
  return solver_->Finished();
}

const Solution *Process::GetSolution() const {
  return solver_->GetResult();
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
  delete ui;
}
