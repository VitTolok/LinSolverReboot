#include "UBigInt.hpp"
#include <gtest/gtest.h>
#include <random>

static_assert(
    []() constexpr -> bool {
      auto a = UBigInt::FromString("1.2e40");
      auto b = UBigInt::FromString("3.4e39");
      return (a + b) == UBigInt::FromString("1.54e40") && (a + b).ToString() == "1.54e40";
    }(),
    "constexpr string addition test");
static_assert(
    []() constexpr -> bool {
      auto a = UBigInt::FromString("1.2e40");
      auto b = UBigInt::FromString("3.4e39");
      return (a - b) == UBigInt::FromString("8.6e39") && (a - b).ToString() == "8.6e39";
    }(),
    "constexpr string subtraction test");
static_assert(
    []() constexpr -> bool {
      auto a = UBigInt::FromString("1.2e40");
      auto b = UBigInt::FromString("3.4e39");
      return (a * b) == UBigInt::FromString("4.08e79") && (a * b).ToString() == "4.08e79";
    }(),
    "constexpr string multiplication test");
static_assert(
    []() constexpr -> bool {
      auto a = UBigInt::FromString("1.2e40");
      auto b = UBigInt::FromString("3.4e29");
      return (a / b) == UBigInt::FromString("35294117647") && (a / b).ToString() == "3.52941e10";
    }(),
    "constexpr string division test");

class IRandomIntTests : public ::testing::Test {
 public:
  std::uniform_int_distribution<uint64_t> dist;
  std::default_random_engine rng{765756};
};

class IConstructorTests : public IRandomIntTests {};

TEST_F(IConstructorTests, DefaultConstructorTest) {
  UBigInt bigint;
  ASSERT_EQ(bigint, 0);
  ASSERT_LE(bigint, 0);
  ASSERT_LT(bigint, 1);
  ASSERT_GE(bigint, 0);
  ASSERT_NE(bigint, 1);
  ASSERT_FALSE(bigint > 0);
}

TEST_F(IConstructorTests, NumberConstructorTest) {
  for (size_t i = 0; i < 100; i++) {
    uint64_t number = dist(rng);
    UBigInt bigint(number);
    ASSERT_EQ(bigint, number);
    ASSERT_EQ(static_cast<uint32_t>(bigint), number % (1ull << 32));
    ASSERT_EQ(static_cast<uint64_t>(bigint), number);
  }
}

TEST_F(IConstructorTests, CopyConstructorTest) {
  for (size_t i = 0; i < 100; i++) {
    uint64_t number = dist(rng);
    const UBigInt first(number);
    auto copy = first;
    ASSERT_EQ(first, number);
    ASSERT_EQ(first, copy);
  }
}
TEST_F(IConstructorTests, MoveConstructorTest) {
  for (size_t i = 0; i < 100; i++) {
    uint64_t number = dist(rng);
    UBigInt first(number);
    auto copy = std::move(first);
    ASSERT_EQ(first, 0);
    ASSERT_EQ(number, copy);
  }
}

class IAssOperatorTests : public IRandomIntTests {};

TEST_F(IAssOperatorTests, CopyOperatorTest) {
  for (size_t i = 0; i < 100; i++) {
    uint64_t number = dist(rng);
    const UBigInt first(number);
    UBigInt copy;
    copy = first;
    ASSERT_EQ(first, number);
    ASSERT_EQ(first, copy);
  }
}
TEST_F(IAssOperatorTests, MoveOperatorTest) {
  for (size_t i = 0; i < 100; i++) {
    uint64_t number = dist(rng);
    UBigInt first(number);
    UBigInt copy;
    copy = std::move(first);
    ASSERT_EQ(first, 0);
    ASSERT_EQ(number, copy);
  }
}
TEST_F(IAssOperatorTests, NumberCopyOperatorTest) {
  for (size_t i = 0; i < 100; ++i) {
    const auto number = dist(rng);
    UBigInt bigint;
    bigint = number;
    ASSERT_EQ(bigint, number);
  }
}

