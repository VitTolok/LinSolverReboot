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
  DecompSolver(Progress *progress, std::function<void(Solution *)> finisher,
               std::function<void(std::exception *)> terminator)
      : Solver(progress, finisher, terminator), finished_(false), solution_() {
  }

  virtual void operator()(const Solvable *ptr) const override {
    auto solvable = dynamic_cast<const DecompSolvable<Numeric> *>(ptr);
    if (solvable == nullptr) {
      throw InvalidSolutionType();
    }
    try {
      Finish(new DecompSolution<Numeric>(solvable->matrix));
    } catch (const std::exception &e) {
      Terminate(new std::exception(e));
    }
  }

  virtual ~DecompSolver() override = default;

 private:
  std::unique_ptr<DecompSolution<Numeric>> solution_;
  int16_t progress_ = 0;
  bool finished_ = false;
};

std::unique_ptr<Solver> MakeDecompSolver(Progress *progress, void (*finisher)(Solution *),
                                         void (*terminator)(std::exception *), const Solvable *ptr) {
  if (dynamic_cast<const DecompSolvable<double> *>(ptr) != nullptr) {
    return std::make_unique<DecompSolver<double>>(progress, finisher, terminator);
  }
  throw InvalidSolutionType();
}