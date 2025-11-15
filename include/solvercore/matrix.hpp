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

template <class T>
concept Matrixable = std::is_same_v<T, std::decay_t<T>>;

template <class Numeric>
  requires Matrixable<Numeric>
class Matrix;

template <class Numeric>
struct MatrixDepth {
  inline static constexpr const size_t value = 0;
};
template <class Numeric>
struct MatrixDepth<Matrix<Numeric>> {
  inline static constexpr const size_t value = MatrixDepth<Numeric>::value + 1;
};
template <class T>
inline static constexpr const size_t matrix_depth_v = MatrixDepth<std::decay_t<T>>::value;

template <class L, class R>
concept SameMatrixDepth = matrix_depth_v<L> == matrix_depth_v<R>;
template <class T>
concept OnlyMatrix = matrix_depth_v<T> > 0;

template <class L, class R>
concept BothMatrix = requires {
  requires OnlyMatrix<L> && OnlyMatrix<R>;
  requires matrix_depth_v<L> == matrix_depth_v<R>;
};
template <class L, class R>
concept LMatRNum = matrix_depth_v<L> > matrix_depth_v<R>;
template <class L, class R>
concept LNumRMat = matrix_depth_v<L> < matrix_depth_v<R>;

template <OnlyMatrix T>
using Numeric_t = typename std::decay_t<T>::numeric_t;

template <class L, class R>
concept ValidContent = SameMatrixDepth<L, R> && Convertible<L, R> && Movable<L>;

template <class T>
concept HasZero = requires { static_cast<std::decay_t<T>>(0); };
template <class T>
concept HasOne = requires { static_cast<std::decay_t<T>>(1); };

template <class L, class R>
concept RightCompoundSubtractable = requires(std::decay_t<L> l, std::decay_t<R> &r) { l.RightCompoundSub(r); };
template <class L, class R>
concept RightCompoundMultipliable = requires(std::decay_t<L> l, std::decay_t<R> &r) { l.RightCompoundMult(r); };

template <class Numeric>
  requires Matrixable<Numeric>
std::istream &operator>>(std::istream &, Matrix<Numeric> &);
template <class Numeric>
  requires Matrixable<Numeric>
std::ostream &operator<<(std::ostream &, const Matrix<Numeric> &);

template <class Numeric>
  requires Matrixable<Numeric>
class Matrix {
 public:
  Matrix() : contents_() {
  }

  using numeric_t = Numeric;

  template <class SomeNumeric>
    requires ValidContent<Numeric, SomeNumeric>
  inline Matrix(size_t rows, size_t cols, SomeNumeric &&value);
  inline Matrix(size_t rows, size_t cols)
    requires HasZero<Numeric>;

  template <class SomeNumeric>
    requires ValidContent<Numeric, SomeNumeric>
  explicit inline Matrix(const std::vector<std::vector<SomeNumeric>> &cont);
  template <class SomeNumeric>
    requires ValidContent<Numeric, SomeNumeric>
  explicit inline Matrix(std::vector<std::vector<SomeNumeric>> &&cont);
  explicit inline Matrix(std::vector<std::vector<Numeric>> &&cont);
  explicit inline Matrix(const std::vector<std::vector<Numeric>> &cont)
    requires Copyable<Numeric>;

  Matrix(Matrix<Numeric> &&) noexcept = default;
  Matrix(const Matrix<Numeric> &) = default;

  template <OnlyMatrix OtherMatrix>
    requires ValidContent<Numeric, Numeric_t<OtherMatrix>>
  explicit inline Matrix(OtherMatrix &&other);

  static inline Matrix<Numeric> Eye(size_t size)
    requires HasZero<Numeric> && HasOne<Numeric>;
  template <class SomeNumeric>
    requires ValidContent<Numeric, SomeNumeric> && HasZero<Numeric>
  static inline Matrix<Numeric> Eye(size_t size, SomeNumeric &&value);
  template <class SomeNumericA, class SomeNumericB>
    requires ValidContent<Numeric, SomeNumericA> && ValidContent<Numeric, SomeNumericB>
  static inline Matrix<Numeric> Eye(size_t size, SomeNumericA &&value, SomeNumericB &&zero_override);

  Matrix<Numeric> &operator=(const Matrix<Numeric> &) = default;
  Matrix<Numeric> &operator=(Matrix<Numeric> &&) noexcept = default;
  template <OnlyMatrix OtherMatrix>
  inline Matrix<Numeric> &operator=(OtherMatrix &&)
    requires ValidContent<Numeric, Numeric_t<OtherMatrix>> && (!std::is_same_v<Numeric, Numeric_t<OtherMatrix>>);

  inline void SwapRows(size_t row1, size_t row2) noexcept;
  inline void SwapCols(size_t col1, size_t col2) noexcept(NoexceptMovable<Numeric>);

