#pragma once
#include <iostream>
#include <sstream>
#include <vector>

#include "definitions.hpp"

class MatrixOutOfRange final : public MachineError {
 public:
  MatrixOutOfRange(size_t range, const std::string &parameter, size_t reference) {
    message_ = (std::ostringstream() << "Matrix has " << range << ' ' << parameter << ", but element " << reference
                                     << " was referenced")
                   .str();
    message_ptr = message_.c_str();
  }

  std::unique_ptr<MachineError> Detach() final {
    return std::make_unique<MatrixOutOfRange>(std::move(*this));
  }

  ~MatrixOutOfRange() final = default;

 private:
  std::string message_;
};

class InvalidMatrixBase final : public MachineError {
 public:
  InvalidMatrixBase() {
    message_ptr = message_;
  }
  ~InvalidMatrixBase() final = default;

 private:
  inline static constexpr const char message_[] = "std::vector used for Matrix construction is invalid";
};

class InvalidMatrixOperation final : public MachineError {
 public:
  InvalidMatrixOperation() {
    message_ptr = message_;
  }
  ~InvalidMatrixOperation() final = default;

 private:
  inline static constexpr const char message_[] = "The requested operation cannot be performed on provided matrixes";
};

class InvalidMatrixString final : public MachineError {
 public:
  InvalidMatrixString() {
    message_ptr = message_;
  }
  ~InvalidMatrixString() final = default;

 private:
  inline static constexpr const char message_[] = "String used for Matrix construction is invalid";
};

template <class Numeric>
class Matrix;

template <class T>
concept OnlyMatrix = std::same_as<Matrix<typename std::decay_t<T>::numeric_t>, std::decay_t<T>>;

template <class Numeric>
class Matrix {
 public:
  Matrix() : contents_() {
  }

  using numeric_t = Numeric;

  template <class SomeNumeric>
  inline Matrix(size_t rows, size_t cols, SomeNumeric &&value);
  inline Matrix(size_t rows, size_t cols);

  template <class SomeNumeric>
  explicit inline Matrix(const std::vector<std::vector<SomeNumeric>> &cont);
  template <class SomeNumeric>
  explicit inline Matrix(std::vector<std::vector<SomeNumeric>> &&cont);
  explicit inline Matrix(std::vector<std::vector<Numeric>> &&cont);
  explicit inline Matrix(const std::vector<std::vector<Numeric>> &cont);

  Matrix(Matrix<Numeric> &&) noexcept = default;
  Matrix(const Matrix<Numeric> &) = default;

  template <OnlyMatrix OtherMatrix>
  explicit inline Matrix(OtherMatrix &&other);

  static inline Matrix<Numeric> Eye(size_t size);
  template <class SomeNumeric>
  static inline Matrix<Numeric> Eye(size_t size, SomeNumeric &&value);
  template <class SomeNumericA, class SomeNumericB>
  static inline Matrix<Numeric> Eye(size_t size, SomeNumericA &&value, SomeNumericB &&zero_override);

  Matrix<Numeric> &operator=(const Matrix<Numeric> &) = default;
  Matrix<Numeric> &operator=(Matrix<Numeric> &&) noexcept = default;

  inline void SwapRows(size_t row1, size_t row2) noexcept;
  inline void SwapCols(size_t col1, size_t col2) noexcept;

  template <class SomeNumeric>
  inline void AddRow(size_t source, size_t target,
                     SomeNumeric &&mult) noexcept(noexcept(std::declval<Numeric &>() += std::declval<Numeric>()) &&
                                                  noexcept(std::declval<Numeric>() *
                                                           std::declval<std::decay_t<SomeNumeric>>()));
  template <class SomeNumeric>
  inline void AddCol(size_t source, size_t target,
                     SomeNumeric &&mult) noexcept(noexcept(std::declval<Numeric &>() += std::declval<Numeric>()) &&
                                                  noexcept(std::declval<Numeric>() *
                                                           std::declval<std::decay_t<SomeNumeric>>()));
  inline void AddRow(size_t source,
                     size_t target) noexcept(noexcept(std::declval<Numeric &>() += std::declval<Numeric>()));
  inline void AddCol(size_t source,
                     size_t target) noexcept(noexcept(std::declval<Numeric &>() += std::declval<Numeric>()));

