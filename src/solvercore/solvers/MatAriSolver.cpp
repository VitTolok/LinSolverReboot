#include "MatAriSolver.hpp"
#include "Rational.hpp"
#include "definitions.hpp"
#include <sstream>

template <class First, class Second, bool Plain = false>
inline std::unique_ptr<ISolution> HandleSum(First &first, Second &second) {
  if constexpr (!Addable<First, Second>) {
    throw InvalidMatrixOperation{};
  } else {
    using ResultType = decltype(std::declval<First>() + std::declval<Second>());
    if constexpr (Plain) {
      return std::make_unique<MatAriSolution<ResultType>>(
          Matrix<ResultType>::Eye(1, std::move(first) + std::move(second)));
    } else {
      return std::make_unique<MatAriSolution<typename ResultType::numeric_t>>(std::move(first) + std::move(second));
    }
  }
}
template <class First, class Second, bool Plain = false>
inline std::unique_ptr<ISolution> HandleSub(First &first, Second &second) {
  if constexpr (!Subtractable<First, Second>) {
    throw InvalidMatrixOperation{};
  } else {
    using ResultType = decltype(std::declval<First>() - std::declval<Second>());
    if constexpr (Plain) {
      return std::make_unique<MatAriSolution<ResultType>>(
          Matrix<ResultType>::Eye(1, std::move(first) - std::move(second)));
    } else {
      return std::make_unique<MatAriSolution<typename ResultType::numeric_t>>(std::move(first) - std::move(second));
    }
  }
}
template <class First, class Second, bool Plain = false>
inline std::unique_ptr<ISolution> HandleMult(First &first, Second &second) {
  if constexpr (!Multipliable<First, Second>) {
    throw InvalidMatrixOperation{};
  } else {
    using ResultType = decltype(std::declval<First>() * std::declval<Second>());
    if constexpr (Plain) {
      return std::make_unique<MatAriSolution<ResultType>>(
          Matrix<ResultType>::Eye(1, std::move(first) * std::move(second)));
    } else {
      return std::make_unique<MatAriSolution<typename ResultType::numeric_t>>(std::move(first) * std::move(second));
    }
  }
}
template <class First, class Second, bool Plain = false>
inline std::unique_ptr<ISolution> HandleDiv(First &first, Second &second) {
  if constexpr (!Divisible<First, Second>) {
    throw InvalidMatrixOperation{};
  } else {
    using ResultType = decltype(std::declval<First>() / std::declval<Second>());
    if constexpr (Plain) {
      return std::make_unique<MatAriSolution<ResultType>>(
          Matrix<ResultType>::Eye(1, std::move(first) / std::move(second)));
    } else {
      return std::make_unique<MatAriSolution<typename ResultType::numeric_t>>(std::move(first) / std::move(second));
    }
  }
}
template <class First, class Second, bool Plain = false>
inline std::unique_ptr<ISolution> HandleMod(First &first, Second &second) {
  if constexpr (!Moddable<First, Second>) {
    throw InvalidMatrixOperation{};
  } else {
    using ResultType = decltype(std::declval<First>() % std::declval<Second>());
    if constexpr (Plain) {
      return std::make_unique<MatAriSolution<ResultType>>(
          Matrix<ResultType>::Eye(1, std::move(first) % std::move(second)));
    } else {
      return std::make_unique<MatAriSolution<typename ResultType::numeric_t>>(std::move(first) % std::move(second));
    }
  }
}

template <class First, class Second, bool Plain = false>
inline std::unique_ptr<ISolution> HandleOperator(const MatAriSolvable *ptr) {
  First first;
  Second second;
  std::istringstream(ptr->first) >> first;
  std::istringstream(ptr->second) >> second;
  switch (ptr->operator_type) {
    case OperatorType::kAdd:
      return HandleSum<First, Second, Plain>(first, second);
      break;
    case OperatorType::kSub:
      return HandleSub<First, Second, Plain>(first, second);
      break;
    case OperatorType::kMul:
      return HandleMult<First, Second, Plain>(first, second);
      break;
    case OperatorType::kDiv:
      return HandleDiv<First, Second, Plain>(first, second);
      break;
    case OperatorType::kMod:
      return HandleMod<First, Second, Plain>(first, second);
      break;
    default:
      throw UnknownMachineError{};
  }
}

template <class First, class Second>
inline std::unique_ptr<ISolution> HandlePlain(const MatAriSolvable *ptr) {
  if (ptr->first_plain && ptr->second_plain) {
    return HandleOperator<First, Second, true>(ptr);
  } else if (ptr->first_plain) {
    return HandleOperator<First, Matrix<Second>>(ptr);
  } else if (ptr->second_plain) {
    return HandleOperator<Matrix<First>, Second>(ptr);
  } else {
    return HandleOperator<Matrix<First>, Matrix<Second>>(ptr);
  }
}

template <class First>
inline std::unique_ptr<ISolution> HandleSecond(const MatAriSolvable *ptr) {
  switch (ptr->second_type) {
    case NumericType::kLongDouble:
      return HandlePlain<First, long double>(ptr);
      break;
    case NumericType::kRational:
      return HandlePlain<First, Rational>(ptr);
      break;
    default:
      throw UnknownMachineError{};
  }
}

void MatAriSolver::operator()(const Solvable &solvable) const {
  auto ptr = dynamic_cast<const MatAriSolvable *>(&solvable);
  if (ptr == nullptr) {
    throw InvalidSolutionType{};
  }
  std::unique_ptr<ISolution> solution;
  switch (ptr->first_type) {
    case NumericType::kLongDouble:
      solution = HandleSecond<long double>(ptr);
      break;
    case NumericType::kRational:
      solution = HandleSecond<Rational>(ptr);
      break;
    default:
      throw UnknownMachineError{};
  }
  Finish(std::move(*solution));
}