  template <class SomeNumeric>
    requires Multipliable<Numeric, SomeNumeric> && CompoundAddable<Numeric, MulType<Numeric, SomeNumeric>>
  inline void AddRow(size_t source, size_t target,
                     SomeNumeric &&mult) noexcept(NoexceptCompoundAddable<Numeric, MulType<Numeric, SomeNumeric>> &&
                                                  NoexceptMultipliable<Numeric, SomeNumeric>);
  template <class SomeNumeric>
    requires Multipliable<Numeric, SomeNumeric> && CompoundAddable<Numeric, MulType<Numeric, SomeNumeric>>
  inline void AddCol(size_t source, size_t target,
                     SomeNumeric &&mult) noexcept(NoexceptCompoundAddable<Numeric, MulType<Numeric, SomeNumeric>> &&
                                                  NoexceptMultipliable<Numeric, SomeNumeric>);
  inline void AddRow(size_t source, size_t target) noexcept(NoexceptCompoundAddable<Numeric>)
    requires CompoundAddable<Numeric>;
  inline void AddCol(size_t source, size_t target) noexcept(NoexceptCompoundAddable<Numeric>)
    requires CompoundAddable<Numeric>;

  template <class SomeNumeric>
    requires CompoundMultipliable<Numeric, SomeNumeric>
  inline void MultRow(size_t row, SomeNumeric &&mult) noexcept(NoexceptCompoundMultipliable<Numeric, SomeNumeric>);
  template <class SomeNumeric>
    requires CompoundMultipliable<Numeric, SomeNumeric>
  inline void MultCol(size_t col, SomeNumeric &&mult) noexcept(NoexceptCompoundMultipliable<Numeric, SomeNumeric>);

  inline Numeric &operator()(size_t row, size_t col) noexcept;
  inline const Numeric &operator()(size_t row, size_t col) const noexcept;

  inline Numeric &At(size_t row, size_t col);
  inline const Numeric &At(size_t row, size_t col) const;

  inline size_t Rows() const noexcept;
  inline size_t Cols() const noexcept;

  bool Empty() const noexcept {
    return contents_.empty();
  }

  template <class NumericA, class OtherMatrix>
    requires Matrixable<NumericA> && BothMatrix<Matrix<NumericA>, OtherMatrix> &&
             CompoundAddable<NumericA, Numeric_t<OtherMatrix>>
  friend inline Matrix<NumericA> &operator+=(Matrix<NumericA> &, OtherMatrix &&);
  template <class NumericA, class OtherMatrix>
    requires Matrixable<NumericA> && BothMatrix<Matrix<NumericA>, OtherMatrix> &&
             CompoundSubtractable<NumericA, Numeric_t<OtherMatrix>>
  friend inline Matrix<NumericA> &operator-=(Matrix<NumericA> &, OtherMatrix &&);
  template <class NumericA, class OtherMatrix>
    requires Matrixable<NumericA> && BothMatrix<Matrix<NumericA>, OtherMatrix> &&
             Multipliable<NumericA, Numeric_t<OtherMatrix>> &&
             CompoundAddable<NumericA, MulType<NumericA, Numeric_t<OtherMatrix>>>
  friend inline Matrix<NumericA> &operator*=(Matrix<NumericA> &, OtherMatrix &&);

  template <class NumericA, class NumericB>
    requires LMatRNum<Matrix<NumericA>, NumericB> && CompoundAddable<NumericA, NumericB>
  friend inline Matrix<NumericA> &operator+=(Matrix<NumericA> &,
                                             NumericB &&) noexcept(NoexceptCompoundAddable<NumericA, NumericB>);
  template <class NumericA, class NumericB>
    requires LMatRNum<Matrix<NumericA>, NumericB> && CompoundSubtractable<NumericA, NumericB>
  friend inline Matrix<NumericA> &operator-=(Matrix<NumericA> &,
                                             NumericB &&) noexcept(NoexceptCompoundSubtractable<NumericA, NumericB>);
  template <class NumericA, class NumericB>
    requires LMatRNum<Matrix<NumericA>, NumericB> && CompoundMultipliable<NumericA, NumericB>
  friend inline Matrix<NumericA> &operator*=(Matrix<NumericA> &,
                                             NumericB &&) noexcept(NoexceptCompoundMultipliable<NumericA, NumericB>);
  template <class NumericA, class NumericB>
    requires LMatRNum<Matrix<NumericA>, NumericB> && CompoundDivisible<NumericA, NumericB>
  friend inline Matrix<NumericA> &operator/=(Matrix<NumericA> &,
                                             NumericB &&) noexcept(NoexceptCompoundDivisible<NumericA, NumericB>);
  template <class NumericA, class NumericB>
    requires LMatRNum<Matrix<NumericA>, NumericB> && CompoundModdable<NumericA, NumericB>
  friend inline Matrix<NumericA> &operator%=(Matrix<NumericA> &,
                                             NumericB &&) noexcept(NoexceptCompoundModdable<NumericA, NumericB>);

