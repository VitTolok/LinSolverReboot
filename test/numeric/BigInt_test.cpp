#include "BigInt.hpp"
#include <gtest/gtest.h>

#include <random>

class RandomIntTests : public ::testing::Test {
public:
  std::uniform_int_distribution<int64_t> dist;
  std::default_random_engine rng;
};

class ConstructorTests : public RandomIntTests {};

TEST_F(ConstructorTests, DefaultConstructorTest) {
  BigInt bigint;
  ASSERT_EQ(bigint, 0);
  ASSERT_LE(bigint, 0);
  ASSERT_LT(bigint, 1);
  ASSERT_GE(bigint, 0);
  ASSERT_NE(bigint, 1);
  ASSERT_GT(bigint, -1);
}

TEST_F(ConstructorTests, NumberConstructorTest) {
  for (size_t i = 0; i < 10000; i++) {
    int64_t number = dist(rng);
    BigInt bigint(number);
    ASSERT_EQ(bigint, number);
    ASSERT_EQ(static_cast<int64_t>(bigint), number);
  }
}

TEST_F(ConstructorTests, CopyConstructorTest) {
  for (size_t i = 0; i < 10000; i++) {
    int64_t number = dist(rng);
    const BigInt first(number);
    auto copy = first;
    ASSERT_EQ(first, number);
    ASSERT_EQ(first, copy);
  }
}
TEST_F(ConstructorTests, MoveConstructorTest) {
  for (size_t i = 0; i < 10000; i++) {
    int64_t number = dist(rng);
    BigInt first(number);
    auto copy = std::move(first);
    ASSERT_EQ(first, 0);
    ASSERT_EQ(number, copy);
  }
}

class AssOperatorTests : public RandomIntTests {};

TEST_F(AssOperatorTests, CopyOperatorTest) {
  for (size_t i = 0; i < 10000; i++) {
    int64_t number = dist(rng);
    const BigInt first(number);
    BigInt copy;
    copy = first;
    ASSERT_EQ(first, number);
    ASSERT_EQ(first, copy);
  }
}
TEST_F(AssOperatorTests, MoveOperatorTest) {
  for (size_t i = 0; i < 10000; i++) {
    int64_t number = dist(rng);
    BigInt first(number);
    BigInt copy;
    copy = std::move(first);
    ASSERT_EQ(first, 0);
    ASSERT_EQ(number, copy);
  }
}
TEST_F(AssOperatorTests, NumberCopyOperatorTest) {
  for (size_t i = 0; i < 10000; ++i) {
    const auto number = dist(rng);
    BigInt bigint;
    bigint = number;
    ASSERT_EQ(bigint, number);
  }
}

class StreamConsistencyTests : public ::testing::TestWithParam<std::string> {};

TEST_P(StreamConsistencyTests, StreamConsistencyTest) {
  BigInt number;
  std::istringstream(GetParam()) >> number;
  ASSERT_EQ((std::ostringstream() << number).str(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(StreamConsistency, StreamConsistencyTests,
                         ::testing::Values("123456", "-1.23456e127",
                                           "9.99999e999", "-1e6", "6.66e4321",
                                           "-4.3e534", "6.746e645",
                                           "-1.5876e1000", "73645", "-4090",
                                           "420", "-69", "3", "0"));

class StreamStandartizationTests
    : public ::testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(StreamStandartizationTests, StreamStandartizationTest) {
  BigInt number;
  ASSERT_NO_THROW(std::istringstream(GetParam().first) >> number);
  ASSERT_EQ((std::ostringstream() << number).str(), GetParam().second);
}

INSTANTIATE_TEST_SUITE_P(
    StreamStandartization, StreamStandartizationTests,
    ::testing::Values(std::make_pair("1000000", "1e6"),
                      std::make_pair("1234567890987654321", "1.23457e18"),
                      std::make_pair("-1.234567890e100", "-1.23457e100"),
                      std::make_pair("1.0000e500", "1e500"),
                      std::make_pair("-7.000001e15", "-7e15"),
                      std::make_pair("123.456e56", "1.23456e58"),
                      std::make_pair("1029384756.6574839201e73", "1.02938e82"),
                      std::make_pair("1.234567e3", "1235"),
                      std::make_pair("-9.999999e304", "-1e305"),
                      std::make_pair("-0", "0")));

class Sub64OperatorTests : public RandomIntTests {};

TEST_F(Sub64OperatorTests, AddTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng) % std::numeric_limits<int64_t>::max();
    auto second =
        dist(rng) % (std::numeric_limits<int64_t>::max() - std::abs(first)) -
        (1l << 62);
    BigInt number(first);
    ASSERT_EQ(BigInt(first) + BigInt(second), first + second);
    ASSERT_EQ(BigInt(first) + second, first + second);
    ASSERT_EQ(number += second, first + second);
    number = first;
    ASSERT_EQ(number += BigInt(second), first + second);
    ASSERT_EQ((number = std::min(first, second)) += number,
              std::min(first, second) * 2);
    int64_t shift = dist(rng) % 256;
    ASSERT_EQ((BigInt(first) << std::abs(shift)) +
                  (BigInt(second) << std::abs(shift)),
              BigInt(first + second) << std::abs(shift));
  }
}
