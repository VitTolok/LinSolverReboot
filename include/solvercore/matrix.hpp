#pragma once
#include "definitions.hpp"
#include <iostream>
#include <sstream>
#include <vector>

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

class InvalidMatrixString : public MachineError {
private:
  std::string message_ = "String used for Matrix construction is invalid";
};

template <class Numeric> class Matrix {
public:
  Matrix() : contents_(), rows_(0), cols_(0) {}

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
};

template <class Numeric>
std::ostream &operator<<(std::ostream &out, const Matrix<Numeric> &matrix) {
  std::vector<size_t> lengths(matrix.Cols());
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