  template <class SomeNumeric>
  inline void MultRow(size_t row, SomeNumeric &&mult) noexcept(noexcept(std::declval<Numeric &>() *=
                                                                        std::declval<std::decay_t<SomeNumeric>>()));
  template <class SomeNumeric>
  inline void MultCol(size_t col, SomeNumeric &&mult) noexcept(noexcept(std::declval<Numeric &>() *=
                                                                        std::declval<std::decay_t<SomeNumeric>>()));

  inline Numeric &operator()(size_t row, size_t col) noexcept;
  inline const Numeric &operator()(size_t row, size_t col) const noexcept;

  inline Numeric &At(size_t row, size_t col);
  inline const Numeric &At(size_t row, size_t col) const;

  inline size_t Rows() const noexcept;
  inline size_t Cols() const noexcept;

  bool Empty() const noexcept {
    return contents_.empty();
  }

  template <class NumericA, OnlyMatrix OtherMatrix>
  friend inline Matrix<NumericA> &operator+=(Matrix<NumericA> &, OtherMatrix &&);
  template <class NumericA, OnlyMatrix OtherMatrix>
  friend inline Matrix<NumericA> &operator-=(Matrix<NumericA> &, OtherMatrix &&);
  template <class NumericA, OnlyMatrix OtherMatrix>
  friend inline Matrix<NumericA> &operator*=(Matrix<NumericA> &, OtherMatrix &&);

  template <class NumericA, class NumericB>
  friend inline Matrix<NumericA> &operator+=(Matrix<NumericA> &,
                                             NumericB &&) noexcept(noexcept(std::declval<NumericA &>() +=
                                                                            std::declval<std::decay_t<NumericB>>()));
  template <class NumericA, class NumericB>
  friend inline Matrix<NumericA> &operator-=(Matrix<NumericA> &,
                                             NumericB &&) noexcept(noexcept(std::declval<NumericA &>() -=
                                                                            std::declval<std::decay_t<NumericB>>()));
  template <class NumericA, class NumericB>
  friend inline Matrix<NumericA> &operator*=(Matrix<NumericA> &,
                                             NumericB &&) noexcept(noexcept(std::declval<NumericA &>() *=
                                                                            std::declval<std::decay_t<NumericB>>()));
  template <class NumericA, class NumericB>
  friend inline Matrix<NumericA> &operator/=(Matrix<NumericA> &,
                                             NumericB &&) noexcept(noexcept(std::declval<NumericA &>() /=
                                                                            std::declval<std::decay_t<NumericB>>()));

  template <OnlyMatrix LMatrix, OnlyMatrix RMatrix>
  friend inline auto operator+(LMatrix &&, RMatrix &&);
  template <OnlyMatrix LMatrix, OnlyMatrix RMatrix>
  friend inline auto operator-(LMatrix &&, RMatrix &&);
  template <OnlyMatrix LMatrix, OnlyMatrix RMatrix>
  friend inline auto operator*(LMatrix &&, RMatrix &&);

  template <class LNumeric, class RNumeric>
  friend inline bool operator==(const Matrix<LNumeric> &,
                                const Matrix<RNumeric> &) noexcept(noexcept(std::declval<const LNumeric &>() !=
                                                                            std::declval<const RNumeric &>()));
  template <class LNumeric, class RNumeric>
  friend inline bool operator!=(const Matrix<LNumeric> &lhs,
                                const Matrix<RNumeric> &rhs) noexcept(noexcept(lhs == rhs));

 private:
  std::vector<std::vector<Numeric>> contents_;

  void ContentsValidityCheck() {
    if (contents_.empty()) {
      return;
    }
    for (const auto &row : contents_) {
      if (row.size() != contents_.front().size()) {
        contents_.clear();
        throw InvalidMatrixBase();
      }
    }
    if (contents_.front().empty()) {
      contents_.clear();
    }
  }

  template <class OtherNumeric>
  void RightCompoundSub(Matrix<OtherNumeric> &other) const {
    if (other.Rows() != Rows() || other.Cols() != Cols()) {
      throw InvalidMatrixOperation{};
    }
    for (size_t r = 0; r < Rows(); ++r) {
      for (size_t c = 0; c < Cols(); ++c) {
        other(r, c) = ((*this)(r, c)) - std::move(other(r, c));
      }
    }
  }
  template <class OtherNumeric>
  void RightCompoundMult(Matrix<OtherNumeric> &other) const {
    if (other.Rows() != Cols()) {
      throw InvalidMatrixOperation{};
    }
    std::vector<OtherNumeric> buffer;
    for (size_t c = 0; c < other.Cols(); ++c) {
      buffer.resize(Rows(), Numeric(0));
      for (size_t r = 0; r < Rows(); ++r) {
        buffer[r] = Numeric(0);
        for (size_t k = 0; k < Cols(); ++k) {
          buffer[r] += (((*this)(r, k)) * other(k, c));
        }
      }
      for (size_t r = 0; r < Rows(); ++r) {
        other(r, c) = std::move(buffer[r]);
      }
    }
  }
};

