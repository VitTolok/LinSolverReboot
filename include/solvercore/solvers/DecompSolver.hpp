#pragma once
#include "Solver.hpp"
#include "decomp.hpp"
#include "rational.hpp"
#include <sstream>

struct DecompSolvableBase : public Solvable {
  enum class NumericType { kDouble, kRational } numeric_type;
  explicit DecompSolvableBase(NumericType type) : numeric_type(type) {
  }
  virtual std::string Print() const = 0;
};

template <class Numeric>
struct DecompSolvable : public DecompSolvableBase {
  Matrix<Numeric> matrix;
  DecompSolvable(std::string &&string, NumericType type) : DecompSolvableBase(type) {
    std::istringstream(std::move(string)) >> matrix;
  }
  virtual std::string Print() const override {
    return (std::ostringstream() << matrix).str();
  }
  virtual ~DecompSolvable() override = default;
};

template <class Numeric>
struct DecompSolution : public Solution {
  DecompSolution(const Matrix<Numeric> &matrix, Progress *progress) : decomp(matrix, progress) {
  }
  Decompose<Numeric> decomp;
  virtual std::unique_ptr<Solution> Detatch() override {
    return std::make_unique<DecompSolution>(std::move(*this));
  }
  virtual ~DecompSolution() override = default;
};

template <class Numeric>
struct DecompSolver : public Solver {
 public:
  DecompSolver(Progress &progress, const std::function<void(Solution &&)> &finisher,
               const std::function<void(MachineError &&)> &terminator)
      : Solver(progress, finisher, terminator) {
  }

  virtual void operator()(const Solvable &solvable) const override {
    auto decomp_solvable = dynamic_cast<const DecompSolvable<Numeric> *>(&solvable);
    if (decomp_solvable == nullptr) {
      throw InvalidSolutionType{};
    }
    Finish(DecompSolution<Numeric>(decomp_solvable->matrix, &progress_));
  }

  virtual ~DecompSolver() override = default;

 private:
  std::unique_ptr<DecompSolution<Numeric>> solution_;
};

inline std::unique_ptr<Solver> MakeDecompSolver(Progress &progress, const std::function<void(Solution &&)> &finisher,
                                                const std::function<void(MachineError &&)> &terminator,
                                                const Solvable &solvable) {
  auto ptr = dynamic_cast<const DecompSolvableBase *>(&solvable);
  if (ptr == nullptr) {
    throw InvalidSolutionType{};
  }
  switch (ptr->numeric_type) {
    case DecompSolvableBase::NumericType::kDouble:
      return std::make_unique<DecompSolver<long double>>(progress, finisher, terminator);
    case DecompSolvableBase::NumericType::kRational:
      return std::make_unique<DecompSolver<Rational>>(progress, finisher, terminator);
    default:
      throw InvalidSolutionType{};
  }
}