#include "MatAriSolver.hpp"
#include "rational.hpp"
#include <sstream>

template <class First, class Second, bool Plain = false>
inline std::unique_ptr<Solution> HandleSum(const MatAriSolvable *ptr, First &first, Second &second) {
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
inline std::unique_ptr<Solution> HandleSub(const MatAriSolvable *ptr, First &first, Second &second) {
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
inline std::unique_ptr<Solution> HandleMult(const MatAriSolvable *ptr, First &first, Second &second) {
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
inline std::unique_ptr<Solution> HandleDiv(const MatAriSolvable *ptr, First &first, Second &second) {
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
inline std::unique_ptr<Solution> HandleMod(const MatAriSolvable *ptr, First &first, Second &second) {
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
inline std::unique_ptr<Solution> HandleOperator(const MatAriSolvable *ptr) {
  First first;
  Second second;
  std::istringstream(ptr->first) >> first;
  std::istringstream(ptr->second) >> second;
  switch (ptr->operator_type) {
    case OperatorType::kAdd:
      return HandleSum<First, Second, Plain>(ptr, first, second);
      break;
    case OperatorType::kSub:
      return HandleSub<First, Second, Plain>(ptr, first, second);
      break;
    case OperatorType::kMul:
      return HandleMult<First, Second, Plain>(ptr, first, second);
      break;
    case OperatorType::kDiv:
      return HandleDiv<First, Second, Plain>(ptr, first, second);
      break;
    case OperatorType::kMod:
      return HandleMod<First, Second, Plain>(ptr, first, second);
      break;
    default:
      throw MachineError{};
  }
}

template <class First, class Second>
inline std::unique_ptr<Solution> HandlePlain(const MatAriSolvable *ptr) {
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
inline std::unique_ptr<Solution> HandleSecond(const MatAriSolvable *ptr) {
  switch (ptr->second_type) {
    case NumericType::kLongDouble:
      return HandlePlain<First, long double>(ptr);
      break;
    case NumericType::kRational:
      return HandlePlain<First, Rational>(ptr);
      break;
    default:
      throw MachineError{};
  }
}

void MatAriSolver::operator()(const Solvable &solvable) const {
  auto ptr = dynamic_cast<const MatAriSolvable *>(&solvable);
  if (ptr == nullptr) {
    throw InvalidSolutionType{};
  }
  std::unique_ptr<Solution> solution;
  switch (ptr->first_type) {
    case NumericType::kLongDouble:
      solution = HandleSecond<long double>(ptr);
      break;
    case NumericType::kRational:
      solution = HandleSecond<Rational>(ptr);
      break;
    default:
      throw MachineError{};
  }
  Finish(std::move(*solution));
}
