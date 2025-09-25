#pragma once
#include <iostream>
#include <sstream>
#include <vector>

#include "definitions.hpp"

class MatrixOutOfRange : public MachineError {
 public:
  MatrixOutOfRange(size_t range, size_t reference) {
    message_ =
        (std::ostringstream() << "Matrix has " << range << " range but element "
                              << reference << " was referenced")
            .str();
  }

  [[nodiscard]] const char *what() const noexcept override {
    return message_.c_str();
  }

 private:
  std::string message_;
};

class InvalidMatrixBase : public MachineError {
 private:
  std::string message_ = "Vector used for Matrix construction is invalid";
};

class InvalidMatrixOperation : public MachineError {
 private:
  std::string message_ =
      "The requested operation cannot be performed on provided matrixes";
};

class InvalidMatrixString : public MachineError {
 private:
  std::string message_ = "String used for Matrix construction is invalid";
};

template <class Numeric>
class Matrix;

template <class T>
concept OnlyMatrix =
    std::same_as<Matrix<typename std::decay_t<T>::numeric_t>, std::decay_t<T>>;

template <class Numeric>
class Matrix {
 public:
  Matrix() : contents_(), rows_(0), cols_(0) {}

  using numeric_t = Numeric;

  Matrix(size_t rows, size_t cols) {
    contents_ = std::vector(rows, std::vector(cols, Numeric(0)));
    rows_ = rows;
    cols_ = cols;
  }

  explicit Matrix(std::vector<std::vector<Numeric>> &&cont)
      : contents_(std::move(cont)) {
    rows_ = contents_.size();
    if (rows_ > 0) {
      cols_ = contents_[0].size();
    } else {
      cols_ = 0;
    }
    ContentsValidityCheck();
  }
  explicit Matrix(const std::vector<std::vector<Numeric>> &cont)
      : contents_(cont) {
    rows_ = contents_.size();
    if (rows_ > 0) {
      cols_ = contents_[0].size();
    } else {
      cols_ = 0;
    }
    ContentsValidityCheck();
  }

  Matrix(Matrix<Numeric> &&source) noexcept {
    contents_ = std::move(source.contents_);
    rows_ = source.rows_;
    cols_ = source.cols_;
    source = Matrix();
  }

  Matrix(const Matrix<Numeric> &source) {
    contents_ = source.contents_;
    rows_ = source.rows_;
    cols_ = source.cols_;
  }

  template <OnlyMatrix OtherMatrix>
  explicit Matrix(OtherMatrix &&other)
      : contents_(other.Rows(), std::vector<Numeric>(other.Cols(), Numeric(0))),
        rows_(other.Rows()),
        cols_(other.Cols()) {
    using OtherNumericType = typename std::decay_t<OtherMatrix>::numeric_t;
    for (size_t r = 0; r < rows_; ++r) {
      for (size_t c = 0; c < cols_; ++c) {
        contents_[r][c] = std::forward<OtherMatrix>(other)(r, c);
      }
    }
  }

  static Matrix<Numeric> Eye(const Numeric &value, size_t size) {
    Matrix<Numeric> identity(size, size);
    for (size_t i = 0; i < size; i++) {
      identity.contents_[i][i] = value;
    }
    return identity;
  }

  Matrix<Numeric> &operator=(const Matrix<Numeric> &other) {
    contents_ = other.contents_;
    rows_ = contents_.size();
    if (!contents_.empty()) {
      cols_ = contents_[0].size();
    }
    return *this;
  }
  Matrix<Numeric> &operator=(Matrix<Numeric> &&other) noexcept {
    contents_ = std::move(other.contents_);
    rows_ = contents_.size();
    if (!contents_.empty()) {
      cols_ = contents_[0].size();
    }
    other.rows_ = 0;
    other.cols_ = 0;
    return *this;
  }

  void SwapRows(size_t row1, size_t row2) {
    if (row1 >= rows_ || row2 >= rows_) {
      throw MatrixOutOfRange(rows_, std::max(row1, row2));
    }
    contents_[row1].swap(contents_[row2]);
  }