class IStreamConsistencyTests : public ::testing::TestWithParam<std::string> {};

TEST_P(IStreamConsistencyTests, StreamConsistencyTest) {
  UBigInt number;
  std::istringstream(GetParam()) >> number;
  ASSERT_EQ((std::ostringstream() << number).str(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(StreamConsistency, IStreamConsistencyTests,
                         ::testing::Values("123456", "1.23456e127", "9.99999e999", "1e6", "6.66e4321", "4.3e534",
                                           "6.746e645", "1.5876e1000", "73645", "4090", "420", "69", "3", "0"));

class IStreamStandartizationTests : public ::testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(IStreamStandartizationTests, StreamStandartizationTest) {
  UBigInt number;
  ASSERT_NO_THROW(std::istringstream(GetParam().first) >> number);
  ASSERT_EQ((std::ostringstream() << number).str(), GetParam().second);
}

INSTANTIATE_TEST_SUITE_P(
    StreamStandartization, IStreamStandartizationTests,
    ::testing::Values(std::make_pair("1000000", "1e6"), std::make_pair("1234567890987654321", "1.23457e18"),
                      std::make_pair("1.234567890e100", "1.23457e100"), std::make_pair("1.0000e500", "1e500"),
                      std::make_pair("7.000001e15", "7e15"), std::make_pair("123.456e56", "1.23456e58"),
                      std::make_pair("1029384756.6574839201e73", "1.02938e82"), std::make_pair("1.234567e3", "1235"),
                      std::make_pair("9.999999e304", "1e305"), std::make_pair("9.999999e50000", "1e50001")));

class ISub64OperatorTests : public IRandomIntTests {};

TEST_F(ISub64OperatorTests, AddTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng);
    auto second = dist(rng) % (std::numeric_limits<uint64_t>::max() - first);
    UBigInt number(first);
    ASSERT_EQ(UBigInt(first) + UBigInt(second), first + second);
    ASSERT_EQ(UBigInt(first) + second, first + second);
    ASSERT_EQ(number += second, first + second);
    number = first;
    ASSERT_EQ(number += UBigInt(second), first + second);
    ASSERT_EQ((number = std::min(first, second)) += number, std::min(first, second) * 2);
  }
}

TEST_F(ISub64OperatorTests, SubTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng);
    auto second = dist(rng) % first;
    ASSERT_EQ(UBigInt(first) - UBigInt(second), first - second);
    ASSERT_THROW(auto a = UBigInt(second) - UBigInt(first), UBigIntUnderflow);
    ASSERT_EQ(UBigInt(first) - second, first - second);
    ASSERT_THROW(auto b = UBigInt(second) - first, UBigIntUnderflow);
    UBigInt number(first);
    ASSERT_EQ(number -= second, first - second);
    ASSERT_THROW((number = second) -= first, UBigIntUnderflow);
    ASSERT_EQ((number = first) -= UBigInt(second), first - second);
    ASSERT_THROW((number = second) -= UBigInt(first), UBigIntUnderflow);
    ASSERT_EQ((number = first) -= number, 0);
  }
}

TEST_F(ISub64OperatorTests, MulTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = 1 + (dist(rng) % std::numeric_limits<uint64_t>::max());
    auto second = dist(rng) % (std::numeric_limits<uint64_t>::max() / first);
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

TEST_F(ISub64OperatorTests, DivTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = 1 + (dist(rng) % std::numeric_limits<uint64_t>::max());
    auto second = 1 + (dist(rng) % std::numeric_limits<uint64_t>::max());
    UBigInt number(first);
    ASSERT_THROW(auto a = UBigInt(first) / 0, BigIntDivisionByZero);
    ASSERT_THROW(number /= 0, BigIntDivisionByZero);
    ASSERT_EQ(UBigInt(first) / 1, first);
    ASSERT_EQ(UBigInt(first) / UBigInt(second), first / second);
    ASSERT_EQ(UBigInt(first) / second, first / second);
    ASSERT_EQ((number = first) /= second, first / second);
    ASSERT_EQ((number = first) /= UBigInt(second), first / second);
    ASSERT_EQ((number = first) /= number, 1);
  }
}

