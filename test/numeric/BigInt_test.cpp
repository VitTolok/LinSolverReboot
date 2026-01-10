#include "BigInt.hpp"
#include <gtest/gtest.h>

#include <cstddef>
#include <random>

static_assert(
    []() constexpr -> bool {
      auto a = BigInt::FromString("-1.2e40");
      auto b = BigInt::FromString("3.4e39");
      return (a + b) == BigInt::FromString("-8.6e39") && (a + b).ToString() == "-8.6e39";
    }(),
    "constexpr string addition test");
static_assert(
    []() constexpr -> bool {
      auto a = BigInt::FromString("-1.2e40");
      auto b = BigInt::FromString("3.4e39");
      return (a - b) == BigInt::FromString("-1.54e40") && (a - b).ToString() == "-1.54e40";
    }(),
    "constexpr string addition test");
static_assert(
    []() constexpr -> bool {
      auto a = BigInt::FromString("-1.2e40");
      auto b = BigInt::FromString("3.4e39");
      return (a * b) == BigInt::FromString("-4.08e79") && (a * b).ToString() == "-4.08e79";
    }(),
    "constexpr string multiplication test");
static_assert(
    []() constexpr -> bool {
      auto a = BigInt::FromString("-1.2e40");
      auto b = BigInt::FromString("3.4e29");
      return (a / b) == BigInt::FromString("-35294117647") && (a / b).ToString() == "-3.52941e10";
    }(),
    "constexpr string multiplication test");

class IRandomIntTests : public ::testing::Test {
 public:
  std::uniform_int_distribution<int64_t> dist;
  std::default_random_engine rng;
};

class IConstructorTests : public IRandomIntTests {};

TEST_F(IConstructorTests, DefaultConstructorTest) {
  BigInt bigint;
  ASSERT_EQ(bigint, 0);
  ASSERT_LE(bigint, 0);
  ASSERT_LT(bigint, 1);
  ASSERT_GE(bigint, 0);
  ASSERT_NE(bigint, 1);
  ASSERT_GT(bigint, -1);
}

TEST_F(IConstructorTests, NumberConstructorTest) {
  for (size_t i = 0; i < 10000; i++) {
    int64_t number = dist(rng);
    BigInt bigint(number);
    ASSERT_EQ(bigint, number);
    ASSERT_EQ(static_cast<int64_t>(bigint), number);
  }
}

TEST_F(IConstructorTests, CopyConstructorTest) {
  for (size_t i = 0; i < 10000; i++) {
    int64_t number = dist(rng);
    const BigInt first(number);
    auto copy = first;
    ASSERT_EQ(first, number);
    ASSERT_EQ(first, copy);
  }
}
TEST_F(IConstructorTests, MoveConstructorTest) {
  for (size_t i = 0; i < 10000; i++) {
    int64_t number = dist(rng);
    BigInt first(number);
    auto copy = std::move(first);
    ASSERT_EQ(first, 0);
    ASSERT_EQ(number, copy);
  }
}

class IAssOperatorTests : public IRandomIntTests {};

TEST_F(IAssOperatorTests, CopyOperatorTest) {
  for (size_t i = 0; i < 10000; i++) {
    int64_t number = dist(rng);
    const BigInt first(number);
    BigInt copy;
    copy = first;
    ASSERT_EQ(first, number);
    ASSERT_EQ(first, copy);
  }
}
TEST_F(IAssOperatorTests, MoveOperatorTest) {
  for (size_t i = 0; i < 10000; i++) {
    int64_t number = dist(rng);
    BigInt first(number);
    BigInt copy;
    copy = std::move(first);
    ASSERT_EQ(first, 0);
    ASSERT_EQ(number, copy);
  }
}
TEST_F(IAssOperatorTests, NumberCopyOperatorTest) {
  for (size_t i = 0; i < 10000; ++i) {
    const auto number = dist(rng);
    BigInt bigint;
    bigint = number;
    ASSERT_EQ(bigint, number);
  }
}

class IStreamConsistencyTests : public ::testing::TestWithParam<std::string> {};

