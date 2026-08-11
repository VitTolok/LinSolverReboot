#pragma once
#include "matrix.hpp"
#include "Solver.hpp"
#include <sstream>
#include <meta>

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

struct MatAriSolutionBase : public ISolution {
  ~MatAriSolutionBase() override {};
  virtual std::string Print() const = 0;
  virtual std::string_view TypeName() const = 0;
};

template <class Numeric>
struct MatAriSolution final : public MatAriSolutionBase {
  static constexpr auto kTypeName = std::define_static_string(std::meta::display_string_of(^^Numeric));
  MatAriSolution(Matrix<Numeric> &&result) : result(std::move(result)) {
  }
  Matrix<Numeric> result;
  std::string Print() const override {
    return (std::ostringstream() << result).str();
  }
  std::string_view TypeName() const override {
    return kTypeName;
  }
  std::unique_ptr<ISolution> Detach() final {
    return std::make_unique<MatAriSolution<Numeric>>(std::move(*this));
  }
  ~MatAriSolution() final = default;
};

class MatAriSolver final : public ISolver {
 public:
  MatAriSolver(IProgress &progress, const std::function<void(ISolution &&)> &finisher,
               const std::function<void(IMachineError &&)> &terminator)
      : ISolver(progress, finisher, terminator) {
  }
  void operator()(const Solvable &) const override;
  ~MatAriSolver() final = default;
};