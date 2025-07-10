#include "UBigInt.hpp"
#include <gtest/gtest.h>
#include <random>

class RandomIntTests : public ::testing::Test {
public:
  std::uniform_int_distribution<u_int64_t> dist;
  std::default_random_engine rng;
};

class ConstructorTests : public RandomIntTests {};

TEST_F(ConstructorTests, DefaultConstructorTest) {
  UBigInt bigint;
  ASSERT_EQ(bigint, 0);
  ASSERT_LE(bigint, 0);
  ASSERT_LT(bigint, 1);
  ASSERT_GE(bigint, 0);
  ASSERT_NE(bigint, 1);
  ASSERT_FALSE(bigint > 0);
}

TEST_F(ConstructorTests, NumberConstructorTest) {
  for (size_t i = 0; i < 100; i++) {
    u_int64_t number = dist(rng);
    UBigInt bigint(number);
    ASSERT_EQ(bigint, number);
    ASSERT_EQ(static_cast<u_int32_t>(bigint), number % (1ull << 32));
    ASSERT_EQ(static_cast<u_int64_t>(bigint), number);
  }
}

TEST_F(ConstructorTests, CopyConstructorTest) {
  for (size_t i = 0; i < 100; i++) {
    u_int64_t number = dist(rng);
    const UBigInt first(number);
    auto copy = first;
    ASSERT_EQ(first, number);
    ASSERT_EQ(first, copy);
  }
}
TEST_F(ConstructorTests, MoveConstructorTest) {
  for (size_t i = 0; i < 100; i++) {
    u_int64_t number = dist(rng);
    UBigInt first(number);
    auto copy = std::move(first);
    ASSERT_EQ(first, 0);
    ASSERT_EQ(number, copy);
  }
}

class AssOperatorTests : public RandomIntTests {};

TEST_F(AssOperatorTests, CopyOperatorTest) {
  for (size_t i = 0; i < 100; i++) {
    u_int64_t number = dist(rng);
    const UBigInt first(number);
    UBigInt copy;
    copy = first;
    ASSERT_EQ(first, number);
    ASSERT_EQ(first, copy);
  }
}
TEST_F(AssOperatorTests, MoveOperatorTest) {
  for (size_t i = 0; i < 100; i++) {
    u_int64_t number = dist(rng);
    UBigInt first(number);
    UBigInt copy;
    copy = std::move(first);
    ASSERT_EQ(first, 0);
    ASSERT_EQ(number, copy);
  }
}
TEST_F(AssOperatorTests, NumberCopyOperatorTest) {
  for (size_t i = 0; i < 100; ++i) {
    const auto number = dist(rng);
    UBigInt bigint;
    bigint = number;
    ASSERT_EQ(bigint, number);
  }
}

class StreamConsistencyTests : public ::testing::TestWithParam<std::string> {};

TEST_P(StreamConsistencyTests, StreamConsistencyTest) {
  UBigInt number;
  std::istringstream(GetParam()) >> number;
  ASSERT_EQ((std::ostringstream() << number).str(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(StreamConsistency, StreamConsistencyTests,
                         ::testing::Values("123456", "1.23456e127",
                                           "9.99999e999", "1e6", "6.66e4321",
                                           "4.3e534", "6.746e645",
                                           "1.5876e1000", "73645", "4090",
                                           "420", "69", "3", "0"));

class StreamStandartizationTests
    : public ::testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(StreamStandartizationTests, StreamStandartizationTest) {
  UBigInt number;
  ASSERT_NO_THROW(std::istringstream(GetParam().first) >> number);
  ASSERT_EQ((std::ostringstream() << number).str(), GetParam().second);
}

INSTANTIATE_TEST_SUITE_P(
    StreamStandartization, StreamStandartizationTests,
    ::testing::Values(std::make_pair("1000000", "1e6"),
                      std::make_pair("1234567890987654321", "1.23457e18"),
                      std::make_pair("1.234567890e100", "1.23457e100"),
                      std::make_pair("1.0000e500", "1e500"),
                      std::make_pair("7.000001e15", "7e15"),
                      std::make_pair("123.456e56", "1.23456e58"),
                      std::make_pair("1029384756.6574839201e73", "1.02938e82"),
                      std::make_pair("1.234567e3", "1235"),
                      std::make_pair("9.999999e304", "1e305")));