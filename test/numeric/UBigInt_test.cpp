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
                         ::testing::Values("123456", "1.23456e127", "9.99999e999", "1e6", "6.66e4321", "4.3e534",
                                           "6.746e645", "1.5876e1000", "73645", "4090", "420", "69", "3", "0"));

class StreamStandartizationTests : public ::testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(StreamStandartizationTests, StreamStandartizationTest) {
  UBigInt number;
  ASSERT_NO_THROW(std::istringstream(GetParam().first) >> number);
  ASSERT_EQ((std::ostringstream() << number).str(), GetParam().second);
}

INSTANTIATE_TEST_SUITE_P(
    StreamStandartization, StreamStandartizationTests,
    ::testing::Values(std::make_pair("1000000", "1e6"), std::make_pair("1234567890987654321", "1.23457e18"),
                      std::make_pair("1.234567890e100", "1.23457e100"), std::make_pair("1.0000e500", "1e500"),
                      std::make_pair("7.000001e15", "7e15"), std::make_pair("123.456e56", "1.23456e58"),
                      std::make_pair("1029384756.6574839201e73", "1.02938e82"), std::make_pair("1.234567e3", "1235"),
                      std::make_pair("9.999999e304", "1e305"), std::make_pair("9.999999e50000", "1e50001")));

class Sub64OperatorTests : public RandomIntTests {};

TEST_F(Sub64OperatorTests, AddTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng);
    auto second = dist(rng) % (std::numeric_limits<u_int64_t>::max() - first);
    UBigInt number(first);
    ASSERT_EQ(UBigInt(first) + UBigInt(second), first + second);
    ASSERT_EQ(UBigInt(first) + second, first + second);
    ASSERT_EQ(number += second, first + second);
    number = first;
    ASSERT_EQ(number += UBigInt(second), first + second);
    ASSERT_EQ((number = std::min(first, second)) += number, std::min(first, second) * 2);
  }
}

TEST_F(Sub64OperatorTests, SubTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng);
    auto second = dist(rng) % first;
    ASSERT_EQ(UBigInt(first) - UBigInt(second), first - second);
    ASSERT_THROW(UBigInt(second) - UBigInt(first), UBigIntUnderflow);
    ASSERT_EQ(UBigInt(first) - second, first - second);
    ASSERT_THROW(UBigInt(second) - first, UBigIntUnderflow);
    UBigInt number(first);
    ASSERT_EQ(number -= second, first - second);
    ASSERT_THROW((number = second) -= first, UBigIntUnderflow);
    ASSERT_EQ((number = first) -= UBigInt(second), first - second);
    ASSERT_THROW((number = second) -= UBigInt(first), UBigIntUnderflow);
    ASSERT_EQ((number = first) -= number, 0);
  }
}

TEST_F(Sub64OperatorTests, MulTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = 1 + (dist(rng) % std::numeric_limits<u_int64_t>::max());
    auto second = dist(rng) % (std::numeric_limits<u_int64_t>::max() / first);
    ASSERT_EQ(UBigInt(first) * 0, 0);
    ASSERT_EQ(UBigInt(first) * 1, first);
    ASSERT_EQ(UBigInt(first) * UBigInt(second), first * second);
    ASSERT_EQ(UBigInt(first) * second, first * second);
    UBigInt number(first);
    ASSERT_EQ(number *= second, first * second);
    ASSERT_EQ((number = first) *= UBigInt(second), first * second);
    ASSERT_EQ((number = std::min(first, second)) *= number, std::min(first, second) * std::min(first, second));
  }
}