TEST_P(IStreamConsistencyTests, StreamConsistencyTest) {
  BigInt number;
  std::istringstream(GetParam()) >> number;
  ASSERT_EQ((std::ostringstream() << number).str(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(StreamConsistency, IStreamConsistencyTests,
                         ::testing::Values("123456", "-1.23456e127", "9.99999e999", "-1e6", "6.66e4321", "-4.3e534",
                                           "6.746e645", "-1.5876e1000", "73645", "-4090", "420", "-69", "3", "0"));

class IStreamStandartizationTests : public ::testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(IStreamStandartizationTests, StreamStandartizationTest) {
  BigInt number;
  ASSERT_NO_THROW(std::istringstream(GetParam().first) >> number);
  ASSERT_EQ((std::ostringstream() << number).str(), GetParam().second);
}

INSTANTIATE_TEST_SUITE_P(
    StreamStandartization, IStreamStandartizationTests,
    ::testing::Values(std::make_pair("1000000", "1e6"), std::make_pair("1234567890987654321", "1.23457e18"),
                      std::make_pair("-1.234567890e100", "-1.23457e100"), std::make_pair("1.0000e500", "1e500"),
                      std::make_pair("-7.000001e15", "-7e15"), std::make_pair("123.456e56", "1.23456e58"),
                      std::make_pair("1029384756.6574839201e73", "1.02938e82"), std::make_pair("1.234567e3", "1235"),
                      std::make_pair("-9.999999e304", "-1e305"), std::make_pair("-0", "0")));

class ISub64OperatorTests : public IRandomIntTests {};

TEST_F(ISub64OperatorTests, AddTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng) % std::numeric_limits<int64_t>::max();
    auto second = dist(rng) % (std::numeric_limits<int64_t>::max() - std::abs(first));
    BigInt number(first);
    ASSERT_EQ(BigInt(first) + BigInt(second), first + second);
    ASSERT_EQ(BigInt(first) + second, first + second);
    ASSERT_EQ(BigInt(first) + UBigInt(std::abs(second)), first + std::abs(second));
    ASSERT_EQ(number += second, first + second);
    ASSERT_EQ((number = first) += BigInt(second), first + second);
    ASSERT_EQ((number = first) += UBigInt(std::abs(second)), first + std::abs(second));
    ASSERT_EQ((number = std::min(first, second)) += number, std::min(first, second) * 2);
    auto shift = static_cast<size_t>(std::abs(dist(rng) % 256));
    ASSERT_EQ((BigInt(first) << shift) + (BigInt(second) << shift), BigInt(first + second) << shift);
  }
}

TEST_F(ISub64OperatorTests, SubTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng) % (1l << 62);
    auto second = dist(rng) % (1l << 62);
    BigInt number(first);
    ASSERT_EQ(BigInt(first) - BigInt(second), first - second);
    ASSERT_EQ(BigInt(first) - second, first - second);
    ASSERT_EQ(BigInt(first) - UBigInt(std::abs(second)), first - std::abs(second));
    ASSERT_EQ(number -= second, first - second);
    ASSERT_EQ((number = first) -= BigInt(second), first - second);
    ASSERT_EQ((number = first) -= UBigInt(std::abs(second)), first - std::abs(second));
    ASSERT_EQ((number = std::min(first, second)) -= number, 0);
    auto shift = static_cast<size_t>(std::abs(dist(rng) % 256));
    ASSERT_EQ((BigInt(first) << shift) - (BigInt(second) << shift), BigInt(first - second) << shift);
  }
}

TEST_F(ISub64OperatorTests, MulTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng) % std::numeric_limits<int64_t>::min();
    auto second = dist(rng) % (std::numeric_limits<int64_t>::max() / first);
    BigInt number(first);
    ASSERT_EQ(BigInt(first) * BigInt(second), first * second);
    ASSERT_EQ(BigInt(first) * second, first * second);
    ASSERT_EQ(BigInt(first) * UBigInt(std::abs(second)), first * std::abs(second));
    ASSERT_EQ(number *= second, first * second);
    ASSERT_EQ((number = first) *= BigInt(second), first * second);
    ASSERT_EQ((number = first) *= UBigInt(std::abs(second)), first * std::abs(second));
    ASSERT_EQ((number = std::min(first, second)) *= number, std::min(first, second) * std::min(first, second));
    auto shift = static_cast<size_t>(std::abs(dist(rng) % 256));
    ASSERT_EQ((BigInt(first) << shift) * (BigInt(second) << shift), BigInt(first * second) << (shift * 2));
  }
}