  void AddRow(size_t source, size_t target, const Numeric &mult) {
    if (source >= rows_ || target >= rows_) {
      throw MatrixOutOfRange(rows_, std::max(source, target));
    }
    for (size_t col = 0; col < cols_; col++) {
      contents_[target][col] += contents_[source][col] * mult;
    }
  }

  void MultRow(size_t row, Numeric mult) {
    if (row >= rows_) {
      throw MatrixOutOfRange(rows_, row);
    }
    for (size_t col = 0; col < cols_; col++) {
      contents_[row][col] *= mult;
    }
  }

  void SwapCols(size_t col1, size_t col2) {
    if (col1 >= cols_ || col2 >= cols_) {
      throw MatrixOutOfRange(cols_, std::max(col1, col2));
    }
    for (size_t row = 0; row < rows_; row++) {
      std::swap(contents_[row][col1], contents_[row][col2]);
    }
  }

  void AddCol(size_t source, size_t target, const Numeric &mult) {
    if (source >= cols_ || target >= cols_) {
      throw MatrixOutOfRange(cols_, std::max(source, target));
    }
    for (size_t row = 0; row < rows_; row++) {
      contents_[row][target] += contents_[row][source] * mult;
    }
  }

  void MultCol(size_t col, const Numeric &mult) {
    if (col >= cols_) {
      throw MatrixOutOfRange(cols_, col);
    }
    for (size_t row = 0; row < rows_; row++) {
      contents_[row][col] *= mult;
    }
  }

  Numeric &operator()(size_t row, size_t col) {
    if (row >= rows_) {
      throw MatrixOutOfRange(rows_, row);
    }
    if (col >= cols_) {
      throw MatrixOutOfRange(cols_, col);
    }
    return contents_[row][col];
  }
  const Numeric &operator()(size_t row, size_t col) const {
    if (row >= rows_) {
      throw MatrixOutOfRange(rows_, row);
    }
    if (col >= cols_) {
      throw MatrixOutOfRange(cols_, col);
    }
    return contents_[row][col];
  }

  size_t Rows() const noexcept { return rows_; }
  size_t Cols() const noexcept { return cols_; }

  bool Empty() const noexcept { return rows_ == 0; }

  template <class NumericA, OnlyMatrix OtherMatrix>
  friend Matrix<NumericA> &operator+=(Matrix<NumericA> &, OtherMatrix &&);
  template <class NumericA, OnlyMatrix OtherMatrix>
  friend Matrix<NumericA> &operator-=(Matrix<NumericA> &, OtherMatrix &&);
  template <class NumericA, OnlyMatrix OtherMatrix>
  friend Matrix<NumericA> &operator*=(Matrix<NumericA> &, OtherMatrix &&);

  template <class NumericA, class NumericB>
  friend Matrix<NumericA> &operator+=(Matrix<NumericA> &, NumericB &&);
  template <class NumericA, class NumericB>
  friend Matrix<NumericA> &operator-=(Matrix<NumericA> &, NumericB &&);
  template <class NumericA, class NumericB>
  friend Matrix<NumericA> &operator*=(Matrix<NumericA> &, NumericB &&);
  template <class NumericA, class NumericB>
  friend Matrix<NumericA> &operator/=(Matrix<NumericA> &, NumericB &&);

  template <OnlyMatrix LMatrix, OnlyMatrix RMatrix>
  friend auto operator+(LMatrix &&, RMatrix &&);
  template <OnlyMatrix LMatrix, OnlyMatrix RMatrix>
  friend auto operator-(LMatrix &&, RMatrix &&);
  template <OnlyMatrix LMatrix, OnlyMatrix RMatrix>
  friend auto operator*(LMatrix &&, RMatrix &&);

  template <OnlyMatrix LMatrix, class RNumeric>
  friend auto operator+(LMatrix &&, RNumeric &&);
  template <OnlyMatrix LMatrix, class RNumeric>
  friend auto operator-(LMatrix &&, RNumeric &&);
  template <OnlyMatrix LMatrix, class RNumeric>
  friend auto operator*(LMatrix &&, RNumeric &&);

