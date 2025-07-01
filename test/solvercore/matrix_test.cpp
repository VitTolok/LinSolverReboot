#include "matrix.hpp"
#include <gtest/gtest.h>
#include <random>

class RandomMatrixCheck : public ::testing::Test {
public:
  std::uniform_int_distribution<size_t> dist;
  std::default_random_engine rng;
  std::vector<std::vector<size_t>> source;
  Matrix<size_t> matrix;
  size_t rows;
  size_t cols;

  void SetUp() override {
    rows = dist(rng) % 64;
    cols = dist(rng) % 64;
    source = std::vector<std::vector<size_t>>(rows);
    for (auto &row : source) {
      row = std::vector<size_t>(cols);
      for (auto &number : row) {
        number = dist(rng) % 1'000'000'000;
      }
    }
    matrix = Matrix(source);
  }
};

TEST_F(RandomMatrixCheck, ContentConfirmation) {
  for (size_t i = 0; i < 64; i++) {
    SetUp();
    ASSERT_EQ(matrix.Rows(), rows);
    ASSERT_EQ(matrix.Cols(), cols);
    for (size_t row = 0; row < rows; row++) {
      for (size_t col = 0; col < cols; col++) {
        ASSERT_EQ(matrix(row, col), source[row][col]);
      }
    }
  }
}

TEST_F(RandomMatrixCheck, InvalidSourceCheck) {
  for (size_t i = 0; i < 64; i++) {
    SetUp();
    while (rows <= 1) {
      SetUp();
    }
    source[dist(rng) % rows].resize((dist(rng) % 2) * (cols + 1) +
                                    (dist(rng) % cols));
    ASSERT_THROW(matrix = Matrix(source), InvalidMatrixBase);
  }
}

TEST_F(RandomMatrixCheck, RowSwapTest) {
  for (size_t i = 0; i < 64; i++) {
    SetUp();
    while (rows == 0) {
      SetUp();
    }
    size_t row1 = dist(rng) % rows;
    size_t row2 = dist(rng) % rows;
    ASSERT_NO_THROW(matrix.SwapRows(row1, row2));
    for (size_t row = 0; row < rows; row++) {
      for (size_t col = 0; col < cols; col++) {
        if (row == row1) {
          ASSERT_EQ(matrix(row, col), source[row2][col]);
          continue;
        }
        if (row == row2) {
          ASSERT_EQ(matrix(row, col), source[row1][col]);
          continue;
        }
        ASSERT_EQ(matrix(row, col), source[row][col]);
      }
    }
  }
}

TEST_F(RandomMatrixCheck, ColSwapTest) {
  for (size_t i = 0; i < 64; i++) {
    SetUp();
    while (rows == 0 || cols == 0) {
      SetUp();
    }
    size_t col1 = dist(rng) % cols;
    size_t col2 = dist(rng) % cols;
    ASSERT_NO_THROW(matrix.SwapCols(col1, col2));
    for (size_t row = 0; row < rows; row++) {
      for (size_t col = 0; col < cols; col++) {
        if (col == col1) {
          ASSERT_EQ(matrix(row, col), source[row][col2]);
          continue;
        }
        if (col == col2) {
          ASSERT_EQ(matrix(row, col), source[row][col1]);
          continue;
        }
        ASSERT_EQ(matrix(row, col), source[row][col]);
      }
    }
  }
}

TEST_F(RandomMatrixCheck, RowAddTest) {
  for (size_t i = 0; i < 64; i++) {
    SetUp();
    while (rows == 0) {
      SetUp();
    }
    size_t src = dist(rng) % rows;
    size_t tgt = dist(rng) % rows;
    size_t mult = dist(rng) % 1'000'000'000;
    ASSERT_NO_THROW(matrix.AddRow(src, tgt, mult));
    for (size_t row = 0; row < rows; row++) {
      for (size_t col = 0; col < cols; col++) {
        if (row == tgt) {
          ASSERT_EQ(matrix(row, col),
                    source[src][col] * mult + source[row][col]);
          continue;
        }
        ASSERT_EQ(matrix(row, col), source[row][col]);
      }
    }
  }
}
TEST_F(RandomMatrixCheck, ColAddTest) {
  for (size_t i = 0; i < 64; i++) {
    SetUp();
    while (rows == 0 || cols == 0) {
      SetUp();
    }
    size_t src = dist(rng) % cols;
    size_t tgt = dist(rng) % cols;
    size_t mult = dist(rng) % 1'000'000'000;
    ASSERT_NO_THROW(matrix.AddCol(src, tgt, mult));
    for (size_t row = 0; row < rows; row++) {
      for (size_t col = 0; col < cols; col++) {
        if (col == tgt) {
          ASSERT_EQ(matrix(row, col),
                    source[row][src] * mult + source[row][col]);
          continue;
        }
        ASSERT_EQ(matrix(row, col), source[row][col]);
      }
    }
  }
}

TEST_F(RandomMatrixCheck, MultRowTest) {
  for (size_t i = 0; i < 64; i++) {
    SetUp();
    while (rows == 0) {
      SetUp();
    }
    size_t tgt = dist(rng) % rows;
    size_t mult = dist(rng) % 1'000'000'000;
    ASSERT_NO_THROW(matrix.MultRow(tgt, mult));
    for (size_t row = 0; row < rows; row++) {
      for (size_t col = 0; col < cols; col++) {
        if (row == tgt) {
          ASSERT_EQ(matrix(row, col), source[row][col] * mult);
          continue;
        }
        ASSERT_EQ(matrix(row, col), source[row][col]);
      }
    }
  }
}