TEST_F(ISub64OperatorTests, DivTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng);
    auto second = dist(rng);
    BigInt number(first);
    ASSERT_EQ(BigInt(first) / BigInt(second), first / second);
    ASSERT_EQ(BigInt(first) / second, first / second);
    ASSERT_EQ(BigInt(first) / UBigInt(std::abs(second)), first / std::abs(second));
    ASSERT_EQ(number /= second, first / second);
    ASSERT_EQ((number = first) /= BigInt(second), first / second);
    ASSERT_EQ((number = first) /= UBigInt(std::abs(second)), first / std::abs(second));
    ASSERT_EQ((number = std::min(first, second)) /= number, 1);
    auto shift = static_cast<size_t>(std::abs(dist(rng) % 256));
    ASSERT_EQ((BigInt(first) << shift) / (BigInt(second) << shift), BigInt(first / second));
  }
}

TEST_F(ISub64OperatorTests, ModTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng);
    auto second = dist(rng);
    BigInt number(first);
    ASSERT_EQ(BigInt(first) % BigInt(second), first % second);
    ASSERT_EQ(BigInt(first) % second, first % second);
    ASSERT_EQ(BigInt(first) % UBigInt(std::abs(second)), first % std::abs(second));
    ASSERT_EQ(number %= second, first % second);
    ASSERT_EQ((number = first) %= BigInt(second), first % second);
    ASSERT_EQ((number = first) %= UBigInt(std::abs(second)), first % std::abs(second));
    ASSERT_EQ((number = std::min(first, second)) %= number, 0);
    auto shift = static_cast<size_t>(std::abs(dist(rng) % 256));
    ASSERT_EQ((BigInt(first) << shift) % (BigInt(second) << shift), BigInt(first % second) << shift);
  }
}