template <class Numeric>
inline Matrix<Numeric>::Matrix(size_t rows, size_t cols) : contents_(rows, std::vector<Numeric>(cols, Numeric(0))) {
}

template <class Numeric>
template <class SomeNumeric>
inline Matrix<Numeric>::Matrix(size_t rows, size_t cols, SomeNumeric &&value)
    : contents_(rows, std::vector<Numeric>(cols, Numeric(std::forward<SomeNumeric>(value)))) {
}

template <class Numeric>
inline Matrix<Numeric>::Matrix(std::vector<std::vector<Numeric>> &&cont) : contents_(std::move(cont)) {
  ContentsValidityCheck();
}

template <class Numeric>
inline Matrix<Numeric>::Matrix(const std::vector<std::vector<Numeric>> &cont) : contents_(cont) {
}

template <class Numeric>
template <class SomeNumeric>
inline Matrix<Numeric>::Matrix(const std::vector<std::vector<SomeNumeric>> &cont)
    : Matrix(cont.size(), cont.empty() ? 0 : cont.front().size()) {
  for (size_t r = 0; r < Rows(); ++r) {
    for (size_t c = 0; c < Cols(); ++c) {
      contents_[r][c] = cont[r][c];
    }
  }
}

template <class Numeric>
template <class SomeNumeric>
inline Matrix<Numeric>::Matrix(std::vector<std::vector<SomeNumeric>> &&cont) {
  Matrix<SomeNumeric> placeholder(std::move(cont));
  *this = std::move(placeholder);
}

template <class Numeric>
template <OnlyMatrix OtherMatrix>
inline Matrix<Numeric>::Matrix(OtherMatrix &&other) : Matrix(other.Rows(), other.Cols()) {
  for (size_t r = 0; r < Rows(); ++r) {
    for (size_t c = 0; c < Cols(); ++c) {
      contents_[r][c] = std::forward<OtherMatrix>(other)(r, c);
    }
  }
}

template <class Numeric>
inline Matrix<Numeric> Matrix<Numeric>::Eye(size_t size) {
  return Eye(size, Numeric(1), Numeric(0));
}

template <class Numeric>
template <class SomeNumeric>
inline Matrix<Numeric> Matrix<Numeric>::Eye(size_t size, SomeNumeric &&value) {
  return Eye(size, value, Numeric(0));
}

template <class Numeric>
template <class SomeNumericA, class SomeNumericB>
inline Matrix<Numeric> Matrix<Numeric>::Eye(size_t size, SomeNumericA &&value, SomeNumericB &&zero_override) {
  Matrix<Numeric> eye(size, size, std::forward<SomeNumericB>(zero_override));
  for (size_t i = 0; i < size; ++i) {
    if constexpr (std::is_rvalue_reference_v<decltype(value)>) {
      if (i + 1 == size) {
        eye(i, i) = std::move(value);
        break;
      }
    }
    eye(i, i) = value;
  }
  return std::move(eye);
}

template <class Numeric>
inline void Matrix<Numeric>::SwapRows(size_t row1, size_t row2) noexcept {
  contents_[row1].swap(contents_[row2]);
}
template <class Numeric>
inline void Matrix<Numeric>::SwapCols(size_t col1, size_t col2) noexcept {
  for (auto &row : contents_) {
    std::swap(row[col1], row[col2]);
  }
}

