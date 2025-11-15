#include "decomp.hpp"
#include <gtest/gtest.h>

class ExampleTests : public ::testing::TestWithParam<
                         std::tuple<std::string, std::string, std::string,
                                    std::string, std::optional<double>>> {
public:
  Matrix<double> source;
  Decompose<double> expected;

  void SetUp() override {
    auto [mastrix, simpling, lowstr, upstr, det] = GetParam();
    std::istringstream(mastrix) >> source;
    std::istringstream(simpling) >> expected.simple;
    if (lowstr != "") {
      expected.lower = Matrix<double>();
      expected.upper = Matrix<double>();
      std::istringstream(lowstr) >> *expected.lower;
      std::istringstream(upstr) >> *expected.upper;
    } else {
      expected.lower.reset();
      expected.upper.reset();
    }
    expected.det = det;
  }
};

TEST_P(ExampleTests, ExampleDecompTests) {
  const auto actual = Decompose(source);
  ASSERT_EQ(expected.det.has_value(), actual.det.has_value());
  ASSERT_EQ(expected.lower.has_value(), actual.lower.has_value());
  ASSERT_EQ(expected.upper.has_value(), actual.upper.has_value());
  if (expected.det.has_value()) {
    ASSERT_EQ(*actual.det, *expected.det);
  }
  for (size_t row = 0; row < source.Rows(); row++) {
    for (size_t col = 0; col < source.Cols(); col++) {
      ASSERT_FLOAT_EQ(expected.simple(row, col), actual.simple(row, col));
      if (expected.lower.has_value()) {
        ASSERT_FLOAT_EQ((*expected.lower)(row, col), (*actual.lower)(row, col));
        ASSERT_FLOAT_EQ((*expected.upper)(row, col), (*actual.upper)(row, col));
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
    ExampleDecompTests, ExampleTests,
    ::testing::Values(
        std::make_tuple("/ 1 /", "/ 1 /", "/ 1 /", "/ 1 /", 1),
        std::make_tuple("/ 1 2 \\ \\ 3 4 /", "/ 1  0 \\ \\ 0 1 /",
                        "/ 1  0 \\ \\ 3 1 /", "/ 1 2 \\ \\ 0 -2 /", -2),
        std::make_tuple("/ 1 2 2 \\ | 4 4 2 | \\ 4 6 4 /",
                        "/ 1 0 0 \\ | 0 1 0 | \\ 0 0 1 /",
                        "/ 1 0 0 \\ | 4 1 0 | \\ 4 0.5 1 /",
                        "/ 1 2 2 \\ | 0 -4 -6 | \\ 0 0 -1 /", 4),
        std::make_tuple(
            "/ 2 8 4 1 \\ | 1 2 3 3 | | 1 2 6 2 | \\ 1 3 4 2 /",
            "/ 1 0 0 0 \\ | 0 1 0 0 | | 0 0 1 0 | \\ 0 0 0 1 /",
            "/ 1 0 0 0 \\ | 0.5 1 0 0 | | 0.5 1 1 0 | \\ 0.5 0.5 0.5 1 /",
            "/ 2 8 4 1 \\ | 0 -2 1 2.5 | | 0 0 3 -1 | \\ 0 0 0 0.75 /", -9),
        std::make_tuple("/ 2 8 4 1 \\ | 1 4 3 3 | | 1 2 6 2 | \\ 1 3 4 2 /",
                        "/ 1 0 0 0 \\ | 0 1 0 0 | | 0 0 1 0 | \\ 0 0 0 1 /", "",
                        "", 3),
        std::make_tuple("/ 1 2 3 4 \\ | 1 2 4 8 | | 1 0 1 0 | \\ 3 4 8 12 /",
                        "/ 1 0 0 -4 \\ | 0 1 0 -2 | | 0 0 1 4 | \\ 0 0 0 0 /",
                        "", "", 0),
        std::make_tuple("/ 1 2 3 4 \\ | 1 2 4 8 | \\ 1 0 1 0 /",
                        "/ 1 0 0 -4 \\ | 0 1 0 -2 | \\ 0 0 1 4 /", "", "",
                        std::optional<double>()),
        std::make_tuple("/ 1 2 3 \\ | 1 2 4 | | 1 0 1 | \\ 4 5 1 /",
                        "/ 1 0 0 \\ | 0 1 0 | | 0 0 1 | \\ 0 0 0 /", "", "",
                        std::optional<double>())));