  template <class LMatrix, class RMatrix>
    requires BothMatrix<LMatrix, RMatrix> && CompoundAddable<LMatrix, RMatrix> &&
             ValidContent<SumType<Numeric_t<LMatrix>, Numeric_t<RMatrix>>, Numeric_t<LMatrix>>
  friend inline auto operator+(LMatrix &&, RMatrix &&);
  template <class LMatrix, class RMatrix>
    requires BothMatrix<LMatrix, RMatrix> && CompoundSubtractable<LMatrix, RMatrix> &&
             ValidContent<SubType<Numeric_t<LMatrix>, Numeric_t<RMatrix>>, Numeric_t<LMatrix>>
  friend inline auto operator-(LMatrix &&, RMatrix &&);
  template <class LMatrix, class RMatrix>
    requires BothMatrix<LMatrix, RMatrix> && CompoundMultipliable<LMatrix, RMatrix> &&
             ValidContent<MulType<Numeric_t<LMatrix>, Numeric_t<RMatrix>>, Numeric_t<LMatrix>>
  friend inline auto operator*(LMatrix &&, RMatrix &&);

  template <class LNumeric, class RNumeric>
    requires SameMatrixDepth<LNumeric, RNumeric> && Comparable<LNumeric, RNumeric>
  friend inline bool operator==(const Matrix<LNumeric> &,
                                const Matrix<RNumeric> &) noexcept(NoexceptComparable<LNumeric, RNumeric>);

  template <class LNumeric, class RNumeric>
    requires SameMatrixDepth<LNumeric, RNumeric> && Comparable<LNumeric, RNumeric>
  friend inline bool operator!=(const Matrix<LNumeric> &lhs,
                                const Matrix<RNumeric> &rhs) noexcept(noexcept(lhs == rhs));

  friend std::istream &operator>> <>(std::istream &, Matrix<Numeric> &);
  friend std::ostream &operator<< <>(std::ostream &, const Matrix<Numeric> &);

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
    requires Subtractable<Numeric, OtherNumeric> && ValidContent<OtherNumeric, SubType<Numeric, OtherNumeric>>
  void RightCompoundSub(Matrix<OtherNumeric> &other) const {
    if (other.Rows() != Rows() || other.Cols() != Cols()) {
      throw InvalidMatrixOperation{};
    }
    for (size_t r = 0; r < Rows(); ++r) {
      for (size_t c = 0; c < Cols(); ++c) {
        if constexpr (Assignable<OtherNumeric, SubType<Numeric, OtherNumeric>>) {
          other(r, c) = ((*this)(r, c)) - std::move(other(r, c));
        } else {
          other(r, c) = static_cast<OtherNumeric>(((*this)(r, c)) - std::move(other(r, c)));
        }
      }
    }
  }

