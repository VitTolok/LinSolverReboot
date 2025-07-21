#pragma once
#include "Solver.hpp"
#include <thread>

class Process {
public:
  enum class ProcType { kDecompProc };
  Process(ProcType, std::unique_ptr<Solvable> &&);
  int16_t Progress() const;
  bool Finished() const;
  const Solution *GetSolution() const;
  ProcType GetProcType() const;

private:
  ProcType type_;
  std::thread thread_;
  std::unique_ptr<Solver> solver_;
};