TEST_F(ISub64OperatorTests, ModTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = 1 + (dist(rng) % std::numeric_limits<uint64_t>::max());
    auto second = 1 + (dist(rng) % std::numeric_limits<uint64_t>::max());
    UBigInt number(first);
    ASSERT_THROW(auto a = UBigInt(first) % 0, BigIntDivisionByZero);
    ASSERT_THROW(number %= 0, BigIntDivisionByZero);
    ASSERT_EQ(UBigInt(first) % 1, 0);
    ASSERT_EQ(UBigInt(first) % UBigInt(second), first % second);
    ASSERT_EQ(UBigInt(first) % second, first % second);
    ASSERT_EQ((number = first) %= second, first % second);
    ASSERT_EQ((number = first) %= UBigInt(second), first % second);
    ASSERT_EQ((number = first) %= number, 0);
  }
}

TEST_F(ISub64OperatorTests, LTTest) {
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
TEST_F(ISub64OperatorTests, GTTest) {
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
TEST_F(ISub64OperatorTests, LETest) {
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
TEST_F(ISub64OperatorTests, GETest) {
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
TEST_F(ISub64OperatorTests, EQTest) {
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
TEST_F(ISub64OperatorTests, NETest) {
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

TEST_F(ISub64OperatorTests, LeftShiftTest) {
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
    ASSERT_EQ(UBigInt(first) << second, UBigInt(first) * Power(UBigInt(2), second));
  }
}
TEST_F(ISub64OperatorTests, RightShiftTest) {
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

class IStringAddOperatorTests : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {};

TEST_P(IStringAddOperatorTests, AddTest) {
  auto [first, second, result] = GetParam();
  UBigInt first_n;
  UBigInt second_n;
  UBigInt result_n;
  std::istringstream(first) >> first_n;
  std::istringstream(second) >> second_n;
  std::istringstream(result) >> result_n;
  ASSERT_EQ(first_n + second_n, result_n);
  ASSERT_EQ(first_n += second_n, result_n);
  std::istringstream(first) >> first_n;
  ASSERT_NE(first_n + second_n, result_n + 1);
  ASSERT_NE(first_n += second_n, result_n + 1);
}

INSTANTIATE_TEST_SUITE_P(
    AddTest, IStringAddOperatorTests,
    ::testing::Values(std::make_tuple("123", "123000", "123123"),
                      std::make_tuple("1234567890987654321234567890", "102030405060708090807060504030201",
                                      "102031639628599078461381738598091"),
                      std::make_tuple("4e500", "5e495", "4.00005e500"),
                      std::make_tuple("2.6e600", "7e560", "2.6000000000000000000000000000000000000007e600")));

class IStringSubOperatorTests : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {};

TEST_P(IStringSubOperatorTests, SubTest) {
  auto [first, second, result] = GetParam();
  UBigInt first_n;
  UBigInt second_n;
  UBigInt result_n;
  std::istringstream(first) >> first_n;
  std::istringstream(second) >> second_n;
  std::istringstream(result) >> result_n;
  ASSERT_EQ(first_n - second_n, result_n);
  ASSERT_EQ(first_n -= second_n, result_n);
  std::istringstream(first) >> first_n;
  ASSERT_THROW(auto a = second_n - first_n, UBigIntUnderflow);
  ASSERT_THROW(second_n -= first_n, UBigIntUnderflow);
  std::istringstream(first) >> first_n;
  ASSERT_NE(first_n - second_n, result_n + 1);
  ASSERT_NE(first_n -= second_n, result_n + 1);
}

INSTANTIATE_TEST_SUITE_P(
    SubTest, IStringSubOperatorTests,
    ::testing::Values(std::make_tuple("123000", "123", "122877"),
                      std::make_tuple("102030405060708090807060504030201", "1234567890987654321234567890",
                                      "102029170492817103152739269462311"),
                      std::make_tuple("4e500", "5e495", "3.99995e500"),
                      std::make_tuple("2.6e600", "7e560", "2.5999999999999999999999999999999999999993e600")));

class IStringMulOperatorTests : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {};

TEST_P(IStringMulOperatorTests, MulTest) {
  auto [first, second, result] = GetParam();
  UBigInt first_n;
  UBigInt second_n;
  UBigInt result_n;
  std::istringstream(first) >> first_n;
  std::istringstream(second) >> second_n;
  std::istringstream(result) >> result_n;
  ASSERT_EQ(first_n * second_n, result_n);
  ASSERT_EQ(first_n *= second_n, result_n);
  std::istringstream(first) >> first_n;
  ASSERT_NE(first_n * second_n, result_n + 1);
  ASSERT_NE(first_n *= second_n, result_n + 1);
}

INSTANTIATE_TEST_SUITE_P(
    MulTest, IStringMulOperatorTests,
    ::testing::Values(std::make_tuple("123000", "123", "15129000"),
                      std::make_tuple("102030405060708090807060504030201", "1234567890987654321234567890",
                                      "125963461992414480029122753599091047726296511681221744845890"),
                      std::make_tuple("4e500", "5e495", "2e996"), std::make_tuple("2.6e600", "7e560", "1.82e1161")));

class IStringDivOperatorTests : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {};

TEST_P(IStringDivOperatorTests, DivTest) {
  auto [first, second, result] = GetParam();
  UBigInt first_n;
  UBigInt second_n;
  UBigInt result_n;
  std::istringstream(first) >> first_n;
  std::istringstream(second) >> second_n;
  std::istringstream(result) >> result_n;
  ASSERT_EQ(first_n / second_n, result_n);
  ASSERT_EQ(first_n /= second_n, result_n);
  std::istringstream(first) >> first_n;
  ASSERT_NE(first_n / second_n, result_n + 1);
  ASSERT_NE(first_n /= second_n, result_n + 1);
}

INSTANTIATE_TEST_SUITE_P(
    DivTest, IStringDivOperatorTests,
    ::testing::Values(std::make_tuple("123000", "123", "1000"),
                      std::make_tuple("102030405060708090807060504030201", "1234567890987654321234567890", "82644"),
                      std::make_tuple("4e500", "5e495", "8e4"),
                      std::make_tuple("2.6e600", "7e560", "3714285714285714285714285714285714285714")));

class IStringModOperatorTests : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {};

TEST_P(IStringModOperatorTests, ModTest) {
  auto [first, second, result] = GetParam();
  UBigInt first_n;
  UBigInt second_n;
  UBigInt result_n;
  std::istringstream(first) >> first_n;
  std::istringstream(second) >> second_n;
  std::istringstream(result) >> result_n;
  ASSERT_EQ(first_n % second_n, result_n);
  ASSERT_EQ(first_n %= second_n, result_n);
  std::istringstream(first) >> first_n;
  ASSERT_NE(first_n % second_n, result_n + 1);
  ASSERT_NE(first_n %= second_n, result_n + 1);
}

INSTANTIATE_TEST_SUITE_P(ModTest, IStringModOperatorTests,
                         ::testing::Values(std::make_tuple("123000", "123", "0"),
                                           std::make_tuple("102030405060708090807060504030201",
                                                           "1234567890987654321234567890",
                                                           "776277924387082950875329041"),
                                           std::make_tuple("4e500", "5e495", "0")));

class ISub64GCDTest : public IRandomIntTests {};

TEST_F(ISub64GCDTest, GCDTest) {
  for (size_t i = 0; i < 1000; i++) {
    auto first = dist(rng);
    auto second = dist(rng);
    ASSERT_EQ(GCD(UBigInt(first), UBigInt(second)), std::gcd(first, second));
  }
}