TEST_F(RandomMatrixCheck, MultColTest) {
  for (size_t i = 0; i < 64; i++) {
    SetUp();
    while (rows == 0 || cols == 0) {
      SetUp();
    }
    size_t tgt = dist(rng) % cols;
    size_t mult = dist(rng) % 1'000'000'000;
    ASSERT_NO_THROW(matrix.MultCol(tgt, mult));
    for (size_t row = 0; row < rows; row++) {
      for (size_t col = 0; col < cols; col++) {
        if (col == tgt) {
          ASSERT_EQ(matrix(row, col), source[row][col] * mult);
          continue;
        }
        ASSERT_EQ(matrix(row, col), source[row][col]);
      }
    }
  }
}

TEST_F(RandomMatrixCheck, IdentityTest) {
  for (size_t i = 0; i < 64; i++) {
    size_t size = dist(rng) % 64;
    int64_t value = static_cast<int64_t>(dist(rng));
    auto identity = Matrix<int64_t>::Eye(value, size);
    ASSERT_EQ(identity.Rows(), size);
    ASSERT_EQ(identity.Cols(), size);
    for (size_t row = 0; row < size; row++) {
      for (size_t col = 0; col < size; col++) {
        if (row == col) {
          ASSERT_EQ(identity(row, col), value);
          continue;
        }
        ASSERT_EQ(identity(row, col), 0);
      }
    }
  }
}

TEST_F(RandomMatrixCheck, OutOfRangeCheck) {
  for (size_t i = 0; i < 64; i++) {
    do {
      SetUp();
    } while (rows == 0 || cols == 0);
    for (size_t row = 0; row <= rows; row++) {
      ASSERT_THROW(matrix(row, cols), MatrixOutOfRange);
      ASSERT_THROW(matrix.SwapRows(row, rows), MatrixOutOfRange);
      ASSERT_THROW(matrix.SwapRows(rows, row), MatrixOutOfRange);
      ASSERT_THROW(matrix.AddRow(rows, row, 0), MatrixOutOfRange);
      ASSERT_THROW(matrix.AddRow(row, rows, 0), MatrixOutOfRange);
    }
    ASSERT_THROW(matrix.MultRow(rows, 0), MatrixOutOfRange);
    for (size_t col = 0; col <= cols; col++) {
      ASSERT_THROW(matrix(rows, col), MatrixOutOfRange);
      ASSERT_THROW(matrix.SwapCols(col, cols), MatrixOutOfRange);
      ASSERT_THROW(matrix.SwapCols(cols, col), MatrixOutOfRange);
      ASSERT_THROW(matrix.AddCol(col, cols, 0), MatrixOutOfRange);
      ASSERT_THROW(matrix.AddCol(cols, col, 0), MatrixOutOfRange);
    }
    ASSERT_THROW(matrix.MultCol(cols, 0), MatrixOutOfRange);
    for (size_t i = 0; i < 64; i++) {
      size_t row = rows + (dist(rng) % 1'000'000'000);
      size_t col = cols + (dist(rng) % 1'000'000'000);
      ASSERT_THROW(matrix(row, col), MatrixOutOfRange);
      ASSERT_THROW(matrix.SwapRows(dist(rng) % rows, row), MatrixOutOfRange);
      ASSERT_THROW(matrix.SwapRows(row, dist(rng) % rows), MatrixOutOfRange);
      ASSERT_THROW(matrix.SwapRows(row, row + col), MatrixOutOfRange);
      ASSERT_THROW(matrix.SwapCols(dist(rng) % cols, col), MatrixOutOfRange);
      ASSERT_THROW(matrix.SwapCols(col, dist(rng) % cols), MatrixOutOfRange);
      ASSERT_THROW(matrix.SwapCols(col, row + col), MatrixOutOfRange);
      ASSERT_THROW(matrix.MultRow(row, 0), MatrixOutOfRange);
      ASSERT_THROW(matrix.MultCol(col, 0), MatrixOutOfRange);
      ASSERT_THROW(matrix.AddRow(0, row, 0), MatrixOutOfRange);
      ASSERT_THROW(matrix.AddRow(row, 0, 0), MatrixOutOfRange);
      ASSERT_THROW(matrix.AddCol(0, col, 0), MatrixOutOfRange);
      ASSERT_THROW(matrix.AddCol(col, 0, 0), MatrixOutOfRange);
    }
  }
}

TEST_F(RandomMatrixCheck, OutputCheck) {
  for (size_t i = 0; i < 64; i++) {
    SetUp();
    std::string output = (std::ostringstream() << matrix).str();
    std::istringstream in(output);
    if (rows == 0) {
      ASSERT_EQ(output, "");
    }
    for (size_t row = 0; row < rows; row++) {
      std::string edge;
      in >> edge;
      ASSERT_EQ(edge, "|");
      for (size_t col = 0; col < cols; col++) {
        size_t value = 0;
        in >> value;
        ASSERT_EQ(value, source[row][col]);
      }
      in >> edge;
      ASSERT_EQ(edge, "|");
    }
  }
}