template <class Numeric>
template <class SomeNumeric>
inline void Matrix<Numeric>::AddRow(size_t source, size_t target, SomeNumeric &&mult) noexcept(
    noexcept(std::declval<Numeric &>() += std::declval<Numeric>()) &&
    noexcept(std::declval<Numeric>() * std::declval<std::decay_t<SomeNumeric>>())) {
  for (size_t c = 0; c + 1 < Cols(); ++c) {
    contents_[target][c] += contents_[source][c] * mult;
  }
  if (Cols() > 0) {
    if constexpr (std::is_rvalue_reference_v<decltype(mult)>) {
      contents_[target].back() += contents_[source].back() * std::move(mult);
    } else {
      contents_[target].back() += contents_[source].back() * mult;
    }
  }
}
template <class Numeric>
template <class SomeNumeric>
inline void Matrix<Numeric>::AddCol(size_t source, size_t target, SomeNumeric &&mult) noexcept(
    noexcept(std::declval<Numeric &>() += std::declval<Numeric>()) &&
    noexcept(std::declval<Numeric>() * std::declval<std::decay_t<SomeNumeric>>())) {
  for (size_t r = 0; r + 1 < Rows(); ++r) {
    contents_[r][target] += contents_[r][source] * mult;
  }
  if (Cols() > 0) {
    if constexpr (std::is_rvalue_reference_v<decltype(mult)>) {
      contents_.back()[target] += contents_.back()[source] * std::move(mult);
    } else {
      contents_.back()[target] += contents_.back()[source] * mult;
    }
  }
}
template <class Numeric>
inline void Matrix<Numeric>::AddRow(size_t source, size_t target) noexcept(noexcept(std::declval<Numeric &>() +=
                                                                                    std::declval<Numeric>())) {
  for (size_t c = 0; c < Cols(); ++c) {
    contents_[target][c] += contents_[source][c];
  }
}
template <class Numeric>
inline void Matrix<Numeric>::AddCol(size_t source, size_t target) noexcept(noexcept(std::declval<Numeric &>() +=
                                                                                    std::declval<Numeric>())) {
  for (size_t r = 0; r < Rows(); ++r) {
    contents_[r][target] += contents_[r][source];
  }
}

template <class Numeric>
template <class SomeNumeric>
inline void Matrix<Numeric>::MultRow(size_t row,
                                     SomeNumeric &&mult) noexcept(noexcept(std::declval<Numeric &>() *=
                                                                           std::declval<std::decay_t<SomeNumeric>>())) {
  if constexpr (std::is_rvalue_reference_v<decltype(mult)>) {
    for (size_t c = 0; c + 1 < Cols(); ++c) {
      contents_[row][c] *= mult;
    }
    contents_[row].back() *= std::move(mult);
  } else {
    for (size_t c = 0; c < Cols(); ++c) {
      contents_[row][c] *= mult;
    }
  }
}
template <class Numeric>
template <class SomeNumeric>
inline void Matrix<Numeric>::MultCol(size_t col,
                                     SomeNumeric &&mult) noexcept(noexcept(std::declval<Numeric &>() *=
                                                                           std::declval<std::decay_t<SomeNumeric>>())) {
  if constexpr (std::is_rvalue_reference_v<decltype(mult)>) {
    for (size_t r = 0; r + 1 < Rows(); ++r) {
      contents_[r][col] *= mult;
    }
    if (Rows() > 0) {
      contents_.back()[col] *= std::move(mult);
    }
  } else {
    for (size_t r = 0; r < Rows(); ++r) {
      contents_[r][col] *= mult;
    }
  }
}

template <class Numeric>
inline Numeric &Matrix<Numeric>::operator()(size_t row, size_t col) noexcept {
  return contents_[row][col];
}
template <class Numeric>
inline const Numeric &Matrix<Numeric>::operator()(size_t row, size_t col) const noexcept {
  return contents_[row][col];
}

template <class Numeric>
inline Numeric &Matrix<Numeric>::At(size_t row, size_t col) {
  if (row >= Rows()) {
    throw MatrixOutOfRange(Rows(), "rows", row);
  }
  if (col >= Cols()) {
    throw MatrixOutOfRange(Cols(), "columns", col);
  }
  return contents_[row][col];
}
template <class Numeric>
inline const Numeric &Matrix<Numeric>::At(size_t row, size_t col) const {
  if (row >= Rows()) {
    throw MatrixOutOfRange(Rows(), "rows", row);
  }
  if (col >= Cols()) {
    throw MatrixOutOfRange(Cols(), "columns", col);
  }
  return contents_[row][col];
}

template <class Numeric>
inline size_t Matrix<Numeric>::Rows() const noexcept {
  return contents_.size();
}
template <class Numeric>
inline size_t Matrix<Numeric>::Cols() const noexcept {
  return contents_.empty() ? 0 : contents_.front().size();
}

