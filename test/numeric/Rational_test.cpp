#include "Rational.hpp"
#include <gtest/gtest.h>
#include <utility>

consteval void ConstAssert(bool expr) {
  if (!expr) {
    std::unreachable();
  }
}
consteval void ConstAssertEQ(const auto &first, const auto &second) {
  ConstAssert(first == second);
}
consteval void ConstAssertLT(const auto &first, const auto &second) {
  ConstAssert(first < second);
}

static_assert([]() constexpr -> bool {
  auto rat = Rational::FromString("1e-40");
  ConstAssertEQ(rat.Den(), UBigInt::FromString("1e40"));
  ConstAssertEQ(rat.Num(), 1);
  ConstAssertEQ(rat.ToString(), "1e-40");
  return true;
}());
static_assert([]() constexpr -> bool {
  auto a = Rational::FromString("6e40");
  auto b = Rational::FromString("2e50");
  ConstAssertLT(a, b);
  ConstAssertEQ(a + b, Rational::FromString("2.0000000006e50"));
  ConstAssertEQ((a + b).ToString(), "2e50");
  ConstAssertEQ(a - b, Rational::FromString("-1.9999999994e50"));
  ConstAssertEQ((a - b).ToString(), "-2e50");
  ConstAssertEQ(a * b, Rational::FromString("1.2e91"));
  ConstAssertEQ((a * b).ToString(), "1.2e91");
  ConstAssertEQ(a / b, Rational::FromString("3e-10"));
  ConstAssertEQ((a / b).ToString(), "3e-10");
  return true;
}());

using NumArr = std::array<std::pair<int64_t, uint64_t>, 6>;
using P = std::pair<int64_t, uint64_t>;

class IRationalParamTest : public ::testing::TestWithParam<NumArr> {
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

INSTANTIATE_TEST_SUITE_P(ParamTest, IRationalParamTest,
                         ::testing::Values(NumArr{P{1, 2}, P{1, 3}, P{5, 6}, P{1, 6}, P{1, 6}, P{3, 2}},
                                           NumArr{P{1, 4}, P{1, 2}, P{3, 4}, P{-1, 4}, P{1, 8}, P{1, 2}},
                                           NumArr{P{-5, 1}, P{3, 4}, P{-17, 4}, P{-23, 4}, P{-15, 4}, P{-20, 3}},
                                           NumArr{P{5, 6}, P{3, 11}, P{73, 66}, P{37, 66}, P{5, 22}, P{55, 18}},
                                           NumArr{P{287, 1}, P{3, 11}, P{3160, 11}, P{3154, 11}, P{861, 11},
                                                  P{3157, 3}}));

TEST_P(IRationalParamTest, ParamTestOper) {
  ASSERT_EQ(first + second, sum);
  ASSERT_EQ(first - second, sub);
  ASSERT_EQ(first * second, mul);
  ASSERT_EQ(first / second, div);
  Rational copy = first;
  ASSERT_EQ(copy += second, sum);
  copy = first;
  ASSERT_EQ(copy -= second, sub);
  copy = first;
  ASSERT_EQ(copy *= second, mul);
  copy = first;
  ASSERT_EQ(copy /= second, div);
}

TEST_P(IRationalParamTest, ParamTestEq) {
  ASSERT_EQ(first, Rational(first.Num() * second.Den(), first.Den() * second.Den()));
  ASSERT_EQ(first.Num(), Rational(first.Num() * second.Den(), first.Den() * second.Den()).Num());
  ASSERT_EQ(first.Den(), Rational(first.Num() * second.Den(), first.Den() * second.Den()).Den());
}

class IRationalStreamTest : public ::testing::TestWithParam<std::tuple<std::string, int64_t, uint64_t>> {
 public:
  std::string str;
  int64_t num;
  uint64_t den;
  void SetUp() override {
    auto [tstr, tnum, tden] = GetParam();
    str = std::move(tstr);
    num = tnum;
    den = tden;
  }
};

INSTANTIATE_TEST_SUITE_P(ParamStreamTests, IRationalStreamTest,
                         ::testing::Values(std::make_tuple("1/2", 1, 2), std::make_tuple("55", 55, 1),
                                           std::make_tuple("-5/13", -5, 13)));

TEST_P(IRationalStreamTest, ParamStreamTests) {
  Rational number;
  std::istringstream(str) >> number;
  ASSERT_EQ(num, number.Num());
  if (den == 0) {
    ASSERT_EQ(1, number.Den());
  } else {
    ASSERT_EQ(den, number.Den());
  }
  ASSERT_EQ((std::ostringstream() << number).str(), str);
  ASSERT_EQ(Rational(num, den), Rational(num, static_cast<int64_t>(den)));
}