#pragma once
#include "Solver.hpp"
#include "decomp.hpp"

template <class Numeric> struct DecompSolvable : public Solvable {
  Matrix<Numeric> matrix;
  virtual ~DecompSolvable() override = default;
};

template <class Numeric> struct DecompSolution : public Solution {
  DecompSolution(const Matrix<Numeric> &matrix) : decomp(matrix) {}
  Decompose<Numeric> decomp;
  virtual ~DecompSolution() override = default;
};

template <class Numeric> struct DecompSolver : public Solver {
public:
  DecompSolver() : progress_(0), finished_(false), solution_() {}

  virtual void operator()(std::unique_ptr<Solvable> &&ptr) override {
    auto solvable = dynamic_cast<DecompSolvable<Numeric> *>(ptr.get());
    if (solvable == nullptr) {
      throw InvalidSolutionType();
    }
    solution_ = std::make_unique<DecompSolution<Numeric>>(solvable->matrix);
    finished_ = true;
  }

  virtual int16_t Progress() const override { return progress_; }

  const Solution *GetResult() const override { return solution_.get(); }

  virtual bool Finished() const override { return finished_; }

  virtual ~DecompSolver() override = default;

private:
  std::unique_ptr<DecompSolution<Numeric>> solution_;
  int16_t progress_ = 0;
  bool finished_ = false;
};

std::unique_ptr<Solver> MakeDecompSolver(const Solvable *ptr) {
  if (dynamic_cast<const DecompSolvable<double> *>(ptr) != nullptr) {
    return std::make_unique<DecompSolver<double>>();
  }
  throw InvalidSolutionType();
}