template <class Numeric>
std::ostream &operator<<(std::ostream &out, const Matrix<Numeric> &matrix) {
  if (matrix.Empty()) {
    return out;
  }
  std::vector<size_t> lengths(matrix.Cols(), 0);
  for (size_t col = 0; col < matrix.Cols(); col++) {
    for (size_t row = 0; row < matrix.Rows(); row++) {
      lengths[col] = std::max(lengths[col], (std::ostringstream() << matrix(row, col)).str().size());
    }
  }
  for (size_t row = 0; row < matrix.Rows(); ++row) {
    if (row == 0) {
      out << "/ ";
    } else if (row + 1 == matrix.Rows()) {
      out << "\\ ";
    } else {
      out << "| ";
    }
    for (size_t col = 0; col < matrix.Cols(); ++col) {
      out << matrix(row, col)
          << std::string(lengths[col] - (std::ostringstream() << matrix(row, col)).str().size() + 1, ' ');
    }
    if (row + 1 == matrix.Rows()) {
      out << "/\n";
    } else if (row == 0) {
      out << "\\\n";
    } else {
      out << "|\n";
    }
  }
  return out;
}

template <class Numeric>
std::istream &operator>>(std::istream &in, Matrix<Numeric> &matrix) {
  std::vector<std::vector<Numeric>> contents;
  std::string input;
  Numeric value;
  size_t cols = 0;
  size_t rows = 0;
  while (in >> input) {
    contents.emplace_back();
    if (rows == 0) {
      if (input != "/") {
        throw InvalidMatrixString();
      }
      in >> input;
      while (input != "/" && input != "\\") {
        try {
          std::istringstream(input) >> value;
        } catch (...) {
          throw InvalidMatrixString();
        }
        contents[0].push_back(value);
        cols++;
        in >> input;
      }
      if (input == "/") {
        matrix = Matrix<Numeric>(std::move(contents));
        return in;
      }
      if (input != "\\") {
        throw InvalidMatrixString();
      }
      rows++;
      continue;
    }
    contents.back().resize(cols);
    if (input == "\\") {
      for (size_t i = 0; i < cols; i++) {
        in >> input;
        try {
          std::istringstream(input) >> value;
        } catch (...) {
          throw InvalidMatrixString();
        }
        contents.back()[i] = value;
      }
      in >> input;
      if (input == "/") {
        matrix = Matrix<Numeric>(std::move(contents));
        return in;
      } else {
        throw InvalidMatrixString();
      }
    }
    if (input == "|") {
      for (size_t i = 0; i < cols; i++) {
        in >> input;
        try {
          std::istringstream(input) >> value;
        } catch (...) {
          throw InvalidMatrixString();
        }
        contents.back()[i] = value;
      }
      in >> input;
      if (input != "|") {
        throw InvalidMatrixString();
      }
      continue;
    }
    throw InvalidMatrixString();
  }
  if (rows == 0) {
    matrix = Matrix<Numeric>();
  } else {
    throw InvalidMatrixString();
  }
  return in;
}

template <class Numeric, OnlyMatrix OtherMatrix>
inline Matrix<Numeric> &operator+=(Matrix<Numeric> &self, OtherMatrix &&other) {
  if (self.Rows() != other.Rows() || self.Cols() != other.Cols()) {
    throw InvalidMatrixOperation{};
  }
  for (size_t i = 0; i < self.Rows(); ++i) {
    for (size_t j = 0; j < self.Cols(); ++j) {
      self(i, j) += std::forward<OtherMatrix>(other)(i, j);
    }
  }
  return self;
}
template <class Numeric, OnlyMatrix OtherMatrix>
inline Matrix<Numeric> &operator-=(Matrix<Numeric> &self, OtherMatrix &&other) {
  if (self.Rows() != other.Rows() || self.Cols() != other.Cols()) {
    throw InvalidMatrixOperation{};
  }
  for (size_t i = 0; i < self.Rows(); ++i) {
    for (size_t j = 0; j < self.Cols(); ++j) {
      self(i, j) -= std::forward<OtherMatrix>(other)(i, j);
    }
  }
  return self;
}
template <class Numeric, OnlyMatrix OtherMatrix>
inline Matrix<Numeric> &operator*=(Matrix<Numeric> &self, OtherMatrix &&other) {
  if (self.Cols() != other.Rows()) {
    throw InvalidMatrixOperation{};
  }
  if constexpr (std::is_same_v<typename std::decay_t<OtherMatrix>::numeric_t, Numeric>) {
    if (&self == &other) {
      return self = self * other;
    }
  }
  std::vector<Numeric> buffer(other.Cols(), Numeric(0));
  for (size_t r = 0; r < self.Rows(); ++r) {
    buffer.resize(other.Cols(), Numeric(0));
    for (size_t c = 0; c < other.Cols(); ++c) {
      buffer[c] = Numeric(0);
      for (size_t k = 0; k < self.Cols(); ++k) {
        buffer[c] += self(r, k) * std::forward<OtherMatrix>(other)(k, c);
      }
    }
    buffer.swap(self.contents_[r]);
  }
  return self;
}