  template <class OtherNumeric>
  void RightCompoundMult(Matrix<OtherNumeric> &other) const
    requires HasZero<Numeric> && Multipliable<Numeric, OtherNumeric> &&
             CompoundAddable<OtherNumeric, MulType<Numeric, OtherNumeric>> && Movable<OtherNumeric>
  {
    if (other.Rows() != Cols()) {
      throw InvalidMatrixOperation{};
    }
    std::vector<OtherNumeric> buffer;
    other.contents_.resize(Rows(), std::vector<OtherNumeric>(other.Cols(), static_cast<OtherNumeric>(0)));
    for (size_t c = 0; c < other.Cols(); ++c) {
      buffer.resize(Rows(), static_cast<Numeric>(0));
      for (size_t r = 0; r < Rows(); ++r) {
        buffer[r] = static_cast<Numeric>(0);
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
  requires Matrixable<Numeric>
inline Matrix<Numeric>::Matrix(std::vector<std::vector<Numeric>> &&cont) : contents_(std::move(cont)) {
  ContentsValidityCheck();
}

template <class Numeric>
  requires Matrixable<Numeric>
template <class SomeNumeric>
  requires ValidContent<Numeric, SomeNumeric>
inline Matrix<Numeric>::Matrix(size_t rows, size_t cols, SomeNumeric &&value)
    : contents_(rows, std::vector<Numeric>(cols, std::is_same_v<Numeric, std::decay_t<SomeNumeric>>
                                                     ? std::forward<SomeNumeric>(value)
                                                     : static_cast<Numeric>(std::forward<SomeNumeric>(value)))) {
}
template <class Numeric>
  requires Matrixable<Numeric>
inline Matrix<Numeric>::Matrix(size_t rows, size_t cols)
  requires HasZero<Numeric>
    : contents_(rows, std::vector<Numeric>(cols, static_cast<Numeric>(0))) {
}

template <class Numeric>
  requires Matrixable<Numeric>
inline Matrix<Numeric>::Matrix(const std::vector<std::vector<Numeric>> &cont)
  requires Copyable<Numeric>
    : contents_(cont) {
  ContentsValidityCheck();
}

template <class Numeric>
  requires Matrixable<Numeric>
template <class SomeNumeric>
  requires ValidContent<Numeric, SomeNumeric>
inline Matrix<Numeric>::Matrix(const std::vector<std::vector<SomeNumeric>> &cont)
    : Matrix(cont.size(), cont.empty() ? 0 : cont.front().size()) {
  for (size_t r = 0; r < Rows(); ++r) {
    for (size_t c = 0; c < Cols(); ++c) {
      contents_[r][c] = static_cast<Numeric>(cont[r][c]);
    }
  }
}

template <class Numeric>
  requires Matrixable<Numeric>
template <class SomeNumeric>
  requires ValidContent<Numeric, SomeNumeric>
inline Matrix<Numeric>::Matrix(std::vector<std::vector<SomeNumeric>> &&cont) {
  Matrix<SomeNumeric> placeholder(std::move(cont));
  *this = std::move(placeholder);
}

template <class Numeric>
  requires Matrixable<Numeric>
template <OnlyMatrix OtherMatrix>
  requires ValidContent<Numeric, Numeric_t<OtherMatrix>>
inline Matrix<Numeric>::Matrix(OtherMatrix &&other) : Matrix(other.Rows(), other.Cols()) {
  for (size_t r = 0; r < Rows(); ++r) {
    for (size_t c = 0; c < Cols(); ++c) {
      contents_[r][c] = static_cast<Numeric>(std::forward<OtherMatrix>(other)(r, c));
    }
  }
}

template <class Numeric>
  requires Matrixable<Numeric>
inline Matrix<Numeric> Matrix<Numeric>::Eye(size_t size)
  requires HasZero<Numeric> && HasOne<Numeric>
{
  return Eye(size, static_cast<Numeric>(1), static_cast<Numeric>(0));
}

template <class Numeric>
  requires Matrixable<Numeric>
template <class SomeNumeric>
  requires ValidContent<Numeric, SomeNumeric> && HasZero<Numeric>
inline Matrix<Numeric> Matrix<Numeric>::Eye(size_t size, SomeNumeric &&value) {
  return Eye(size, std::forward<SomeNumeric>(value), static_cast<Numeric>(0));
}

template <class Numeric>
  requires Matrixable<Numeric>
template <class SomeNumericA, class SomeNumericB>
  requires ValidContent<Numeric, SomeNumericA> && ValidContent<Numeric, SomeNumericB>
inline Matrix<Numeric> Matrix<Numeric>::Eye(size_t size, SomeNumericA &&value, SomeNumericB &&zero_override) {
  Matrix<Numeric> eye(size, size, std::forward<SomeNumericB>(zero_override));
  if constexpr (std::is_rvalue_reference_v<decltype(value)>) {
    for (size_t i = 0; i < size; ++i) {
      eye(i, i) = value;
    }
  } else {
    for (size_t i = 0; i + 1 < size; ++i) {
      eye(i, i) = value;
    }
    if (size > 0) {
      eye(size - 1, size - 1) = std::move(value);
    }
  }
  return std::move(eye);
}

template <class Numeric>
  requires Matrixable<Numeric>
template <OnlyMatrix OtherMatrix>
inline Matrix<Numeric> &Matrix<Numeric>::operator=(OtherMatrix &&other)
  requires ValidContent<Numeric, Numeric_t<OtherMatrix>> && (!std::is_same_v<Numeric, Numeric_t<OtherMatrix>>)
{
  *this = Matrix<Numeric>(other.Rows(), other.Cols());
  for (size_t r = 0; r < Rows(); ++r) {
    for (size_t c = 0; c < Cols(); ++c) {
      if constexpr (Assignable<Numeric, Numeric_t<OtherMatrix>>) {
        (*this)(r, c) = std::forward<OtherMatrix>(other)(r, c);
      } else {
        (*this)(r, c) = static_cast<Numeric>(std::forward<OtherMatrix>(other)(r, c));
      }
    }
  }
}

template <class Numeric>
  requires Matrixable<Numeric>
inline void Matrix<Numeric>::SwapRows(size_t row1, size_t row2) noexcept {
  contents_[row1].swap(contents_[row2]);
}
template <class Numeric>
  requires Matrixable<Numeric>
inline void Matrix<Numeric>::SwapCols(size_t col1, size_t col2) noexcept(NoexceptMovable<Numeric>) {
  for (auto &row : contents_) {
    std::swap(row[col1], row[col2]);
  }
}

template <class Numeric>
  requires Matrixable<Numeric>
template <class SomeNumeric>
  requires Multipliable<Numeric, SomeNumeric> && CompoundAddable<Numeric, MulType<Numeric, SomeNumeric>>
inline void Matrix<Numeric>::AddRow(size_t source, size_t target, SomeNumeric &&mult) noexcept(
    NoexceptCompoundAddable<Numeric, MulType<Numeric, SomeNumeric>> && NoexceptMultipliable<Numeric, SomeNumeric>) {
  for (size_t c = 0; c + 1 < Cols(); ++c) {
    contents_[target][c] += contents_[source][c] * mult;
  }
  if (Cols() > 0) {
    contents_[target].back() += contents_[source].back() * std::forward<SomeNumeric>(mult);
  }
}
template <class Numeric>
  requires Matrixable<Numeric>
template <class SomeNumeric>
  requires Multipliable<Numeric, SomeNumeric> && CompoundAddable<Numeric, MulType<Numeric, SomeNumeric>>
inline void Matrix<Numeric>::AddCol(size_t source, size_t target, SomeNumeric &&mult) noexcept(
    NoexceptCompoundAddable<Numeric, MulType<Numeric, SomeNumeric>> && NoexceptMultipliable<Numeric, SomeNumeric>) {
  for (size_t r = 0; r + 1 < Rows(); ++r) {
    contents_[r][target] += contents_[r][source] * mult;
  }
  if (Rows() > 0) {
    contents_.back()[target] += contents_.back()[source] * std::forward(mult);
  }
}
template <class Numeric>
  requires Matrixable<Numeric>
inline void Matrix<Numeric>::AddRow(size_t source, size_t target) noexcept(NoexceptCompoundAddable<Numeric, Numeric>)
  requires CompoundAddable<Numeric>
{
  for (size_t c = 0; c < Cols(); ++c) {
    contents_[target][c] += contents_[source][c];
  }
}
template <class Numeric>
  requires Matrixable<Numeric>
inline void Matrix<Numeric>::AddCol(size_t source, size_t target) noexcept(NoexceptCompoundAddable<Numeric, Numeric>)
  requires CompoundAddable<Numeric>
{
  for (size_t r = 0; r < Rows(); ++r) {
    contents_[r][target] += contents_[r][source];
  }
}

template <class Numeric>
  requires Matrixable<Numeric>
template <class SomeNumeric>
  requires CompoundMultipliable<Numeric, SomeNumeric>
inline void Matrix<Numeric>::MultRow(size_t row,
                                     SomeNumeric &&mult) noexcept(NoexceptCompoundMultipliable<Numeric, SomeNumeric>) {
  if constexpr (std::is_rvalue_reference_v<decltype(mult)>) {
    for (size_t c = 0; c + 1 < Cols(); ++c) {
      contents_[row][c] *= mult;
    }
    if (Cols() > 0) {
      contents_[row].back() *= std::move(mult);
    }
  } else {
    for (size_t c = 0; c < Cols(); ++c) {
      contents_[row][c] *= mult;
    }
  }
}
template <class Numeric>
  requires Matrixable<Numeric>
template <class SomeNumeric>
  requires CompoundMultipliable<Numeric, SomeNumeric>
inline void Matrix<Numeric>::MultCol(size_t col,
                                     SomeNumeric &&mult) noexcept(NoexceptCompoundMultipliable<Numeric, SomeNumeric>) {
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
  requires Matrixable<Numeric>
inline Numeric &Matrix<Numeric>::operator()(size_t row, size_t col) noexcept {
  return contents_[row][col];
}
template <class Numeric>
  requires Matrixable<Numeric>
inline const Numeric &Matrix<Numeric>::operator()(size_t row, size_t col) const noexcept {
  return contents_[row][col];
}

template <class Numeric>
  requires Matrixable<Numeric>
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
  requires Matrixable<Numeric>
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
  requires Matrixable<Numeric>
inline size_t Matrix<Numeric>::Rows() const noexcept {
  return contents_.size();
}
template <class Numeric>
  requires Matrixable<Numeric>
inline size_t Matrix<Numeric>::Cols() const noexcept {
  return contents_.empty() ? 0 : contents_.front().size();
}

template <class Numeric>
  requires Matrixable<Numeric>
std::ostream &operator<<(std::ostream &out, const Matrix<Numeric> &matrix) {
  if (matrix.Empty()) {
    return out;
  }
  std::vector<size_t> lengths(matrix.Cols(), 0);
  std::string output;
  for (size_t col = 0; col < matrix.Cols(); col++) {
    for (size_t row = 0; row < matrix.Rows(); row++) {
      output = (std::ostringstream() << matrix(row, col)).str();
      lengths[col] = std::max(lengths[col], output.size());
    }
  }
  for (size_t row = 0; row < matrix.Rows(); ++row) {
    for (size_t col = 0; col < matrix.Cols(); ++col) {
      output = (std::ostringstream() << matrix(row, col)).str();
      out << output;
      for (size_t i = 0; i < lengths[col] - output.size() + 1; i++) {
        out << ' ';
      }
    }
    out << '\n';
  }
  return out;
}

template <class Numeric>
  requires Matrixable<Numeric>
std::istream &operator>>(std::istream &in, Matrix<Numeric> &matrix) {
  std::string input;
  std::getline(in, input);
  if (input.empty()) {
    matrix.contents_.clear();
    return in;
  }
  Numeric value;
  for (size_t i = 0; !input.empty(); std::getline(in, input), i++) {
    std::istringstream line(std::move(input));
    if (matrix.contents_.size() <= i) {
      matrix.contents_.emplace_back();
    }
    matrix.contents_[i].clear();
    while (line >> value) {
      matrix.contents_[i].emplace_back(std::move(value));
    }
    if (matrix.contents_[i].size() != matrix.contents_[0].size()) {
      matrix.contents_.clear();
      throw InvalidMatrixString{};
    }
  }
  return in;
}

template <class NumericA, class OtherMatrix>
  requires Matrixable<NumericA> && BothMatrix<Matrix<NumericA>, OtherMatrix> &&
           CompoundAddable<NumericA, Numeric_t<OtherMatrix>>
inline Matrix<NumericA> &operator+=(Matrix<NumericA> &self, OtherMatrix &&other) {
  if (self.Rows() != other.Rows() || self.Cols() != other.Cols()) {
    throw InvalidMatrixOperation{};
  }
  for (size_t i = 0; i < self.Rows(); ++i) {
    for (size_t j = 0; j < self.Cols(); ++j) {
      self(i, j) += std::is_same_v<NumericA, Numeric_t<OtherMatrix>>
                        ? std::forward<OtherMatrix>(other)(i, j)
                        : static_cast<NumericA>(std::forward<OtherMatrix>(other)(i, j));
    }
  }
  return self;
}
template <class NumericA, class OtherMatrix>
  requires Matrixable<NumericA> && BothMatrix<Matrix<NumericA>, OtherMatrix> &&
           CompoundSubtractable<NumericA, Numeric_t<OtherMatrix>>
inline Matrix<NumericA> &operator-=(Matrix<NumericA> &self, OtherMatrix &&other) {
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
template <class NumericA, class OtherMatrix>
  requires Matrixable<NumericA> && BothMatrix<Matrix<NumericA>, OtherMatrix> &&
           Multipliable<NumericA, Numeric_t<OtherMatrix>> &&
           CompoundAddable<NumericA, MulType<NumericA, Numeric_t<OtherMatrix>>>
inline Matrix<NumericA> &operator*=(Matrix<NumericA> &self, OtherMatrix &&other) {
  if (self.Cols() != other.Rows()) {
    throw InvalidMatrixOperation{};
  }
  if constexpr (std::is_same_v<Numeric_t<OtherMatrix>, NumericA>) {
    if (&self == &other) {
      return self = self * other;
    }
  }
  std::vector<NumericA> buffer(other.Cols(), static_cast<NumericA>(0));
  size_t rows = self.Rows();
  size_t cols = self.Cols();
  for (size_t r = 0; r < rows; ++r) {
    buffer.resize(other.Cols(), static_cast<NumericA>(0));
    for (size_t c = 0; c < other.Cols(); ++c) {
      buffer[c] = static_cast<NumericA>(0);
      for (size_t k = 0; k < cols; ++k) {
        buffer[c] += self(r, k) * std::forward<OtherMatrix>(other)(k, c);
      }
    }
    buffer.swap(self.contents_[r]);
  }
  return self;
}

template <class NumericA, class NumericB>
  requires LMatRNum<Matrix<NumericA>, NumericB> && CompoundAddable<NumericA, NumericB>
inline Matrix<NumericA> &operator+=(Matrix<NumericA> &matrix,
                                    NumericB &&number) noexcept(NoexceptCompoundAddable<NumericA, NumericB>) {
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
  requires LMatRNum<Matrix<NumericA>, NumericB> && CompoundSubtractable<NumericA, NumericB>
inline Matrix<NumericA> &operator-=(Matrix<NumericA> &matrix,
                                    NumericB &&number) noexcept(NoexceptCompoundSubtractable<NumericA, NumericB>) {
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
  requires LMatRNum<Matrix<NumericA>, NumericB> && CompoundMultipliable<NumericA, NumericB>
inline Matrix<NumericA> &operator*=(Matrix<NumericA> &matrix,
                                    NumericB &&number) noexcept(NoexceptCompoundMultipliable<NumericA, NumericB>) {
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
  requires LMatRNum<Matrix<NumericA>, NumericB> && CompoundDivisible<NumericA, NumericB>
inline Matrix<NumericA> &operator/=(Matrix<NumericA> &matrix,
                                    NumericB &&number) noexcept(NoexceptCompoundDivisible<NumericA, NumericB>) {
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
template <class NumericA, class NumericB>
  requires LMatRNum<Matrix<NumericA>, NumericB> && CompoundModdable<NumericA, NumericB>
inline Matrix<NumericA> &operator%=(Matrix<NumericA> &matrix,
                                    NumericB &&number) noexcept(NoexceptCompoundModdable<NumericA, NumericB>) {
  if (matrix.Rows() == 1 && matrix.Cols() == 1) {
    matrix(0, 0) %= std::forward<NumericB>(number);
    return matrix;
  }
  for (size_t r = 0; r < matrix.Rows(); ++r) {
    for (size_t c = 0; c < matrix.Cols(); ++c) {
      matrix(r, c) %= number;
    }
  }
  return matrix;
}

template <class LMatrix, class RMatrix>
  requires BothMatrix<LMatrix, RMatrix> && CompoundAddable<LMatrix, RMatrix> &&
           ValidContent<SumType<Numeric_t<LMatrix>, Numeric_t<RMatrix>>, Numeric_t<LMatrix>>
inline auto operator+(LMatrix &&first, RMatrix &&second) {
  using LType = Numeric_t<LMatrix>;
  using RType = Numeric_t<RMatrix>;
  using EndType = SumType<LType, RType>;
  if constexpr (std::is_same_v<LType, EndType> && std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first += std::forward<RMatrix>(second));
  } else if constexpr (CompoundAddable<RMatrix, LMatrix> && std::is_same_v<RType, EndType> &&
                       std::is_rvalue_reference_v<decltype(second)>) {
    return std::move(second += std::forward<LMatrix>(first));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(first));
    return std::move(copy += std::forward<RMatrix>(second));
  }
}
template <class LMatrix, class RMatrix>
  requires BothMatrix<LMatrix, RMatrix> && CompoundSubtractable<LMatrix, RMatrix> &&
           ValidContent<SubType<Numeric_t<LMatrix>, Numeric_t<RMatrix>>, Numeric_t<LMatrix>>
inline auto operator-(LMatrix &&first, RMatrix &&second) {
  using LType = Numeric_t<LMatrix>;
  using RType = Numeric_t<RMatrix>;
  using EndType = SubType<LType, RType>;
  if constexpr (std::is_same_v<LType, EndType> && std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first -= std::forward<RMatrix>(second));
  } else if constexpr (RightCompoundSubtractable<LMatrix, RMatrix> && std::is_same_v<RType, EndType> &&
                       std::is_rvalue_reference_v<decltype(second)>) {
    return std::move((first.RightCompoundSub(second), second));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(first));
    return std::move(copy -= std::forward<RMatrix>(second));
  }
}
template <class LMatrix, class RMatrix>
  requires BothMatrix<LMatrix, RMatrix> && CompoundMultipliable<LMatrix, RMatrix> &&
           ValidContent<MulType<Numeric_t<LMatrix>, Numeric_t<RMatrix>>, Numeric_t<LMatrix>>
inline auto operator*(LMatrix &&first, RMatrix &&second) {
  using LType = Numeric_t<LMatrix>;
  using RType = Numeric_t<RMatrix>;
  using EndType = MulType<LType, RType>;
  if constexpr (std::is_same_v<LType, EndType> && std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first *= std::forward<RMatrix>(second));
  } else if constexpr (RightCompoundMultipliable<LMatrix, RMatrix> && std::is_same_v<RType, EndType> &&
                       std::is_rvalue_reference_v<decltype(second)>) {
    return std::move((first.RightCompoundMult(second), second));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(first));
    return std::move(copy *= std::forward<RMatrix>(second));
  }
}

template <class LMatrix, class RNumeric>
  requires LMatRNum<LMatrix, RNumeric> && CompoundAddable<LMatrix, RNumeric> &&
           ValidContent<SumType<Numeric_t<LMatrix>, RNumeric>, Numeric_t<LMatrix>>
inline auto operator+(LMatrix &&matrix, RNumeric &&number) noexcept(
    std::is_same_v<Numeric_t<LMatrix>, SumType<Numeric_t<LMatrix>, RNumeric>> &&
    std::is_rvalue_reference_v<decltype(matrix)> && noexcept(matrix += std::forward<RNumeric>(number))) {
  using LType = Numeric_t<LMatrix>;
  using RType = typename std::decay_t<RNumeric>;
  using EndType = SumType<LType, RType>;
  if constexpr (std::is_same_v<LType, EndType> && std::is_rvalue_reference_v<decltype(matrix)>) {
    return std::move(matrix += std::forward<RNumeric>(number));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(matrix));
    return std::move(copy += std::forward<RNumeric>(number));
  }
}
template <class LMatrix, class RNumeric>
  requires LMatRNum<LMatrix, RNumeric> && CompoundSubtractable<LMatrix, RNumeric> &&
           ValidContent<SubType<Numeric_t<LMatrix>, RNumeric>, Numeric_t<LMatrix>>
inline auto operator-(LMatrix &&matrix, RNumeric &&number) noexcept(
    std::is_same_v<Numeric_t<LMatrix>, SubType<Numeric_t<LMatrix>, RNumeric>> &&
    std::is_rvalue_reference_v<decltype(matrix)> && noexcept(matrix -= std::forward<RNumeric>(number))) {
  using LType = Numeric_t<LMatrix>;
  using RType = typename std::decay_t<RNumeric>;
  using EndType = SubType<LType, RType>;
  if constexpr (std::is_same_v<LType, EndType> && std::is_rvalue_reference_v<decltype(matrix)>) {
    return std::move(matrix -= std::forward<RNumeric>(number));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(matrix));
    return std::move(copy -= std::forward<RNumeric>(number));
  }
}
template <class LMatrix, class RNumeric>
  requires LMatRNum<LMatrix, RNumeric> && CompoundMultipliable<LMatrix, RNumeric> &&
           ValidContent<MulType<Numeric_t<LMatrix>, RNumeric>, Numeric_t<LMatrix>>
inline auto operator*(LMatrix &&matrix, RNumeric &&number) noexcept(
    std::is_same_v<Numeric_t<LMatrix>, MulType<Numeric_t<LMatrix>, RNumeric>> &&
    std::is_rvalue_reference_v<decltype(matrix)> && noexcept(matrix *= std::forward<RNumeric>(number))) {
  using LType = Numeric_t<LMatrix>;
  using RType = typename std::decay_t<RNumeric>;
  using EndType = MulType<LType, RType>;
  if constexpr (std::is_same_v<LType, EndType> && std::is_rvalue_reference_v<decltype(matrix)>) {
    return std::move(matrix *= std::forward<RNumeric>(number));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(matrix));
    return std::move(copy *= std::forward<RNumeric>(number));
  }
}

template <class LNumeric, class RMatrix>
  requires LNumRMat<LNumeric, RMatrix> && Addable<RMatrix, LNumeric>
inline auto operator+(LNumeric &&number, RMatrix &&matrix) noexcept(noexcept(std::forward<RMatrix>(matrix) +
                                                                             std::forward<LNumeric>(number))) {
  return std::forward<RMatrix>(matrix) + std::forward<LNumeric>(number);
}
template <class LNumeric, class RMatrix>
  requires LNumRMat<LNumeric, RMatrix> && CompoundSubtractable<LNumeric, Numeric_t<RMatrix>> &&
           ValidContent<SubType<LNumeric, Numeric_t<RMatrix>>, LNumeric>
inline auto operator-(LNumeric &&number, RMatrix &&matrix) noexcept(
    std::is_same_v<Numeric_t<RMatrix>, SubType<LNumeric, Numeric_t<RMatrix>>> &&
    std::is_rvalue_reference_v<decltype(matrix)> && NoexceptSubtractable<LNumeric, Numeric_t<RMatrix>> &&
    NoexceptMovable<Numeric_t<RMatrix>>) {
  using RType = Numeric_t<RMatrix>;
  using EndType = SubType<LNumeric, RType>;
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
  buffer = Matrix<EndType>(matrix.Rows(), matrix.Cols(), std::forward<LNumeric>(number));
  return std::move(buffer -= matrix);
}
template <class LNumeric, class RMatrix>

