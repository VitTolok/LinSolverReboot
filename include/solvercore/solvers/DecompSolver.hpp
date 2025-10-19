#pragma once
#include "Solver.hpp"
#include "decomp.hpp"
#include "rational.hpp"
#include <sstream>

struct DecompSolvable : public Solvable {
  std::string input;
  NumericType numeric_type;
  DecompSolvable(std::string &&string, NumericType type) {
    input = std::move(string);
    numeric_type = type;
  }
  virtual ~DecompSolvable() override = default;
};

template <class Numeric>
struct DecompSolution : public Solution {
  std::string det;
  std::string solved;
  std::string lower;
  std::string upper;
  DecompSolution(const std::string &input, Progress *progress) {
    Matrix<Numeric> matrix;
    std::istringstream(input) >> matrix;
    auto decomp = Decompose(matrix, progress);
    det = decomp.det.has_value() ? (std::ostringstream() << *decomp.det).str() : "DNE";
    solved = (std::ostringstream() << decomp.simple).str();
    lower = decomp.lower.has_value() ? (std::ostringstream() << *decomp.lower).str() : "DNE";
    upper = decomp.upper.has_value() ? (std::ostringstream() << *decomp.upper).str() : "DNE";
  }
  virtual std::unique_ptr<Solution> Detach() override {
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
    auto decomp_solvable = dynamic_cast<const DecompSolvable *>(&solvable);
    if (decomp_solvable == nullptr) {
      throw InvalidSolutionType{};
    }
    Finish(DecompSolution<Numeric>(decomp_solvable->input, &progress_));
  }

  virtual ~DecompSolver() override = default;

 private:
  std::unique_ptr<DecompSolution<Numeric>> solution_;
};

inline std::unique_ptr<Solver> MakeDecompSolver(Progress &progress, const std::function<void(Solution &&)> &finisher,
                                                const std::function<void(MachineError &&)> &terminator,
                                                const Solvable &solvable) {
  auto ptr = dynamic_cast<const DecompSolvable *>(&solvable);
  if (ptr == nullptr) {
    throw InvalidSolutionType{};
  }
  switch (ptr->numeric_type) {
    case NumericType::kLongDouble:
      return std::make_unique<DecompSolver<long double>>(progress, finisher, terminator);
    case NumericType::kRational:
      return std::make_unique<DecompSolver<Rational>>(progress, finisher, terminator);
    default:
      throw InvalidSolutionType{};
  }
}