template <class NumericA, class NumericB>
inline Matrix<NumericA> &operator+=(Matrix<NumericA> &matrix,
                                    NumericB &&number) noexcept(noexcept(std::declval<NumericA &>() +=
                                                                         std::declval<std::decay_t<NumericB>>())) {
  if (matrix.Rows() == 1 && matrix.Cols() == 1) {
    matrix(0, 0) += std::forward<NumericB>(number);
    return matrix;
  }
  for (size_t r = 0; r < matrix.Rows(); ++r) {
    for (size_t c = 0; c < matrix.Cols(); ++c) {
      matrix(r, c) += number;
    }
  }
  return matrix;
}
template <class NumericA, class NumericB>
inline Matrix<NumericA> &operator-=(Matrix<NumericA> &matrix,
                                    NumericB &&number) noexcept(noexcept(std::declval<NumericA &>() -=
                                                                         std::declval<std::decay_t<NumericB>>())) {
  if (matrix.Rows() == 1 && matrix.Cols() == 1) {
    matrix(0, 0) -= std::forward<NumericB>(number);
    return matrix;
  }
  for (size_t r = 0; r < matrix.Rows(); ++r) {
    for (size_t c = 0; c < matrix.Cols(); ++c) {
      matrix(r, c) -= number;
    }
  }
  return matrix;
}
template <class NumericA, class NumericB>
inline Matrix<NumericA> &operator*=(Matrix<NumericA> &matrix,
                                    NumericB &&number) noexcept(noexcept(std::declval<NumericA &>() *=
                                                                         std::declval<std::decay_t<NumericB>>())) {
  if (matrix.Rows() == 1 && matrix.Cols() == 1) {
    matrix(0, 0) *= std::forward<NumericB>(number);
    return matrix;
  }
  for (size_t r = 0; r < matrix.Rows(); ++r) {
    for (size_t c = 0; c < matrix.Cols(); ++c) {
      matrix(r, c) *= number;
    }
  }
  return matrix;
}
template <class NumericA, class NumericB>
inline Matrix<NumericA> &operator/=(Matrix<NumericA> &matrix,
                                    NumericB &&number) noexcept(noexcept(std::declval<NumericA &>() /=
                                                                         std::declval<std::decay_t<NumericB>>())) {
  if (matrix.Rows() == 1 && matrix.Cols() == 1) {
    matrix(0, 0) /= std::forward<NumericB>(number);
    return matrix;
  }
  for (size_t r = 0; r < matrix.Rows(); ++r) {
    for (size_t c = 0; c < matrix.Cols(); ++c) {
      matrix(r, c) /= number;
    }
  }
  return matrix;
}

template <OnlyMatrix LMatrix, OnlyMatrix RMatrix>
  requires Addable<typename std::decay_t<LMatrix>::numeric_t, typename std::decay_t<RMatrix>::numeric_t>
inline auto operator+(LMatrix &&first, RMatrix &&second) {
  using LType = typename std::decay_t<LMatrix>::numeric_t;
  using RType = typename std::decay_t<RMatrix>::numeric_t;
  using EndType = decltype(std::declval<const LType &>() + std::declval<const RType &>());
  if constexpr (std::is_same_v<LType, EndType> && std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first += std::forward<RMatrix>(second));
  } else if constexpr (std::is_same_v<RType, EndType> && std::is_rvalue_reference_v<decltype(second)>) {
    return std::move(second += std::forward<LMatrix>(first));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(first));
    return std::move(copy += std::forward<RMatrix>(second));
  }
}
template <OnlyMatrix LMatrix, OnlyMatrix RMatrix>
  requires Subtractable<typename std::decay_t<LMatrix>::numeric_t, typename std::decay_t<RMatrix>::numeric_t>