  template <class LNumeric, OnlyMatrix RMatrix>
  friend auto operator+(LNumeric &&, RMatrix &&);
  template <class LNumeric, OnlyMatrix RMatrix>
  friend auto operator-(LNumeric &&, RMatrix &&);
  template <class LNumeric, OnlyMatrix RMatrix>
  friend auto operator*(LNumeric &&, RMatrix &&);

 private:
  std::vector<std::vector<Numeric>> contents_;
  size_t rows_;
  size_t cols_;

  void ContentsValidityCheck() {
    if (rows_ == 0) {
      return;
    }
    for (const auto &row : contents_) {
      if (row.size() != cols_) {
        throw InvalidMatrixBase();
      }
    }
  }

  template <class OtherNumeric>
  void RightCompoundSub(Matrix<OtherNumeric> &other) const {
    if (other.Rows() != rows_ || other.Cols() != cols_) {
      throw InvalidMatrixOperation{};
    }
    for (size_t r = 0; r < rows_; ++r) {
      for (size_t c = 0; c < cols_; ++c) {
        other(r, c) = ((*this)(r, c)) - std::move(other(r, c));
      }
    }
  }
  template <class OtherNumeric>
  void RightCompoundMult(Matrix<OtherNumeric> &other) const {
    if (other.Rows() != cols_) {
      throw InvalidMatrixOperation{};
    }
    std::vector<OtherNumeric> buffer;
    for (size_t c = 0; c < other.Cols(); ++c) {
      buffer.resize(rows_, Numeric(0));
      for (size_t r = 0; r < rows_; ++r) {
        buffer[r] = Numeric(0);
        for (size_t k = 0; k < cols_; ++k) {
          buffer[r] += (((*this)(r, k)) * other(k, c));
        }
      }
      for (size_t r = 0; r < rows_; ++r) {
        other(r, c) = std::move(buffer[r]);
      }
    }
  }
};

