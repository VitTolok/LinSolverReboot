#include "vector.hpp"
#include <gtest/gtest.h>
#include <random>

class RandomIntTests : public ::testing::Test {
public:
  std::uniform_int_distribution<int64_t> dist;
  std::default_random_engine rng;

  Matrix<int64_t> RandomMatrix(size_t rows = 0, size_t cols = 0) {
    if (rows == 0) {
      rows = dist(rng) % 100 + 1;
    }
    if (cols == 0) {
      cols = dist(rng) % 100 + 1;
    }
    Matrix<int64_t> matrix(rows, cols);
    for (size_t row = 0; row < rows; row++) {
      for (size_t col = 0; col < cols; col++) {
        matrix(row, col) = dist(rng) % 1'000'000'000;
      }
    }
    return matrix;
  }

  std::vector<int64_t> RandomVector(size_t size = 0) {
    if (size == 0) {
      size = dist(rng) % 100 + 1;
    }
    std::vector<int64_t> vector(size);
    for (size_t i = 0; i < size; i++) {
      vector[i] = dist(rng) % 1'000'000'000;
    }
    return vector;
  }
};

class ConstructorTests : public RandomIntTests {};

TEST_F(ConstructorTests, DefaultConstructorTest) {
  Vector<int64_t> vector;
  ASSERT_TRUE(vector.Empty());
  ASSERT_THROW(vector[0], VectorOutOfRange);
}

