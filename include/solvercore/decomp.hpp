#pragma once
#include "matrix.hpp"
#include <optional>

template <class Numeric> struct Decompose {
  Matrix<Numeric> simple;
  std::optional<Numeric> det;
  std::optional<Matrix<Numeric>> upper;
  std::optional<Matrix<Numeric>> lower;

  explicit Decompose(const Matrix<Numeric> &matrix) : simple(matrix) {
    if (simple.Rows() == simple.Cols()) {
      det = Numeric(1);
      upper = simple;
      lower = Matrix<Numeric>::Eye(Numeric(1), simple.Rows());
    } else {
      det.reset();
      upper.reset();
      lower.reset();
    }
    size_t cur_row = 0;
    for (size_t col = 0; col < simple.Cols(); col++) {
      if (cur_row >= simple.Rows()) {
        break;
      }
      if (simple(cur_row, col) == Numeric(0)) {
        upper.reset();
        lower.reset();
        size_t rep_row = cur_row;
        while (rep_row < simple.Rows() && simple(rep_row, col) == Numeric(0)) {
          rep_row++;
        }
        if (rep_row >= simple.Rows()) {
          if (det.has_value()) {
            det = Numeric(0);
          }
          continue;
        }
        simple.SwapRows(cur_row, rep_row);
        if (det.has_value()) {
          *det *= Numeric(-1);
        }
      }
      if (det.has_value()) {
        *det *= simple(cur_row, col);
      }
      simple.MultRow(cur_row, Numeric(1) / simple(cur_row, col));
      for (size_t row = 0; row < simple.Rows(); row++) {
        if (row == cur_row) {
          continue;
        }
        if (row > cur_row) {
          if (upper.has_value()) {
            (*upper).AddRow(cur_row, row,
                            -simple(row, col) / (*upper)(cur_row, col));
          }
          if (lower.has_value()) {
            (*lower)(row, col) = simple(row, col) / (*upper)(cur_row, col);
          }
        }
        simple.AddRow(cur_row, row, -simple(row, col));
      }
      cur_row++;
    }
  }
};