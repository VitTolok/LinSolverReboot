#include "Process.hpp"
#include "DecompSolver.hpp"

void Execute(Solver *solver, std::unique_ptr<Solvable> &&ptr) {
  (*solver)(std::move(ptr));
}

Process::Process(ProcType proc_type, std::unique_ptr<Solvable> &&ptr) {
  type_ = proc_type;
  switch (proc_type) {
  case ProcType::kDecompProc:
    solver_ = MakeDecompSolver(ptr.get());
    break;
  default:
    throw InvalidSolutionType();
  }
  thread_ = std::thread(Execute, solver_.get(), std::move(ptr));
}

int16_t Process::Progress() const { return solver_->Progress(); }

bool Process::Finished() const { return solver_->Finished(); }

const Solution *Process::GetSolution() const { return solver_->GetResult(); }

Process::ProcType Process::GetProcType() const { return type_; }
