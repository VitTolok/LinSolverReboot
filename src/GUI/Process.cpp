#include "Process.hpp"
#include "DecompSolver.hpp"

Process::Process(ProcType proc_type, std::unique_ptr<Solvable> &&ptr) {
  switch (proc_type) {
  case ProcType::kDecompProc:
    solver_ = MakeDecompSolver(ptr.get());
    break;
  default:
    throw InvalidSolutionType();
  }
  thread_ = std::thread(*solver_, ptr);
  type_ = proc_type;
}

int16_t Process::Progress() const { return solver_->Progress(); }

bool Process::Finished() const { return solver_->Finished(); }

const std::shared_ptr<Solution> &Process::GetSolution() const {
  return solver_->GetResult();
}

Process::ProcType Process::GetProcType() const { return type_; }
