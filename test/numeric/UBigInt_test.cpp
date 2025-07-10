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