TEST_F(ISub64OperatorTests, LTTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng) % std::numeric_limits<int64_t>::min();
    auto second = dist(rng) % std::numeric_limits<int64_t>::min();
    ASSERT_FALSE(BigInt(first) < BigInt(first));
    ASSERT_EQ(BigInt(first) < BigInt(second), first < second);
    ASSERT_EQ(BigInt(first) < UBigInt(std::abs(second)), first < std::abs(second));
    ASSERT_EQ(BigInt(first) < second, first < second);
    ASSERT_EQ(UBigInt(std::abs(first)) < BigInt(second), std::abs(first) < second);
    ASSERT_EQ(first < BigInt(second), first < second);
    auto shift = static_cast<size_t>(std::abs(dist(rng) % 256));
    ASSERT_EQ((BigInt(first) << shift) < (BigInt(second) << shift), first < second);
  }
}
TEST_F(ISub64OperatorTests, GTTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng) % std::numeric_limits<int64_t>::min();
    auto second = dist(rng) % std::numeric_limits<int64_t>::min();
    ASSERT_FALSE(BigInt(first) > BigInt(first));
    ASSERT_EQ(BigInt(first) > BigInt(second), first > second);
    ASSERT_EQ(BigInt(first) > UBigInt(std::abs(second)), first > std::abs(second));
    ASSERT_EQ(BigInt(first) > second, first > second);
    ASSERT_EQ(UBigInt(std::abs(first)) > BigInt(second), std::abs(first) > second);
    ASSERT_EQ(first > BigInt(second), first > second);
    auto shift = static_cast<size_t>(std::abs(dist(rng) % 256));
    ASSERT_EQ((BigInt(first) << shift) > (BigInt(second) << shift), first > second);
  }
}
TEST_F(ISub64OperatorTests, LETest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng) % std::numeric_limits<int64_t>::min();
    auto second = dist(rng) % std::numeric_limits<int64_t>::min();
    ASSERT_TRUE(BigInt(first) <= BigInt(first));
    ASSERT_EQ(BigInt(first) <= BigInt(second), first <= second);
    ASSERT_EQ(BigInt(first) <= UBigInt(std::abs(second)), first <= std::abs(second));
    ASSERT_EQ(BigInt(first) <= second, first <= second);
    ASSERT_EQ(UBigInt(std::abs(first)) <= BigInt(second), std::abs(first) <= second);
    ASSERT_EQ(first <= BigInt(second), first <= second);
    auto shift = static_cast<size_t>(std::abs(dist(rng) % 256));
    ASSERT_EQ((BigInt(first) << shift) <= (BigInt(second) << shift), first <= second);
  }
}
TEST_F(ISub64OperatorTests, GETest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng) % std::numeric_limits<int64_t>::min();
    auto second = dist(rng) % std::numeric_limits<int64_t>::min();
    ASSERT_TRUE(BigInt(first) >= BigInt(first));
    ASSERT_EQ(BigInt(first) >= BigInt(second), first >= second);
    ASSERT_EQ(BigInt(first) >= UBigInt(std::abs(second)), first >= std::abs(second));
    ASSERT_EQ(BigInt(first) >= second, first >= second);
    ASSERT_EQ(UBigInt(std::abs(first)) >= BigInt(second), std::abs(first) >= second);
    ASSERT_EQ(first >= BigInt(second), first >= second);
    auto shift = static_cast<size_t>(std::abs(dist(rng) % 256));
    ASSERT_EQ((BigInt(first) << shift) >= (BigInt(second) << shift), first >= second);
  }
}
TEST_F(ISub64OperatorTests, EQTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng) % std::numeric_limits<int64_t>::min();
    auto second = dist(rng) % std::numeric_limits<int64_t>::min();
    ASSERT_TRUE(BigInt(first) == BigInt(first));
    ASSERT_EQ(BigInt(first) == BigInt(second), first == second);
    ASSERT_EQ(BigInt(first) == UBigInt(std::abs(second)), first == std::abs(second));
    ASSERT_EQ(BigInt(first) == second, first == second);
    ASSERT_EQ(UBigInt(std::abs(first)) == BigInt(second), std::abs(first) == second);
    ASSERT_EQ(first == BigInt(second), first == second);
    auto shift = static_cast<size_t>(std::abs(dist(rng) % 256));
    ASSERT_EQ((BigInt(first) << shift) == (BigInt(second) << shift), first == second);
  }
}
TEST_F(ISub64OperatorTests, NETest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng) % std::numeric_limits<int64_t>::min();
    auto second = dist(rng) % std::numeric_limits<int64_t>::min();
    ASSERT_FALSE(BigInt(first) != BigInt(first));
    ASSERT_EQ(BigInt(first) != BigInt(second), first != second);
    ASSERT_EQ(BigInt(first) != UBigInt(std::abs(second)), first != std::abs(second));
    ASSERT_EQ(BigInt(first) != second, first != second);
    ASSERT_EQ(UBigInt(std::abs(first)) != BigInt(second), std::abs(first) != second);
    ASSERT_EQ(first != BigInt(second), first != second);
    auto shift = static_cast<size_t>(std::abs(dist(rng) % 256));
    ASSERT_EQ((BigInt(first) << shift) != (BigInt(second) << shift), first != second);
  }
}

TEST_F(ISub64OperatorTests, LeftShiftTest) {
  for (size_t i = 0; i < 1000; ++i) {
    auto first = dist(rng) % (1ll << 48);
    auto second = static_cast<size_t>(dist(rng) % 15);
    BigInt number;
    ASSERT_EQ(BigInt(first) << 0, first);
    ASSERT_EQ((number = first) <<= 0, first);
    ASSERT_EQ(BigInt(first) << second, first << second);
    ASSERT_EQ((number = first) <<= second, first << second);
    first = dist(rng);
    second = static_cast<size_t>(dist(rng) % 1024);
    ASSERT_EQ(BigInt(first) << second, BigInt(first) * Power(UBigInt(2), second));
  }
}

TEST_F(ISub64OperatorTests, RightShiftTest) {
  for (size_t i = 0; i < 1000; ++i) {
    auto first = dist(rng);
    auto second = static_cast<size_t>(std::abs(dist(rng)) % 63);
    BigInt number;
    ASSERT_EQ(BigInt(first) >> 0, first);
    ASSERT_EQ((number = first) >>= 0, first);
    ASSERT_EQ(BigInt(first) >> second, first >> second);
    ASSERT_EQ((number = first) >>= second, first >> second);
  }
}