TEST_F(Sub64OperatorTests, DivTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = 1 + (dist(rng) % std::numeric_limits<u_int64_t>::max());
    auto second = 1 + (dist(rng) % std::numeric_limits<u_int64_t>::max());
    UBigInt number(first);
    ASSERT_THROW(UBigInt(first) / 0, BigIntDivisionByZero);
    ASSERT_THROW(number /= 0, BigIntDivisionByZero);
    ASSERT_EQ(UBigInt(first) / 1, first);
    ASSERT_EQ(UBigInt(first) / UBigInt(second), first / second);
    ASSERT_EQ(UBigInt(first) / second, first / second);
    ASSERT_EQ((number = first) /= second, first / second);
    ASSERT_EQ((number = first) /= UBigInt(second), first / second);
    ASSERT_EQ((number = first) /= number, 1);
  }
}

TEST_F(Sub64OperatorTests, ModTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = 1 + (dist(rng) % std::numeric_limits<u_int64_t>::max());
    auto second = 1 + (dist(rng) % std::numeric_limits<u_int64_t>::max());
    UBigInt number(first);
    ASSERT_THROW(UBigInt(first) % 0, BigIntDivisionByZero);
    ASSERT_THROW(number %= 0, BigIntDivisionByZero);
    ASSERT_EQ(UBigInt(first) % 1, 0);
    ASSERT_EQ(UBigInt(first) % UBigInt(second), first % second);
    ASSERT_EQ(UBigInt(first) % second, first % second);
    ASSERT_EQ((number = first) %= second, first % second);
    ASSERT_EQ((number = first) %= UBigInt(second), first % second);
    ASSERT_EQ((number = first) %= number, 0);
  }
}

TEST_F(Sub64OperatorTests, LTTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng);
    auto second = dist(rng);
    ASSERT_FALSE(UBigInt(first) < UBigInt(first));
    ASSERT_FALSE(UBigInt(first) < first);
    ASSERT_FALSE(first < UBigInt(first));
    ASSERT_EQ(UBigInt(first) < UBigInt(second), first < second);
    ASSERT_EQ(UBigInt(first) < second, first < second);
    ASSERT_EQ(first < UBigInt(second), first < second);
  }
}
TEST_F(Sub64OperatorTests, GTTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng);
    auto second = dist(rng);
    ASSERT_FALSE(UBigInt(first) > UBigInt(first));
    ASSERT_FALSE(UBigInt(first) > first);
    ASSERT_FALSE(first > UBigInt(first));
    ASSERT_EQ(UBigInt(first) > UBigInt(second), first > second);
    ASSERT_EQ(UBigInt(first) > second, first > second);
    ASSERT_EQ(first > UBigInt(second), first > second);
  }
}
TEST_F(Sub64OperatorTests, LETest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng);
    auto second = dist(rng);
    ASSERT_TRUE(UBigInt(first) <= UBigInt(first));
    ASSERT_TRUE(UBigInt(first) <= first);
    ASSERT_TRUE(first <= UBigInt(first));
    ASSERT_EQ(UBigInt(first) <= UBigInt(second), first <= second);
    ASSERT_EQ(UBigInt(first) <= second, first <= second);
    ASSERT_EQ(first <= UBigInt(second), first <= second);
  }
}
TEST_F(Sub64OperatorTests, GETest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng);
    auto second = dist(rng);
    ASSERT_TRUE(UBigInt(first) >= UBigInt(first));
    ASSERT_TRUE(UBigInt(first) >= first);
    ASSERT_TRUE(first >= UBigInt(first));
    ASSERT_EQ(UBigInt(first) >= UBigInt(second), first >= second);
    ASSERT_EQ(UBigInt(first) >= second, first >= second);
    ASSERT_EQ(first >= UBigInt(second), first >= second);
  }
}
TEST_F(Sub64OperatorTests, EQTest) {
  for (size_t i = 0; i < 100000; i++) {
    auto first = dist(rng);
    auto second = dist(rng);
    ASSERT_TRUE(UBigInt(first) == UBigInt(first));
    ASSERT_TRUE(UBigInt(first) == first);
    ASSERT_TRUE(first == UBigInt(first));
    ASSERT_EQ(UBigInt(first) == UBigInt(second), first == second);
    ASSERT_EQ(UBigInt(first) == second, first == second);
    ASSERT_EQ(first == UBigInt(second), first == second);
  }
}
TEST_F(Sub64OperatorTests, NETest) {
  for (size_t i = 0; i < 100000; i++) {
    auto first = dist(rng);
    auto second = dist(rng);
    ASSERT_FALSE(UBigInt(first) != UBigInt(first));
    ASSERT_FALSE(UBigInt(first) != first);
    ASSERT_FALSE(first != UBigInt(first));
    ASSERT_EQ(UBigInt(first) != UBigInt(second), first != second);
    ASSERT_EQ(UBigInt(first) != second, first != second);
    ASSERT_EQ(first != UBigInt(second), first != second);
  }
}

