#pragma once
#include "matrix.hpp"

class InvalidMatrixToVector : public MachineError {
private:
  std::string message_ = "Matrix has to have one column";
};

class InvalidVectorOperation : public MachineError {
private:
  std::string message_ = "Vectors have to be the same length";
};

class VectorOutOfRange : public MachineError {
public:
  VectorOutOfRange(size_t range, size_t reference) {
    message_ =
        (std::ostringstream() << "Vector has " << range << " coords but coord "
                              << reference << " was referenced")
            .str();
  }

private:
  std::string message_;
};

template <class Numeric> class Vector {
public:
  Vector() : coords_() {}
  explicit Vector(size_t size) : coords_(size) {}

  explicit Vector(const Matrix<Numeric> &matrix) {
    if (matrix.Rows() == 0) {
      coords_.resize(0);
      return;
    }
    if (matrix.Cols() != 1) {
      throw InvalidMatrixToVector();
    }
    coords_.resize(matrix.Rows());
    for (size_t i = 0; i < coords_.size(); i++) {
      coords_[i] = matrix(i, 0);
    }
  }

  Vector(const Vector<Numeric> &other) : coords_(other.coords_) {}
  Vector(Vector<Numeric> &&other) : coords_(std::move(other.coords_)) {}

  explicit Vector(const std::vector<Numeric> &source) : coords_(source) {}
  explicit Vector(std::vector<Numeric> &&source) : coords_(std::move(source)) {}

  Vector &operator=(const Vector<Numeric> &other) {
    coords_ = other.coords_;
    return *this;
  }
  Vector &operator=(Vector<Numeric> &&other) {
    coords_ = std::move(other.coords_);
    return *this;
  }

  static std::vector<Vector<Numeric>>
  MatrixToMany(const Matrix<Numeric> &matrix) {
    std::vector<Vector<Numeric>> vectors(matrix.Cols(),
                                         Vector<Numeric>(matrix.Rows()));
    for (size_t row = 0; row < matrix.Rows(); row++) {
      for (size_t col = 0; col < matrix.Cols(); col++) {
        vectors[col].coords_[row] = matrix(row, col);
      }
    }
    return vectors;
  }

  explicit operator Matrix<Numeric>() const {
    Matrix<Numeric> matrix(coords_.size(), 1);
    for (size_t i = 0; i < coords_.size(); i++) {
      matrix(i, 0) = coords_[i];
    }
    return matrix;
  }

  Numeric &operator[](size_t index) {
    if (index >= coords_.size()) {
      throw VectorOutOfRange(coords_.size(), index);
    }
    return coords_[index];
  }
  const Numeric &operator[](size_t index) const {
    if (index >= coords_.size()) {
      throw VectorOutOfRange(coords_.size(), index);
    }
    return coords_[index];
  }

  size_t Size() const noexcept { return coords_.size(); }

  Vector &operator+=(const Vector &other) {
    if (coords_.size() != other.coords_.size()) {
      throw InvalidVectorOperation();
    }
    for (size_t i = 0; i < coords_.size(); i++) {
      coords_[i] += other.coords_[i];
    }
    return *this;
  }
  Vector &operator-=(const Vector &other) {
    if (coords_.size() != other.coords_.size()) {
      throw InvalidVectorOperation();
    }
    for (size_t i = 0; i < coords_.size(); i++) {
      coords_[i] -= other.coords_[i];
    }
    return *this;
  }
  Vector &operator*=(const Numeric &mult) {
    for (auto &coord : coords_) {
      coord *= mult;
    }
    return *this;
  }
  Vector &operator/=(const Numeric &div) {
    for (auto &coord : coords_) {
      coord /= div;
    }
    return *this;
  }

  Vector<Numeric> operator+(const Vector &other) const {
    auto copy = *this;
    return copy += other;
  }
  Vector<Numeric> operator-(const Vector &other) const {
    auto copy = *this;
    return copy -= other;
  }
  Vector<Numeric> operator*(const Numeric &mult) const {
    auto copy = *this;
    return copy *= mult;
  }
  Vector<Numeric> operator/(const Numeric &div) const {
    auto copy = *this;
    return copy /= div;
  }

  bool operator==(const Vector &other) const {
    if (coords_.size() != other.coords_.size()) {
      return false;
    }
    for (size_t i = 0; i < coords_.size(); i++) {
      if (coords_[i] != other.coords_[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const Vector &other) const { return !(*this == other); }

  bool Empty() const noexcept { return coords_.empty(); }

private:
  std::vector<Numeric> coords_;
};