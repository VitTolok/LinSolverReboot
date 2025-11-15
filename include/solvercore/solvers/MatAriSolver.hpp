#pragma once
#include "matrix.hpp"
#include "Solver.hpp"
#include <sstream>

struct MatAriSolvable final : public Solvable {
  std::string first;
  std::string second;
  NumericType first_type, second_type;
  OperatorType operator_type;
  bool first_plain, second_plain;
  MatAriSolvable(std::string &&first, std::string &&second, NumericType first_type, NumericType second_type,
                 OperatorType op_type, bool first_plain, bool second_plain)
      : first(std::move(first))
      , second(std::move(second))
      , first_type(first_type)
      , second_type(second_type)
      , operator_type(op_type)
      , first_plain(first_plain)
      , second_plain(second_plain) {
  }
  ~MatAriSolvable() final = default;
};

struct MatAriSolutionBase : public Solution {
  ~MatAriSolutionBase() override {};
  virtual std::string Print() const = 0;
  virtual std::string TypeName() const = 0;
};

template <class Numeric>
struct MatAriSolution final : public MatAriSolutionBase {
  MatAriSolution(Matrix<Numeric> &&result) : result(std::move(result)) {
  }
  Matrix<Numeric> result;
  std::string Print() const override {
    return (std::ostringstream() << result).str();
  }
  std::string TypeName() const override {
    return typeid(Numeric).name();
  }
  std::unique_ptr<Solution> Detach() final {
    return std::make_unique<MatAriSolution<Numeric>>(std::move(*this));
  }
  ~MatAriSolution() final = default;
};

class MatAriSolver final : public Solver {
 public:
  MatAriSolver(Progress &progress, const std::function<void(Solution &&)> &finisher,
               const std::function<void(MachineError &&)> &terminator)
      : Solver(progress, finisher, terminator) {
  }
  void operator()(const Solvable &) const override;
  ~MatAriSolver() final = default;
};