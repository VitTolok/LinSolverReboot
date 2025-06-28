#include <iostream>
#include <sstream>
#include <vector>

template <class Numeric> class Matrix {
  Matrix(size_t rows, size_t cols) {
    contents_ = std::vector(std::vector(Numeric(0), cols), rows);
    rows_ = contents_.size();
    cols_ = contents_[0].size();
  }

  Matrix(std::vector<std::vector<Numeric>> &&cont)
      : contents_(std::forward(cont)) {
    rows_ = contents_.size();
    cols_ = contents_[0].size();
  }

  Matrix(Matrix<Numeric> &&source) {
    contents_ = std::move(source.contents_);
    rows_ = contents_.size();
    cols_ = contents_[0].size();
  }

  Matrix(const Matrix<Numeric> &source) {
    contents_ = source.contents_;
    rows_ = contents_.size();
    cols_ = contents_[0].size();
  }

  void SwapRows(size_t row1, size_t row2) {
    contents_[row1].swap(contents_[row2]);
  }

  void AddRow(size_t source, size_t target, const Numeric &mult) {
    for (size_t col = 0; col < cols_; col++) {
      contents_[target] += contents_[source] * mult;
    }
  }

  void MultRow(size_t row, Numeric mult) {
    for (size_t col = 0; col < cols_; col++) {
      contents_[row][col] *= mult;
    }
  }

  void SwapCols(size_t col1, size_t col2) {
    for (size_t row = 0; row < rows_; row++) {
      std::swap(contents_[row][col1], contents_[row][col2]);
    }
  }

  void AddCol(size_t source, size_t target, const Numeric &mult) {
    for (size_t row = 0; row < rows_; row++) {
      contents_[row][target] += contents_[row][source] * mult;
    }
  }

  void MultCol(size_t col, const Numeric &mult) {
    for (size_t row = 0; row < rows_; row++) {
      contents_[row][col] *= mult;
    }
  }

  Numeric &operator()(size_t row, size_t col) { return contents_[row][col]; }
  const Numeric &operator()(size_t row, size_t col) const {
    return contents_[row][col];
  }

  size_t Rows() const { return rows_; }
  size_t Cols() const { return cols_; }

private:
  std::vector<std::vector<Numeric>> contents_;
  size_t rows_;
  size_t cols_;
};

template <class Numeric>
std::ostream &operator<<(std::ostream &out, const Matrix<Numeric> &matrix) {
  size_t lengths[matrix.Cols()]{};
  for (size_t col = 0; col < matrix.Cols(); col++) {
    for (size_t row = 0; row < matrix.Rows(); row++) {
      lengths[col] =
          std::max(lengths[col],
                   (std::ostringstream() << matrix(row, col)).str().size());
    }
  }
  for (size_t row = 0; row < matrix.Rows(); ++row) {
    std::cout << "| ";
    for (size_t col = 0; col < matrix.Cols(); ++col) {
      std::cout
          << matrix(row, col)
          << std::string(
                 lengths[col] -
                     (std::ostringstream() << matrix(row, col)).str().size() +
                     1,
                 ' ');
    }
    std::cout << "|\n";
  }
}