TEST_F(ConstructorTests, SizeConstructorTest) {
  {
    Vector<int64_t> vector(0);
    ASSERT_TRUE(vector.Empty());
    ASSERT_THROW(vector[0], VectorOutOfRange);
  }
  for (size_t i = 0; i < 64; i++) {
    size_t size = dist(rng) % 100 + 1;
    Vector<int64_t> vector(size);
    ASSERT_EQ(vector.Size(), size);
    ASSERT_FALSE(vector.Empty());
    for (size_t i = 0; i < size; i++) {
      ASSERT_NO_THROW(vector[i]);
      ASSERT_EQ(vector[i], 0);
    }
    ASSERT_THROW(vector[size], VectorOutOfRange);
    for (size_t i = 0; i < 100; i++) {
      ASSERT_THROW(vector[size + dist(rng) % 1'000'000'000], VectorOutOfRange);
    }
  }
}

TEST_F(ConstructorTests, MatrixConstructorTest) {
  for (size_t i = 0; i < 64; i++) {
    {
      auto matrix = RandomMatrix(0, 1);
      Vector<int64_t> vector;
      ASSERT_NO_THROW(vector = Vector<int64_t>(matrix));
      ASSERT_EQ(vector.Size(), matrix.Rows());
      ASSERT_FALSE(vector.Empty());
      for (size_t i = 0; i < vector.Size(); i++) {
        ASSERT_NO_THROW(vector[i]);
        ASSERT_EQ(vector[i], matrix(i, 0));
      }
      ASSERT_THROW(vector[vector.Size()], VectorOutOfRange);
    }
    {
      Matrix<int64_t> matrix(0, dist(rng) % 100);
      Vector<int64_t> vector;
      ASSERT_NO_THROW(vector = Vector<int64_t>(matrix));
      ASSERT_TRUE(vector.Empty());
      ASSERT_THROW(vector[0], VectorOutOfRange);
    }
    {
      Matrix<int64_t> matrix;
      while (matrix.Empty() || matrix.Cols() == 1) {
        matrix = RandomMatrix();
      }
      Vector<int64_t> vector;
      ASSERT_THROW(vector = Vector<int64_t>(matrix), InvalidMatrixToVector);
    }
  }
}

TEST_F(ConstructorTests, CopyConstructorTest) {
  for (size_t i = 0; i < 64; i++) {
    const auto source = RandomVector();
    {
      auto copy = source;
      Vector<int64_t> vector(copy);
      ASSERT_EQ(source.size(), copy.size());
      ASSERT_EQ(source.size(), vector.Size());
      for (size_t i = 0; i < source.size(); i++) {
        ASSERT_EQ(source[i], copy[i]);
        ASSERT_EQ(source[i], vector[i]);
        ASSERT_NE(&(copy[i]), &(vector[i]));
      }
    }
    {
      Vector<int64_t> vector(source);
      auto copy = vector;
      ASSERT_EQ(vector.Size(), copy.Size());
      for (size_t i = 0; i < vector.Size(); i++) {
        ASSERT_EQ(vector[i], copy[i]);
        ASSERT_NE(&(vector[i]), &(copy[i]));
      }
    }
  }
}

TEST_F(ConstructorTests, MoveConstructorTest) {
  for (size_t i = 0; i < 64; i++) {
    const auto source = RandomVector();
    {
      auto copy = source;
      Vector<int64_t> vector(std::move(copy));
      ASSERT_EQ(source.size(), vector.Size());
      ASSERT_TRUE(copy.empty());
      for (size_t i = 0; i < source.size(); i++) {
        ASSERT_EQ(source[i], vector[i]);
        ASSERT_NE(&(source[i]), &(vector[i]));
      }
    }
    {
      Vector<int64_t> vector(source);
      auto moved = std::move(vector);
      ASSERT_EQ(source.size(), moved.Size());
      ASSERT_TRUE(vector.Empty());
      for (size_t i = 0; i < vector.Size(); i++) {
        ASSERT_EQ(source[i], moved[i]);
        ASSERT_NE(&(source[i]), &(moved[i]));
      }
    }
  }
}

class StaticTest : public RandomIntTests {};

TEST_F(StaticTest, MatrixToManyTest) {
  for (size_t i = 0; i < 64; i++) {
    const auto matrix = RandomMatrix();
    const auto vectors = Vector<int64_t>::MatrixToMany(matrix);
    ASSERT_EQ(vectors.size(), matrix.Cols());
    if (!vectors.empty()) {
      for (size_t col = 0; col < matrix.Cols(); col++) {
        ASSERT_EQ(vectors[col].Size(), matrix.Rows());
        for (size_t row = 0; row < matrix.Rows(); row++) {
          ASSERT_EQ(vectors[col][row], matrix(row, col));
        }
      }
    }
  }
}

class ConversionTests : public RandomIntTests {};

TEST_F(ConversionTests, MatrixConversionTest) {
  for (size_t i = 0; i < 64; i++) {
    const auto vector = Vector<int64_t>(RandomVector());
    const auto matrix = Matrix<int64_t>(vector);
    ASSERT_EQ(matrix.Cols(), 1);
    ASSERT_EQ(matrix.Rows(), vector.Size());
    for (size_t i = 0; i < vector.Size(); i++) {
      ASSERT_EQ(matrix(i, 0), vector[i]);
    }
  }
}

class OperatorTests : public RandomIntTests {};

TEST_F(OperatorTests, AssignmentOperatorTest) {
  for (size_t i = 0; i < 64; i++) {
    const auto source = RandomVector();
    {
      Vector<int64_t> vector(source);
      Vector<int64_t> copy;
      copy = vector;
      ASSERT_EQ(copy.Size(), vector.Size());
      ASSERT_EQ(vector.Size(), source.size());
      for (size_t i = 0; i < vector.Size(); i++) {
        ASSERT_EQ(vector[i], copy[i]);
        ASSERT_EQ(vector[i], source[i]);
        ASSERT_NE(&(vector[i]), &(copy[i]));
      }
    }
    {
      Vector<int64_t> vector(source);
      Vector<int64_t> moved;
      moved = std::move(vector);
      ASSERT_EQ(moved.Size(), source.size());
      ASSERT_TRUE(vector.Empty());
      for (size_t i = 0; i < vector.Size(); i++) {
        ASSERT_EQ(source[i], moved[i]);
        ASSERT_NE(&(source[i]), &(moved[i]));
      }
    }
  }
}