  requires LNumRMat<LNumeric, RMatrix> && Multipliable<RMatrix, LNumeric>
inline auto operator*(LNumeric &&number, RMatrix &&matrix) noexcept(noexcept(std::forward<RMatrix>(matrix) *
                                                                             std::forward<LNumeric>(number))) {
  return std::forward<RMatrix>(matrix) * std::forward<LNumeric>(number);
}

template <class LNumeric, class RNumeric>
  requires SameMatrixDepth<LNumeric, RNumeric> && Comparable<LNumeric, RNumeric>
inline bool operator==(const Matrix<LNumeric> &lhs,
                       const Matrix<RNumeric> &rhs) noexcept(NoexceptComparable<LNumeric, RNumeric>) {
  if (lhs.Rows() != rhs.Rows() || lhs.Cols() != rhs.Cols()) {
    return false;
  }
  for (size_t r = 0; r < lhs.Rows(); ++r) {
    for (size_t c = 0; c < lhs.Cols(); ++c) {
      if (!(lhs(r, c) == rhs(r, c))) {
        return false;
      }
    }
  }
  return true;
}
template <class LNumeric, class RNumeric>
  requires SameMatrixDepth<LNumeric, RNumeric> && Comparable<LNumeric, RNumeric>
inline bool operator!=(const Matrix<LNumeric> &lhs, const Matrix<RNumeric> &rhs) noexcept(noexcept(lhs == rhs)) {
  return !(lhs == rhs);
}