inline auto operator-(LMatrix &&first, RMatrix &&second) {
  using LType = typename std::decay_t<LMatrix>::numeric_t;
  using RType = typename std::decay_t<RMatrix>::numeric_t;
  using EndType = decltype(LType{} - RType{});
  if constexpr (std::is_same_v<LType, EndType> && std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first -= std::forward<RMatrix>(second));
  } else if constexpr (std::is_same_v<RType, EndType> && std::is_rvalue_reference_v<decltype(second)>) {
    return std::move((first.RightCompoundSub(second), second));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(first));
    return std::move(copy -= std::forward<RMatrix>(second));
  }
}
template <OnlyMatrix LMatrix, OnlyMatrix RMatrix>
  requires Multipliable<typename std::decay_t<LMatrix>::numeric_t, typename std::decay_t<RMatrix>::numeric_t>
inline auto operator*(LMatrix &&first, RMatrix &&second) {
  using LType = typename std::decay_t<LMatrix>::numeric_t;
  using RType = typename std::decay_t<RMatrix>::numeric_t;
  using EndType = decltype(LType{} * RType{});
  if constexpr (std::is_same_v<LType, EndType> && std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first *= std::forward<RMatrix>(second));
  } else if constexpr (std::is_same_v<RType, EndType> && std::is_rvalue_reference_v<decltype(second)>) {
    return std::move((first.RightCompoundMult(second), second));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(first));
    return std::move(copy *= std::forward<RMatrix>(second));
  }
}

template <OnlyMatrix LMatrix, class RNumeric>
  requires Addable<typename std::decay_t<LMatrix>::numeric_t, std::decay_t<RNumeric>>
inline auto operator+(LMatrix &&matrix, RNumeric &&number) noexcept(
    std::is_same_v<typename std::decay_t<LMatrix>::numeric_t,
                   decltype((typename std::decay_t<LMatrix>::numeric_t){} * (typename std::decay_t<RNumeric>){})> &&
    std::is_rvalue_reference_v<decltype(matrix)> && noexcept(matrix += std::declval<decltype(number)>())) {
  using LType = typename std::decay_t<LMatrix>::numeric_t;
  using RType = typename std::decay_t<RNumeric>;
  using EndType = decltype(std::declval<const LType &>() + std::declval<const RType &>());
  if constexpr (std::is_same_v<LType, EndType> && std::is_rvalue_reference_v<decltype(matrix)>) {
    return std::move(matrix += std::forward<RNumeric>(number));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(matrix));
    return std::move(copy += std::forward<RNumeric>(number));
  }
}
template <OnlyMatrix LMatrix, class RNumeric>
  requires Subtractable<typename std::decay_t<LMatrix>::numeric_t, std::decay_t<RNumeric>>
inline auto operator-(LMatrix &&matrix, RNumeric &&number) noexcept(
    std::is_same_v<typename std::decay_t<LMatrix>::numeric_t,
                   decltype((typename std::decay_t<LMatrix>::numeric_t){} * (typename std::decay_t<RNumeric>){})> &&
    std::is_rvalue_reference_v<decltype(matrix)> && noexcept(matrix -= std::declval<decltype(number)>())) {
  using LType = typename std::decay_t<LMatrix>::numeric_t;
  using RType = typename std::decay_t<RNumeric>;
  using EndType = decltype(LType{} - RType{});
  if constexpr (std::is_same_v<LType, EndType> && std::is_rvalue_reference_v<decltype(matrix)>) {
    return std::move(matrix -= std::forward<RNumeric>(number));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(matrix));
    return std::move(copy -= std::forward<RNumeric>(number));
  }
}
template <OnlyMatrix LMatrix, class RNumeric>
  requires Multipliable<typename std::decay_t<LMatrix>::numeric_t, std::decay_t<RNumeric>>
inline auto operator*(LMatrix &&matrix, RNumeric &&number) noexcept(
    std::is_same_v<typename std::decay_t<LMatrix>::numeric_t,
                   decltype((typename std::decay_t<LMatrix>::numeric_t){} * (typename std::decay_t<RNumeric>){})> &&
    std::is_rvalue_reference_v<decltype(matrix)> && noexcept(matrix *= std::declval<decltype(number)>())) {
  using LType = typename std::decay_t<LMatrix>::numeric_t;
  using RType = typename std::decay_t<RNumeric>;
  using EndType = decltype(LType{} * RType{});
  if constexpr (std::is_same_v<LType, EndType> && std::is_rvalue_reference_v<decltype(matrix)>) {
    return std::move(matrix *= std::forward<RNumeric>(number));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(matrix));
    return std::move(copy *= std::forward<RNumeric>(number));
  }
}