TEST_F(Sub64OperatorTests, LeftShiftTest) {
  for (size_t i = 0; i < 1000; ++i) {
    auto first = dist(rng) % (1ull << 48);
    auto second = dist(rng) % 16;
    UBigInt number;
    ASSERT_EQ(UBigInt(first) << 0, first);
    ASSERT_EQ((number = first) <<= 0, first);
    ASSERT_EQ(UBigInt(first) << second, first << second);
    ASSERT_EQ((number = first) <<= second, first << second);
    first = dist(rng);
    second = dist(rng) % 1024;
    std::cout << (UBigInt(first) << second).Exact() << '\n';
    std::cout << (UBigInt(first) * Power(UBigInt(2), second)).Exact() << '\n';
    ASSERT_EQ(UBigInt(first) << second, UBigInt(first) * Power(UBigInt(2), second));
  }
}
TEST_F(Sub64OperatorTests, RightShiftTest) {
  for (size_t i = 0; i < 1000; ++i) {
    auto first = dist(rng);
    auto second = dist(rng) % 64;
    UBigInt number;
    ASSERT_EQ(UBigInt(first) >> 0, first);
    ASSERT_EQ((number = first) >>= 0, first);
    ASSERT_EQ(UBigInt(first) >> second, first >> second);
    ASSERT_EQ((number = first) >>= second, first >> second);
  }
}

class StringAddOperatorTests : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {};

TEST_P(StringAddOperatorTests, AddTest) {
  auto [first, second, result] = GetParam();
  UBigInt first_;
  UBigInt second_;
  UBigInt result_;
  std::istringstream(first) >> first_;
  std::istringstream(second) >> second_;
  std::istringstream(result) >> result_;
  ASSERT_EQ(first_ + second_, result_);
  ASSERT_EQ(first_ += second_, result_);
  std::istringstream(first) >> first_;
  ASSERT_NE(first_ + second_, result_ + 1);
  ASSERT_NE(first_ += second_, result_ + 1);
}

INSTANTIATE_TEST_SUITE_P(
    AddTest, StringAddOperatorTests,
    ::testing::Values(std::make_tuple("123", "123000", "123123"),
                      std::make_tuple("1234567890987654321234567890", "102030405060708090807060504030201",
                                      "102031639628599078461381738598091"),
                      std::make_tuple("4e500", "5e495", "4.00005e500"),
                      std::make_tuple("2.6e600", "7e560", "2.6000000000000000000000000000000000000007e600")));

class StringSubOperatorTests : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {};

TEST_P(StringSubOperatorTests, SubTest) {
  auto [first, second, result] = GetParam();
  UBigInt first_;
  UBigInt second_;
  UBigInt result_;
  std::istringstream(first) >> first_;
  std::istringstream(second) >> second_;
  std::istringstream(result) >> result_;
  ASSERT_EQ(first_ - second_, result_);
  ASSERT_EQ(first_ -= second_, result_);
  std::istringstream(first) >> first_;
  ASSERT_THROW(second_ - first_, UBigIntUnderflow);
  ASSERT_THROW(second_ -= first_, UBigIntUnderflow);
  std::istringstream(first) >> first_;
  ASSERT_NE(first_ - second_, result_ + 1);
  ASSERT_NE(first_ -= second_, result_ + 1);
}

