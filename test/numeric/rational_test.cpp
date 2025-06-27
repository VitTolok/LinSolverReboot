#include "rational.hpp"
#include <gtest/gtest.h>

using NumArr = std::array<std::pair<int64_t, uint64_t>, 6>;
using P = std::pair<int64_t, uint64_t>;

class RationalOperTest : public ::testing::TestWithParam<NumArr> {
public:
  Rational first, second, sum, sub, mul, div;
  void SetUp() override {
    std::array<std::pair<int64_t, uint64_t>, 6> temp = GetParam();
    first = Rational(temp[0].first, temp[0].second);
    second = Rational(temp[1].first, temp[1].second);
    sum = Rational(temp[2].first, temp[2].second);
    sub = Rational(temp[3].first, temp[3].second);
    mul = Rational(temp[4].first, temp[4].second);
    div = Rational(temp[5].first, temp[5].second);
  }
};

INSTANTIATE_TEST_SUITE_P(ParamTest, RationalOperTest,
                         ::testing::Values(NumArr{P{1, 2}, P{1, 3}, P{5, 6},
                                                  P{1, 6}, P{1, 6}, P{3, 2}}));

TEST_P(RationalOperTest, ParamTestOper) {
  ASSERT_EQ(first + second, sum);
  ASSERT_EQ(first - second, sub);
  ASSERT_EQ(first * second, mul);
  ASSERT_EQ(first / second, div);
}

TEST_P(RationalOperTest, ParamTestEq) {
  ASSERT_EQ(first,
            Rational(first.Num() * second.Den(), first.Den() * second.Den()));
}