template <class LNumeric, OnlyMatrix RMatrix>
  requires Addable<std::decay_t<LNumeric>, typename std::decay_t<RMatrix>::numeric_t>
inline auto operator+(LNumeric &&number, RMatrix &&matrix) noexcept(noexcept(std::declval<decltype(matrix)>() +
                                                                             std::declval<decltype(number)>())) {
  return std::forward<RMatrix>(matrix) + std::forward<LNumeric>(number);
}
template <class LNumeric, OnlyMatrix RMatrix>
  requires Subtractable<std::decay_t<LNumeric>, typename std::decay_t<RMatrix>::numeric_t>
inline auto operator-(LNumeric &&number, RMatrix &&matrix) noexcept(
    std::is_same_v<typename std::decay_t<RMatrix>::numeric_t,
                   decltype(std::declval<decltype(number)>() -
                            std::declval<typename std::decay_t<RMatrix>::numeric_t>())> &&
    std::is_rvalue_reference_v<decltype(matrix)> &&
    noexcept(std::declval<std::decay_t<LNumeric>>() - std::declval<typename std::decay_t<RMatrix>::numeric_t>())) {
  using RType = typename std::decay_t<RMatrix>::numeric_t;
  using EndType =
      decltype(std::declval<std::decay_t<LNumeric>>() - std::declval<typename std::decay_t<RMatrix>::numeric_t>());
  if constexpr (std::is_same_v<RType, EndType> && std::is_rvalue_reference_v<decltype(matrix)>) {
    if (matrix.Rows() == 1 && matrix.Cols() == 1) {
      matrix(0, 0) = std::forward<LNumeric>(number) - std::move(matrix(0, 0));
      return std::move(matrix);
    }
    for (size_t r = 0; r < matrix.Rows(); ++r) {
      for (size_t c = 0; c < matrix.Rows(); ++c) {
        matrix(r, c) = number - std::move(matrix(r, c));
      }
    }
    return std::move(matrix);
  }
  Matrix<EndType> buffer;
  if (matrix.Rows() == 1 && matrix.Cols() == 1) {
    buffer = Matrix<EndType>(1, 1);
    buffer(0, 0) = std::forward<LNumeric>(number) - std::forward<RMatrix>(matrix)(0, 0);
    return std::move(buffer);
  }
  buffer = Matrix<EndType>(matrix.Rows(), matrix.Cols());
  for (size_t r = 0; r < matrix.Rows(); ++r) {
    for (size_t c = 0; c < matrix.Rows(); ++c) {
      buffer(r, c) = number - std::forward<RMatrix>(matrix)(r, c);
    }
  }
  return std::move(buffer);
}
template <class LNumeric, OnlyMatrix RMatrix>
  requires Multipliable<std::decay_t<LNumeric>, typename std::decay_t<RMatrix>::numeric_t>
inline auto operator*(LNumeric &&number, RMatrix &&matrix) noexcept(noexcept(std::declval<decltype(matrix)>() *
                                                                             std::declval<decltype(number)>())) {
  return std::forward<RMatrix>(matrix) * std::forward<LNumeric>(number);
}

template <class LNumeric, class RNumeric>
inline bool operator==(const Matrix<LNumeric> &lhs,
                       const Matrix<RNumeric> &rhs) noexcept(noexcept(std::declval<const LNumeric &>() !=
                                                                      std::declval<const RNumeric &>())) {
  if (lhs.Rows() != rhs.Rows() || lhs.Cols() != rhs.Cols()) {
    return false;
  }
  for (size_t r = 0; r < lhs.Rows(); ++r) {
    for (size_t c = 0; c < lhs.Cols(); ++c) {
      if (lhs(r, c) != rhs(r, c)) {
        return false;
      }
    }
  }
  return true;
}
template <class LNumeric, class RNumeric>
inline bool operator!=(const Matrix<LNumeric> &lhs, const Matrix<RNumeric> &rhs) noexcept(noexcept(lhs == rhs)) {
  return !(lhs == rhs);
}