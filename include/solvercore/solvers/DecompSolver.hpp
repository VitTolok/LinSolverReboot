#pragma once
#include "Solver.hpp"
#include "decomp.hpp"

template <class Numeric>
struct DecompSolvable : public Solvable {
  Matrix<Numeric> matrix;
  virtual ~DecompSolvable() override = default;
};

template <class Numeric>
struct DecompSolution : public Solution {
  DecompSolution(const Matrix<Numeric> &matrix) : decomp(matrix) {
  }
  Decompose<Numeric> decomp;
  virtual ~DecompSolution() override = default;
};

template <class Numeric>
struct DecompSolver : public Solver {
 public:
  DecompSolver(Progress &progress, const std::function<void(Solution &&)> &finisher,
               const std::function<void(std::exception &&)> &terminator)
      : Solver(progress, finisher, terminator) {
  }

  virtual void operator()(const Solvable &solvable) const override {
    auto decomp_solvable = dynamic_cast<const DecompSolvable<Numeric> *>(&solvable);
    if (decomp_solvable == nullptr) {
      throw InvalidSolutionType();
    }
    try {
      Finish(DecompSolution<Numeric>(decomp_solvable->matrix));
    } catch (std::exception &e) {
      Terminate(std::move(e));
    }
  }

  virtual ~DecompSolver() override = default;

 private:
  std::unique_ptr<DecompSolution<Numeric>> solution_;
};

std::unique_ptr<Solver> MakeDecompSolver(Progress &progress, const std::function<void(Solution &&)> &finisher,
                                         const std::function<void(std::exception &&)> &terminator,
                                         const Solvable &solvable) {
  if (dynamic_cast<const DecompSolvable<double> *>(&solvable) != nullptr) {
    return std::make_unique<DecompSolver<double>>(progress, finisher, terminator);
  }
  throw InvalidSolutionType();
}