INSTANTIATE_TEST_SUITE_P(
    SubTest, StringSubOperatorTests,
    ::testing::Values(std::make_tuple("123000", "123", "122877"),
                      std::make_tuple("102030405060708090807060504030201", "1234567890987654321234567890",
                                      "102029170492817103152739269462311"),
                      std::make_tuple("4e500", "5e495", "3.99995e500"),
                      std::make_tuple("2.6e600", "7e560", "2.5999999999999999999999999999999999999993e600")));

class StringMulOperatorTests : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {};

TEST_P(StringMulOperatorTests, MulTest) {
  auto [first, second, result] = GetParam();
  UBigInt first_;
  UBigInt second_;
  UBigInt result_;
  std::istringstream(first) >> first_;
  std::istringstream(second) >> second_;
  std::istringstream(result) >> result_;
  ASSERT_EQ(first_ * second_, result_);
  ASSERT_EQ(first_ *= second_, result_);
  std::istringstream(first) >> first_;
  ASSERT_NE(first_ * second_, result_ + 1);
  ASSERT_NE(first_ *= second_, result_ + 1);
}

INSTANTIATE_TEST_SUITE_P(
    MulTest, StringMulOperatorTests,
    ::testing::Values(std::make_tuple("123000", "123", "15129000"),
                      std::make_tuple("102030405060708090807060504030201", "1234567890987654321234567890",
                                      "125963461992414480029122753599091047726296511681221744845890"),
                      std::make_tuple("4e500", "5e495", "2e996"), std::make_tuple("2.6e600", "7e560", "1.82e1161")));

class StringDivOperatorTests : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {};

TEST_P(StringDivOperatorTests, DivTest) {
  auto [first, second, result] = GetParam();
  UBigInt first_;
  UBigInt second_;
  UBigInt result_;
  std::istringstream(first) >> first_;
  std::istringstream(second) >> second_;
  std::istringstream(result) >> result_;
  ASSERT_EQ(first_ / second_, result_);
  ASSERT_EQ(first_ /= second_, result_);
  std::istringstream(first) >> first_;
  ASSERT_NE(first_ / second_, result_ + 1);
  ASSERT_NE(first_ /= second_, result_ + 1);
}

INSTANTIATE_TEST_SUITE_P(
    DivTest, StringDivOperatorTests,
    ::testing::Values(std::make_tuple("123000", "123", "1000"),
                      std::make_tuple("102030405060708090807060504030201", "1234567890987654321234567890", "82644"),
                      std::make_tuple("4e500", "5e495", "8e4"),
                      std::make_tuple("2.6e600", "7e560", "3714285714285714285714285714285714285714")));

class StringModOperatorTests : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {};

TEST_P(StringModOperatorTests, DivTest) {
  auto [first, second, result] = GetParam();
  UBigInt first_;
  UBigInt second_;
  UBigInt result_;
  std::istringstream(first) >> first_;
  std::istringstream(second) >> second_;
  std::istringstream(result) >> result_;
  ASSERT_EQ(first_ % second_, result_);
  ASSERT_EQ(first_ %= second_, result_);
  std::istringstream(first) >> first_;
  ASSERT_NE(first_ % second_, result_ + 1);
  ASSERT_NE(first_ %= second_, result_ + 1);
}

INSTANTIATE_TEST_SUITE_P(ModTest, StringModOperatorTests,
                         ::testing::Values(std::make_tuple("123000", "123", "0"),
                                           std::make_tuple("102030405060708090807060504030201",
                                                           "1234567890987654321234567890",
                                                           "776277924387082950875329041"),
                                           std::make_tuple("4e500", "5e495", "0")));

class Sub64GCDTest : public RandomIntTests {};

TEST_F(Sub64GCDTest, GCDTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng);
    auto second = dist(rng);
    ASSERT_EQ(GCD(UBigInt(first), UBigInt(second)), std::gcd(first, second));
  }
}