template <class Numeric>
std::ostream &operator<<(std::ostream &out, const Matrix<Numeric> &matrix) {
  if (matrix.Empty()) {
    return out;
  }
  std::vector<size_t> lengths(matrix.Cols(), 0);
  for (size_t col = 0; col < matrix.Cols(); col++) {
    for (size_t row = 0; row < matrix.Rows(); row++) {
      lengths[col] =
          std::max(lengths[col],
                   (std::ostringstream() << matrix(row, col)).str().size());
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
          << std::string(
                 lengths[col] -
                     (std::ostringstream() << matrix(row, col)).str().size() +
                     1,
                 ' ');
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
Matrix<Numeric> &operator+=(Matrix<Numeric> &self, OtherMatrix &&other) {
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
Matrix<Numeric> &operator-=(Matrix<Numeric> &self, OtherMatrix &&other) {
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
Matrix<Numeric> &operator*=(Matrix<Numeric> &self, OtherMatrix &&other) {
  if (self.Cols() != other.Rows()) {
    throw InvalidMatrixOperation{};
  }
  if constexpr (std::is_same_v<typename std::decay_t<OtherMatrix>::numeric_t,
                               Numeric>) {
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
Matrix<NumericA> &operator+=(Matrix<NumericA> &matrix, NumericB &&number) {
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
Matrix<NumericA> &operator-=(Matrix<NumericA> &matrix, NumericB &&number) {
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
Matrix<NumericA> &operator*=(Matrix<NumericA> &matrix, NumericB &&number) {
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
Matrix<NumericA> &operator/=(Matrix<NumericA> &matrix, NumericB &&number) {
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
auto operator+(LMatrix &&first, RMatrix &&second) {
  using LType = typename std::decay_t<LMatrix>::numeric_t;
  using RType = typename std::decay_t<RMatrix>::numeric_t;
  using EndType = decltype(LType{} + RType{});
  if constexpr (std::is_same_v<LType, EndType> &&
                std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first += std::forward<RMatrix>(second));
  } else if constexpr (std::is_same_v<RType, EndType> &&
                       std::is_rvalue_reference_v<decltype(second)>) {
    return std::move(second += std::forward<LMatrix>(first));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(first));
    return std::move(copy += std::forward<RMatrix>(second));
  }
}
template <OnlyMatrix LMatrix, OnlyMatrix RMatrix>
auto operator-(LMatrix &&first, RMatrix &&second) {
  using LType = typename std::decay_t<LMatrix>::numeric_t;
  using RType = typename std::decay_t<RMatrix>::numeric_t;
  using EndType = decltype(LType{} - RType{});
  if constexpr (std::is_same_v<LType, EndType> &&
                std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first -= std::forward<RMatrix>(second));
  } else if constexpr (std::is_same_v<RType, EndType> &&
                       std::is_rvalue_reference_v<decltype(second)>) {
    return std::move((first.RightCompoundSub(second), second));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(first));
    return std::move(copy -= std::forward<RMatrix>(second));
  }
}
template <OnlyMatrix LMatrix, OnlyMatrix RMatrix>
auto operator*(LMatrix &&first, RMatrix &&second) {
  using LType = typename std::decay_t<LMatrix>::numeric_t;
  using RType = typename std::decay_t<RMatrix>::numeric_t;
  using EndType = decltype(LType{} * RType{});
  if constexpr (std::is_same_v<LType, EndType> &&
                std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first *= std::forward<RMatrix>(second));
  } else if constexpr (std::is_same_v<RType, EndType> &&
                       std::is_rvalue_reference_v<decltype(second)>) {
    return std::move((first.RightCompoundMult(second), second));
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(first));
    return std::move(copy *= std::forward<RMatrix>(second));
  }
}

template <OnlyMatrix LMatrix, class RNumeric>
auto operator+(LMatrix &&matrix, RNumeric &&number) {
  using LType = typename std::decay_t<LMatrix>::numeric_t;
  using RType = typename std::decay_t<RNumeric>;
  using EndType = decltype(LType{} + RType{});
  if constexpr (std::is_same_v<LType, EndType> &&
                std::is_rvalue_reference_v<decltype(matrix)>) {
    return std::move(matrix += number);
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(matrix));
    return std::move(copy += std::forward<RNumeric>(number));
  }
}
template <OnlyMatrix LMatrix, class RNumeric>
auto operator-(LMatrix &&matrix, RNumeric &&number) {
  using LType = typename std::decay_t<LMatrix>::numeric_t;
  using RType = typename std::decay_t<RNumeric>;
  using EndType = decltype(LType{} - RType{});
  if constexpr (std::is_same_v<LType, EndType> &&
                std::is_rvalue_reference_v<decltype(matrix)>) {
    return std::move(matrix -= number);
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(matrix));
    return std::move(copy -= std::forward<RNumeric>(number));
  }
}
template <OnlyMatrix LMatrix, class RNumeric>
auto operator*(LMatrix &&matrix, RNumeric &&number) {
  using LType = typename std::decay_t<LMatrix>::numeric_t;
  using RType = typename std::decay_t<RNumeric>;
  using EndType = decltype(LType{} * RType{});
  if constexpr (std::is_same_v<LType, EndType> &&
                std::is_rvalue_reference_v<decltype(matrix)>) {
    return std::move(matrix *= number);
  } else {
    Matrix<EndType> copy(std::forward<LMatrix>(matrix));
    return std::move(copy *= std::forward<RNumeric>(number));
  }
}

template <class LNumeric, OnlyMatrix RMatrix>
auto operator+(LNumeric &&number, RMatrix &&matrix) {
  return std::forward<RMatrix>(matrix) + std::forward<LNumeric>(number);
}
template <class LNumeric, OnlyMatrix RMatrix>
auto operator-(LNumeric &&number, RMatrix &&matrix) {
  using RType = typename std::decay_t<RMatrix>::numeric_t;
  using EndType = decltype(LNumeric{} - RType{});
  if constexpr (std::is_same_v<RType, EndType> &&
                std::is_rvalue_reference_v<decltype(matrix)>) {
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
    buffer(0, 0) =
        std::forward<LNumeric>(number) - std::forward<RMatrix>(matrix)(0, 0);
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
auto operator*(LNumeric &&number, RMatrix &&matrix) {
  return std::forward<RMatrix>(matrix) * std::forward<LNumeric>(number);
}