#pragma once
#include <sys/types.h>
#include <charconv>
#include <map>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>
#include <bit>
#include <cstddef>
#include <utility>
#include <cmath>
#include "definitions.hpp"
#include <type_traits>
#include <iostream>
#include <cstdint>
#include <limits>
#include <compare>
#include <algorithm>
#include "CircleArr.hpp"

std::uint64_t consteval operator""_uint64(unsigned long long number) {
  return static_cast<std::uint64_t>(number);
}

class UBigIntUnderflow final : public MachineError {
 public:
  UBigIntUnderflow() {
    message_ptr = kMessage;
  }
  ~UBigIntUnderflow() final = default;

 private:
  constexpr inline static char kMessage[] = "The unsigned big integer cannot go negative";
};

class BigIntInvalidString final : public MachineError {
 public:
  BigIntInvalidString() {
    message_ptr = kMessage;
  }
  ~BigIntInvalidString() final = default;

 private:
  constexpr inline static char kMessage[] = "Use either full integers or '1.01612e288' form";
};

class BigIntDivisionByZero final : public MachineError {
 public:
  BigIntDivisionByZero() {
    message_ptr = kMessage;
  }
  ~BigIntDivisionByZero() final = default;

 private:
  constexpr inline static char kMessage[] = "Can't divide by 0";
};

class UBigInt {
 public:
  using UInt64 = std::uint64_t;
  using UInt32 = std::uint32_t;
  using SizeT = std::size_t;
  constexpr UBigInt() : storage_() {
  }
  constexpr explicit UBigInt(UInt64 number) : storage_() {
    if (number > 0) {
      if (number >= (1_uint64 << 32)) {
        storage_.Reserve(2);
        storage_.PushBack(static_cast<UInt32>(number));
        storage_.PushBack(static_cast<UInt32>(number >> 32));
      } else {
        storage_.Reserve(1);
        storage_.PushBack(static_cast<UInt32>(number));
      }
    }
  }
  template <std::integral Sub64Int>
  constexpr explicit UBigInt(Sub64Int integer) : UBigInt() {
    if constexpr (std::is_signed_v<Sub64Int>) {
      if (integer < 0) {
        throw UBigIntUnderflow{};
      }
    }
    *this = static_cast<UInt64>(integer);
  }
  constexpr UBigInt(const UBigInt &) = default;
  constexpr UBigInt(UBigInt &&) noexcept = default;

  constexpr UBigInt &operator=(const UBigInt &) = default;
  constexpr UBigInt &operator=(UBigInt &&) noexcept = default;
  constexpr inline UBigInt &operator=(UInt64 number) {
    storage_.Clear();
    if (number > 0) {
      if (number <= std::numeric_limits<UInt32>::max()) {
        storage_.Reserve(1).PushBack(static_cast<UInt32>(number));
      } else {
        storage_.Reserve(2).PushBack(static_cast<UInt32>(number)).PushBack(static_cast<UInt32>(number >> 32));
      }
    }
    return *this;
  }
  template <std::integral Sub64Int>
  constexpr UBigInt &operator=(Sub64Int integer) {
    if constexpr (std::is_signed_v<Sub64Int>) {
      if (integer < 0) [[unlikely]] {
        throw UBigIntUnderflow{};
      }
    }
    return *this = static_cast<UInt64>(integer);
  }

  constexpr inline friend UBigInt &operator+=(UBigInt &, const UBigInt &);
  constexpr inline friend UBigInt &operator-=(UBigInt &, const UBigInt &);
  constexpr inline friend UBigInt &operator*=(UBigInt &, const UBigInt &);
  constexpr inline friend UBigInt &operator/=(UBigInt &, const UBigInt &);
  constexpr inline friend UBigInt &operator%=(UBigInt &, const UBigInt &);
  constexpr inline friend UBigInt &operator*=(UBigInt &, UBigInt &&);
  constexpr inline friend UBigInt &operator/=(UBigInt &, UBigInt &&);
  constexpr inline friend UBigInt &operator+=(UBigInt &, UInt64);
  constexpr inline friend UBigInt &operator-=(UBigInt &, UInt64);
  constexpr inline friend UBigInt &operator*=(UBigInt &, UInt64);
  constexpr inline friend UBigInt &operator/=(UBigInt &, UInt64);
  constexpr inline friend UBigInt &operator%=(UBigInt &, UInt64);

  constexpr inline friend std::strong_ordering operator<=>(const UBigInt &, const UBigInt &) noexcept;
  constexpr inline friend bool operator==(const UBigInt &, const UBigInt &) noexcept;
  constexpr inline friend std::strong_ordering operator<=>(const UBigInt &, UInt64) noexcept;
  constexpr inline friend bool operator==(const UBigInt &, UInt64) noexcept;

  constexpr inline friend UBigInt &operator<<=(UBigInt &, SizeT);
  constexpr inline friend UBigInt &operator>>=(UBigInt &, SizeT) noexcept;

  [[nodiscard]] constexpr inline explicit operator UInt64() const;
  [[nodiscard]] constexpr inline explicit operator UInt32() const;

  constexpr inline friend UBigInt Power(const UBigInt &, SizeT);
  constexpr inline friend UBigInt GCD(const UBigInt &, const UBigInt &);

  [[nodiscard]] constexpr inline std::string ToString() const;
  constexpr inline char *ToString(char *, size_t) const noexcept;
  static constexpr inline UBigInt &FromString(std::string_view, UBigInt &);
  [[nodiscard]] static constexpr inline UBigInt FromString(std::string_view);

  friend constexpr inline std::ostream &operator<<(std::ostream &, const UBigInt &);
  friend constexpr inline std::istream &operator>>(std::istream &, UBigInt &);

  struct Approximation {
    long double multiplier;
    SizeT power;
  };

  [[nodiscard]] constexpr inline Approximation GetApproximation() const noexcept;
  [[nodiscard]] constexpr inline std::string Exact() const &;
  [[nodiscard]] constexpr inline std::string Exact() &&;

  constexpr inline ~UBigInt() noexcept = default;

 private:
  CircleArr<UInt32> storage_;

  constexpr inline void Shrink() noexcept {
    while (!storage_.Empty() && storage_.Back() == 0u) {
      storage_.PopBack();
    }
    if (storage_.Size() == 0 && storage_.Capacity() > 16) {
      storage_ = CircleArr<uint32_t>{};
    }
  }

  constexpr inline SizeT RemoveTailZeroes() {
    if (storage_.Size() == 0) {
      return 0;
    }
    SizeT bitcount = 0;
    while (storage_.Front() == 0) {
      bitcount += 32;
      storage_.PopFront();
    }
    auto zeroes = static_cast<SizeT>(std::countr_zero(storage_.Front()));
    *this >>= zeroes;
    return bitcount + zeroes;
  }

  static constexpr inline void ParseInteger(UBigInt &, std::string_view str);
  static constexpr inline long double TenToDoublePower(long double power);
  static constexpr inline uint32_t InPlaceBigDiv(UBigInt &remainder, const UBigInt &div);
  static constexpr inline uint32_t InPlaceBigDiv(UBigInt &remainder, uint64_t div);
};

[[nodiscard]] constexpr inline std::strong_ordering operator<=>(const UBigInt &, const UBigInt &) noexcept;
[[nodiscard]] constexpr inline UBigInt Power(const UBigInt &, UBigInt::SizeT);
[[nodiscard]] constexpr inline UBigInt GCD(const UBigInt &, const UBigInt &);

[[nodiscard]] constexpr inline bool operator==(const UBigInt &first, const UBigInt &second) noexcept {
  return (first <=> second) == std::strong_ordering::equal;
}
[[nodiscard]] constexpr inline std::strong_ordering operator<=>(const UBigInt &bigint,
                                                                UBigInt::UInt64 number) noexcept {
  if (bigint.storage_.Size() > 2) [[likely]] {
    return std::strong_ordering::greater;
  }
  return static_cast<UBigInt::UInt64>(bigint) <=> number;
}
[[nodiscard]] constexpr inline bool operator==(const UBigInt &bigint, UBigInt::UInt64 number) noexcept {
  return (bigint <=> number) == std::strong_ordering::equal;
}

[[nodiscard]] constexpr inline UBigInt::operator UBigInt::UInt64() const {
  if (storage_.Size() == 0) {
    return 0;
  }
  if (storage_.Size() == 1) {
    return storage_[0];
  }
  return (static_cast<UBigInt::UInt64>(storage_[1]) << 32) | static_cast<UBigInt::UInt64>(storage_[0]);
}

[[nodiscard]] constexpr inline UBigInt::operator UBigInt::UInt32() const {
  if (storage_.Size() == 0) {
    return 0;
  }
  return storage_[0];
}
// operator overloads

[[nodiscard]] constexpr inline UBigInt operator+(const UBigInt &first, const UBigInt &second) {
  UBigInt copy(first);
  return std::move(copy += second);
}
[[nodiscard]] constexpr inline UBigInt operator+(UBigInt &&first, const UBigInt &second) {
  return std::move(first += second);
}
[[nodiscard]] constexpr inline UBigInt operator+(const UBigInt &first, UBigInt &&second) {
  return std::move(second += first);
}
[[nodiscard]] constexpr inline UBigInt operator+(UBigInt &&first, UBigInt &&second) {
  return std::move(first += second);
}
[[nodiscard]] constexpr inline UBigInt operator-(const UBigInt &first, const UBigInt &second) {
  if (first == second) {
    return UBigInt{};
  }
  UBigInt copy(first);
  return std::move(copy -= second);
}
[[nodiscard]] constexpr inline UBigInt operator-(UBigInt &&first, const UBigInt &second) {
  return std::move(first -= second);
}
[[nodiscard]] constexpr inline UBigInt operator-(UBigInt &&first, UBigInt &&second) {
  return std::move(first -= second);
}
[[nodiscard]] constexpr inline UBigInt operator*(const UBigInt &first, const UBigInt &second) {
  UBigInt copy(first);
  return std::move(copy *= second);
}
[[nodiscard]] constexpr inline UBigInt operator*(UBigInt &&first, const UBigInt &second) {
  return std::move(first *= second);
}
[[nodiscard]] constexpr inline UBigInt operator*(UBigInt &&first, UBigInt &&second) {
  return std::move(first *= second);
}
[[nodiscard]] constexpr inline UBigInt operator*(const UBigInt &first, UBigInt &&second) {
  return std::move(second *= first);
}
[[nodiscard]] constexpr inline UBigInt operator/(const UBigInt &first, const UBigInt &second) {
  if (first == second) {
    return UBigInt(1);
  }
  if (first < second) {
    return UBigInt{};
  }
  UBigInt copy(first);
  return std::move(copy /= second);
}
[[nodiscard]] constexpr inline UBigInt operator/(UBigInt &&first, const UBigInt &second) {
  return std::move(first /= second);
}
[[nodiscard]] constexpr inline UBigInt operator/(UBigInt &&first, UBigInt &&second) {
  return std::move(first /= second);
}
[[nodiscard]] constexpr inline UBigInt operator/(const UBigInt &first, UBigInt &&second) {
  return first / second;
}
[[nodiscard]] constexpr inline UBigInt operator%(const UBigInt &first, const UBigInt &second) {
  UBigInt copy(first);
  return std::move(copy %= second);
}
[[nodiscard]] constexpr inline UBigInt operator%(UBigInt &&first, const UBigInt &second) {
  return std::move(first %= second);
}
[[nodiscard]] constexpr inline UBigInt operator%(UBigInt &&first, UBigInt &&second) {
  return std::move(first %= second);
}

[[nodiscard]] constexpr inline UBigInt operator<<(const UBigInt &bigint, std::size_t shift) {
  UBigInt copy(bigint);
  return std::move(copy <<= shift);
}
[[nodiscard]] constexpr inline UBigInt operator<<(UBigInt &&bigint, std::size_t shift) {
  return std::move(bigint <<= shift);
}
[[nodiscard]] constexpr inline UBigInt operator>>(const UBigInt &bigint, std::size_t shift) {
  UBigInt copy(bigint);
  return std::move(copy >>= shift);
}
[[nodiscard]] constexpr inline UBigInt operator>>(UBigInt &&bigint, std::size_t shift) {
  return std::move(bigint >>= shift);
}

// Loads of convenience operator templates

template <std::integral Sub64Int>
[[nodiscard]] constexpr inline std::strong_ordering operator<=>(const UBigInt &bigint, Sub64Int integer) noexcept {
  if (integer < 0) {
    return std::strong_ordering::greater;
  }
  return bigint <=> static_cast<std::uint64_t>(integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline bool operator==(const UBigInt &bigint, Sub64Int integer) noexcept {
  return (bigint <=> static_cast<std::uint64_t>(integer)) == std::strong_ordering::equal;
}

template <std::integral Sub64Int>
[[nodiscard]] constexpr inline std::strong_ordering operator<=>(Sub64Int integer, const UBigInt &bigint) noexcept {
  return 0 <=> (bigint <=> integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline bool operator==(Sub64Int integer, const UBigInt &bigint) noexcept {
  return bigint == integer;
}

template <std::integral Sub64Int>
constexpr inline UBigInt &operator+=(UBigInt &bigint, Sub64Int integer) {
  if constexpr (std::is_signed_v<Sub64Int>) {
    if (integer < 0) {
      if constexpr (std::numeric_limits<Sub64Int>::min() <= std::numeric_limits<int64_t>::min()) {
        if (integer == std::numeric_limits<int64_t>::min()) [[unlikely]] {
          return bigint -= static_cast<uint64_t>(1_uint64 << 63);
        }
      }
      return bigint -= static_cast<uint64_t>(-static_cast<int64_t>(integer));
    }
  }
  return bigint += static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
constexpr inline UBigInt &operator-=(UBigInt &bigint, Sub64Int integer) {
  if constexpr (std::is_signed_v<Sub64Int>) {
    if (integer < 0) {
      if constexpr (std::numeric_limits<Sub64Int>::min() <= std::numeric_limits<int64_t>::min()) {
        if (integer == std::numeric_limits<int64_t>::min()) [[unlikely]] {
          return bigint += static_cast<uint64_t>(1_uint64 << 63);
        }
      }
      return bigint += static_cast<uint64_t>(-static_cast<int64_t>(integer));
    }
  }
  return bigint -= static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
constexpr inline UBigInt &operator*=(UBigInt &bigint, Sub64Int integer) {
  if constexpr (std::is_signed_v<Sub64Int>) {
    if (integer < 0) [[unlikely]] {
      throw UBigIntUnderflow{};
    }
  }
  return bigint *= static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
constexpr inline UBigInt &operator/=(UBigInt &bigint, Sub64Int integer) {
  if constexpr (std::is_signed_v<Sub64Int>) {
    if (integer < 0) [[unlikely]] {
      throw UBigIntUnderflow{};
    }
  }
  return bigint /= static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
constexpr inline UBigInt &operator%=(UBigInt &bigint, Sub64Int integer) {
  if constexpr (std::is_signed_v<Sub64Int>) {
    if (integer < 0) {
      if constexpr (std::numeric_limits<Sub64Int>::min() <= std::numeric_limits<int64_t>::min()) {
        if (integer == std::numeric_limits<int64_t>::min()) [[unlikely]] {
          return bigint %= static_cast<uint64_t>(1_uint64 << 63);
        }
      }
      return bigint %= static_cast<uint64_t>(-static_cast<int64_t>(integer));
    }
  }
  return bigint %= static_cast<uint64_t>(integer);
}

template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator+(const UBigInt &bigint, Sub64Int integer) {
  return UBigInt(bigint) + integer;
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator+(UBigInt &&bigint, Sub64Int integer) {
  return std::move(bigint += integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator-(const UBigInt &bigint, Sub64Int integer) {
  return UBigInt(bigint) - integer;
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator-(UBigInt &&bigint, Sub64Int integer) {
  return std::move(bigint -= integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator*(const UBigInt &bigint, Sub64Int integer) {
  if (integer == 0 || bigint == 0) {
    return UBigInt{};
  }
  return UBigInt(bigint) * integer;
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator*(UBigInt &&bigint, Sub64Int integer) {
  return std::move(bigint *= integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator/(const UBigInt &bigint, Sub64Int integer) {
  UBigInt copy(bigint);
  return std::move(copy /= integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator/(UBigInt &&bigint, Sub64Int integer) {
  return std::move(bigint /= integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator%(const UBigInt &bigint, Sub64Int integer) {
  UBigInt copy(bigint);
  return std::move(copy %= integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator%(UBigInt &&bigint, Sub64Int integer) {
  return std::move(bigint %= integer);
}

template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator+(Sub64Int integer, const UBigInt &bigint) {
  return bigint + integer;
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator+(Sub64Int integer, UBigInt &&bigint) {
  return std::move(bigint) + integer;
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator-(Sub64Int integer, const UBigInt &bigint) {
  if (bigint > integer) [[unlikely]] {
    throw UBigIntUnderflow{};
  }
  if (bigint == integer) {
    return UBigInt{};
  }
  UBigInt copy(integer);
  return std::move(copy -= bigint);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator*(Sub64Int integer, const UBigInt &bigint) {
  return bigint * integer;
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator*(Sub64Int integer, UBigInt &&bigint) {
  return std::move(bigint) * integer;
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator/(Sub64Int integer, const UBigInt &bigint) {
  if (bigint > integer) {
    return UBigInt{};
  }
  UBigInt copy(integer);
  return std::move(copy /= bigint);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline UBigInt operator%(Sub64Int integer, const UBigInt &bigint) {
  UBigInt copy(integer);
  return std::move(copy %= bigint);
}

constexpr inline UBigInt &operator+=(UBigInt &first, const UBigInt &second) {
  if (first == 0) {
    return first = second;
  }
  first.storage_.Reserve(second.storage_.Size());
  while (first.storage_.Size() < second.storage_.Size()) {
    first.storage_.PushBack(0);
  }
  uint32_t carryover = 0u;
  for (size_t i = 0; i < second.storage_.Size(); ++i) {
    uint64_t sum = static_cast<uint64_t>(first.storage_[i]) + static_cast<uint64_t>(second.storage_[i]) +
                   static_cast<uint64_t>(carryover);
    first.storage_[i] = static_cast<uint32_t>(sum);
    carryover = static_cast<uint32_t>(sum >> 32);
  }
  try {
    if (carryover > 0u) [[unlikely]] {
      for (size_t i = second.storage_.Size(); carryover > 0u; ++i) {
        if (first.storage_.Size() <= i) {
          first.storage_.PushBack(0u);
        }
        uint64_t sum = static_cast<uint64_t>(first.storage_[i]) + static_cast<uint64_t>(carryover);
        first.storage_[i] = static_cast<uint32_t>(sum % (1_uint64 << 32));
        carryover = static_cast<uint32_t>(sum >> 32);
      }
    }
  } catch (...) {
    first.Shrink();
    throw;
  }
  first.Shrink();
  return first;
}

constexpr inline UBigInt &operator-=(UBigInt &first, const UBigInt &second) {
  if (&first == &second || first == second) {
    return first = 0;
  }
  if (first < second) [[unlikely]] {
    throw UBigIntUnderflow();
  }
  if (first.storage_.Size() <= 2 && second.storage_.Size() <= 2) [[likely]] {
    return first = static_cast<uint64_t>(first) - static_cast<uint64_t>(second);
  }
  uint32_t debt = 0u;
  for (size_t i = 0; i < second.storage_.Size(); ++i) {
    auto available = static_cast<uint64_t>(first.storage_[i]);
    uint64_t subtracted = static_cast<uint64_t>(second.storage_[i]) + static_cast<uint64_t>(debt);
    if (available >= subtracted) {
      first.storage_[i] -= second.storage_[i] + debt;
      debt = 0u;
      continue;
    }
    debt = 1u;
    first.storage_[i] = static_cast<uint32_t>((available | (1_uint64 << 32)) - subtracted);
  }
  for (size_t i = second.storage_.Size(); debt > 0 && i < first.storage_.Size(); i++) {
    if (first.storage_[i] >= debt) [[likely]] {
      first.storage_[i] -= debt;
      break;
    }
    first.storage_[i] = static_cast<uint32_t>(((1_uint64 << 32) | static_cast<uint64_t>(first.storage_[i])) - debt);
  }
  first.Shrink();
  return first;
}

constexpr inline UBigInt &operator*=(UBigInt &first, const UBigInt &second) {
  if (first == 0 || second == 0) {
    return first = 0;
  }
  if (second.storage_.Size() <= 2) {
    return first *= static_cast<uint64_t>(second);
  }
  size_t bitcount = first.RemoveTailZeroes();
  if (&first == &second) {
    first *= auto{second};
    first <<= (2 * bitcount);
  }
  UBigInt buffer{};
  for (size_t i = 0; i < first.storage_.Size(); ++i) {
    buffer += second * first.storage_[i];
    first.storage_[i] = buffer > 0 ? buffer.storage_[0] : 0;
    if (!buffer.storage_.Empty()) {
      buffer.storage_.PopFront();
    }
  }
  for (size_t i = 0; i < buffer.storage_.Size(); ++i) {
    first.storage_.PushBack(buffer.storage_[i]);
  }
  first.Shrink();
  first <<= bitcount;
  return first;
}

constexpr inline uint32_t UBigInt::InPlaceBigDiv(UBigInt &remainder, const UBigInt &div) {
  if (remainder < div) {
    return 0;
  }
  if (remainder == div) {
    remainder = 0;
    return 1;
  }
  auto rem_approx = remainder.GetApproximation();
  auto div_approx = div.GetApproximation();
  long double double_res =
      rem_approx.multiplier / div_approx.multiplier * TenToIntPower(rem_approx.power - div_approx.power);
  uint32_t result = 0;
  if (double_res > std::numeric_limits<uint32_t>::max()) [[unlikely]] {
    result = std::numeric_limits<uint32_t>::max();
  } else {
    result = static_cast<uint32_t>(double_res);
  }
  auto whole = div * result;
  if (remainder <= whole) [[unlikely]] {
    result--;
    whole -= div;
  }
  remainder -= whole;
  if (remainder >= div) [[unlikely]] {
    ++result;
    remainder -= div;
  }
  return result;
}

constexpr inline UBigInt &operator/=(UBigInt &first, const UBigInt &second) {
  if (&first == &second || first == second) {
    return first = 1;
  }
  if (first < second) {
    first = 0;
    return first;
  }
  if (first.storage_.Size() <= 2 && second.storage_.Size() <= 2) {
    return first = static_cast<uint64_t>(first) / static_cast<uint64_t>(second);
  }
  if (second.storage_.Size() <= 1) {
    return first /= static_cast<uint32_t>(second);
  }
  UBigInt remainder;
  for (size_t i = 0; i < first.storage_.Size(); ++i) {
    remainder.storage_.PushFront(first.storage_[first.storage_.Size() - i - 1]);
    first.storage_[first.storage_.Size() - i - 1] = UBigInt::InPlaceBigDiv(remainder, second);
  }
  first.Shrink();
  return first;
}

constexpr inline UBigInt &operator%=(UBigInt &first, const UBigInt &second) {
  if (&first == &second || first == second) {
    return first = 0;
  }
  if (first <= std::numeric_limits<uint64_t>::max() && second <= std::numeric_limits<uint64_t>::max()) {
    return first = static_cast<uint64_t>(first) % static_cast<uint64_t>(second);
  }
  if (second < (1_uint64 << 32)) {
    return first %= static_cast<uint32_t>(second);
  }
  {
    UBigInt remainder;
    for (size_t i = 0; i < first.storage_.Size(); ++i) {
      remainder.storage_.PushFront(first.storage_[first.storage_.Size() - i - 1]);
      UBigInt::InPlaceBigDiv(remainder, second);
    }
    return first = remainder;
  }
}

constexpr inline UBigInt &operator*=(UBigInt &first, UBigInt &&second) {
  if (first == 0 || second == 0) {
    return first = 0;
  }
  if (second.storage_.Size() <= 2) {
    return first *= static_cast<uint64_t>(second);
  }
  if (first.storage_.Size() <= 2) {
    return first = std::move(second *= static_cast<uint64_t>(first));
  }
  size_t bitcount = first.RemoveTailZeroes();
  if (&first == &second) {
    bitcount *= 2;
  } else {
    bitcount += second.RemoveTailZeroes();
  }
  first *= std::as_const(second);
  return first <<= bitcount;
}

constexpr inline UBigInt &operator/=(UBigInt &first, UBigInt &&second) {
  if (&first == &second || first == second) {
    return first = 1;
  }
  if (first < second) {
    first = 0;
    return first;
  }
  if (second.storage_.Size() == 0) {
    throw BigIntDivisionByZero{};
  }
  size_t zeroes = second.RemoveTailZeroes();
  first >>= zeroes;
  return first /= second;
}

constexpr inline UBigInt &operator+=(UBigInt &bigint, uint64_t number) {
  if (bigint == 0) {
    return bigint = number;
  }
  const auto upper = static_cast<uint32_t>(number >> 32);
  const auto lower = static_cast<uint32_t>(number);
  uint32_t carryover = 0;
  {
    uint64_t sum = static_cast<uint64_t>(bigint.storage_[0]) + lower;
    bigint.storage_[0] = static_cast<uint32_t>(sum);
    carryover = static_cast<uint32_t>(sum >> 32);
  }
  if (bigint.storage_.Size() == 1) {
    if (upper > 0) {
      {
        uint64_t sum = static_cast<uint64_t>(upper) + carryover;
        bigint.storage_.PushBack(static_cast<uint32_t>(sum));
        carryover = static_cast<uint32_t>(sum >> 32);
      }
    }
    if (carryover > 0) {
      bigint.storage_.PushBack(carryover);
    }
    return bigint;
  }
  {
    uint64_t sum = (static_cast<uint64_t>(bigint.storage_[1]) + upper) + carryover;
    bigint.storage_[1] = static_cast<uint32_t>(sum);
    carryover = static_cast<uint32_t>(sum >> 32);
  }
  for (size_t i = 2; i < bigint.storage_.Size() && carryover > 0; i++) {
    uint64_t sum = static_cast<uint64_t>(bigint.storage_[i]) + carryover;
    bigint.storage_[i] = static_cast<uint32_t>(sum);
    carryover = static_cast<uint32_t>(sum >> 32);
  }
  if (carryover > 0) {
    bigint.storage_.PushBack(carryover);
  }
  return bigint;
}

constexpr inline UBigInt &operator-=(UBigInt &bigint, uint64_t number) {
  if (number == 0) {
    return bigint;
  }
  if (bigint < number) [[unlikely]] {
    throw UBigIntUnderflow();
  }
  if (bigint.storage_.Size() <= 2) {
    return bigint = static_cast<uint64_t>(bigint) - number;
  }
  if (number < (1_uint64 << 32)) {
    auto debt = static_cast<uint32_t>(number);
    for (size_t pos = 0;; ++pos) {
      if (bigint.storage_[pos] >= debt) {
        bigint.storage_[pos] -= debt;
        break;
      }
      bigint.storage_[pos] = static_cast<uint32_t>(((1_uint64 << 32) + bigint.storage_[pos]) - debt);
      debt = 1u;
    }
    bigint.Shrink();
    return bigint;
  }
  uint32_t head = bigint.storage_.PopFrontValue();
  bigint -= (number >> 32);
  bigint.storage_.PushFront(head);
  return bigint -= (number & ((1_uint64 << 32) - 1));
}

constexpr inline UBigInt &operator*=(UBigInt &bigint, uint64_t number) {
  if (number == 0) {
    return bigint = 0;
  }
  auto zeroes = static_cast<size_t>(std::countr_zero(number));
  bigint <<= zeroes;
  number >>= zeroes;
  if (bigint == 0 || number == 1) {
    return bigint;
  }
  if ((number >> 32) > 0) {
    bigint.storage_.PushBack(0);
  }
  uint32_t carryover = 0;
  uint32_t previous = 0;
  for (size_t pos = 0; pos < bigint.storage_.Size(); pos++) {
    uint64_t first = bigint.storage_[pos] * (number & ((1_uint64 << 32) - 1));
    uint64_t second = previous * (number >> 32);
    uint64_t sum = first + second + carryover;
    previous = bigint.storage_[pos];
    bigint.storage_[pos] = static_cast<uint32_t>(sum);
    carryover = static_cast<uint32_t>(sum >> 32);
  }
  if (carryover > 0) {
    bigint.storage_.PushBack(carryover);
  }
  return bigint;
}

constexpr inline UBigInt &operator/=(UBigInt &bigint, uint64_t div) {
  if (div == 0) [[unlikely]] {
    throw BigIntDivisionByZero();
  }
  auto zeroes = static_cast<uint64_t>(std::countr_zero(div));
  bigint >>= zeroes;
  div >>= zeroes;
  if (div == 1 || bigint == 0) {
    return bigint;
  }
  if (bigint.storage_.Size() <= 2) {
    return bigint = static_cast<uint64_t>(bigint) / div;
  }
  if (div < (1_uint64 << 32)) {
    uint32_t remainder = 0;
    for (size_t i = 0; i < bigint.storage_.Size(); ++i) {
      uint64_t num = static_cast<uint64_t>(bigint.storage_[bigint.storage_.Size() - i - 1]) +
                     (static_cast<uint64_t>(remainder) << 32);
      bigint.storage_[bigint.storage_.Size() - i - 1] = static_cast<uint32_t>(num / div);
      remainder = static_cast<uint32_t>(num % div);
    }
    bigint.Shrink();
    return bigint;
  }
  return bigint /= UBigInt(div);
}
constexpr inline UBigInt &operator%=(UBigInt &bigint, uint64_t div) {
  if (div == 0) {
    throw BigIntDivisionByZero();
  }
  if (div == 1 || bigint == 0) {
    return bigint = 0;
  }
  if (bigint.storage_.Size() <= 2) {
    return bigint = static_cast<uint64_t>(bigint) % div;
  }
  if (div < (1_uint64 << 32)) {
    uint32_t remainder = 0;
    for (size_t i = 0; i < bigint.storage_.Size(); ++i) {
      uint64_t num = static_cast<uint64_t>(bigint.storage_[bigint.storage_.Size() - i - 1]) |
                     (static_cast<uint64_t>(remainder) << 32);
      remainder = static_cast<uint32_t>(num % div);
    }
    return bigint = remainder;
  }
  return bigint %= UBigInt(div);
}

[[nodiscard]] constexpr inline std::strong_ordering operator<=>(const UBigInt &first, const UBigInt &second) noexcept {
  if (&first == &second) {
    return std::strong_ordering::equivalent;
  }
  if (first.storage_.Size() != second.storage_.Size()) {
    return first.storage_.Size() <=> second.storage_.Size();
  }
  size_t size = first.storage_.Size();
  for (size_t i = 0; i < first.storage_.Size(); ++i) {
    if (first.storage_[size - i - 1] != second.storage_[size - i - 1]) {
      return first.storage_[size - i - 1] <=> second.storage_[size - i - 1];
    }
  }
  return std::strong_ordering::equal;
}

constexpr inline UBigInt &operator<<=(UBigInt &bigint, size_t shift) {
  if (bigint == 0) {
    return bigint;
  }
  for (size_t i = 0; i < shift / 32; i++) {
    bigint.storage_.PushFront(0);
  }
  shift %= 32;
  if (shift > 0) {
    uint32_t carryover = 0;
    for (size_t i = 0; i < bigint.storage_.Size(); i++) {
      auto shift_64bit = static_cast<uint64_t>(bigint.storage_[i]);
      shift_64bit <<= shift;
      bigint.storage_[i] = static_cast<uint32_t>(shift_64bit) | carryover;
      carryover = static_cast<uint32_t>(shift_64bit >> 32);
    }
    if (carryover > 0) {
      bigint.storage_.PushBack(carryover);
    }
  }
  return bigint;
}
constexpr inline UBigInt &operator>>=(UBigInt &bigint, size_t shift) noexcept {
  if (bigint == 0) [[unlikely]] {
    return bigint;
  }
  for (size_t i = 0; i < shift / 32; i++) {
    bigint.storage_.PopFront();
  }
  shift %= 32;
  if (shift > 0 && !bigint.storage_.Empty()) {
    uint32_t carryover = 0;
    for (size_t i = bigint.storage_.Size(); i > 0; i--) {
      uint64_t shift_64bit = (static_cast<uint64_t>(bigint.storage_[i - 1]) << 32);
      shift_64bit >>= shift;
      bigint.storage_[i - 1] = static_cast<uint32_t>(shift_64bit >> 32) | carryover;
      carryover = static_cast<uint32_t>(shift_64bit);
    }
  }
  bigint.Shrink();
  return bigint;
}

[[nodiscard]] constexpr inline UBigInt::Approximation UBigInt::GetApproximation() const noexcept {
  if (storage_.Size() == 0) {
    return {0, 0};
  }
  static constexpr long double kPowerRatio = std::numbers::log2e_v<long double> / std::numbers::log10e_v<long double>;
  size_t power_of_2 = 32 * (storage_.Size());
  power_of_2 -= static_cast<size_t>(std::countl_zero(storage_.Back()));
  --power_of_2;
  const long double raw = static_cast<long double>(power_of_2) / kPowerRatio;
  auto power_of_10 = static_cast<size_t>(raw);
  long double mult = TenToDoublePower(raw - static_cast<long double>(power_of_10));
  if (storage_.Size() == 1) {
    mult *= static_cast<long double>(storage_[0]) / static_cast<long double>(1_uint64 << power_of_2);
  } else if (storage_.Size() == 2) {
    mult *= static_cast<long double>((static_cast<uint64_t>(storage_[1]) << 32) | storage_[0]) /
            static_cast<long double>(1_uint64 << power_of_2);
  } else {
    size_t shift = 31 - (power_of_2 % 32);
    mult *= static_cast<long double>((static_cast<uint64_t>(storage_.Back()) << (32 + shift)) |
                                     (static_cast<uint64_t>(storage_[storage_.Size() - 2]) << shift) |
                                     (static_cast<uint64_t>(storage_[storage_.Size() - 3]) >> (32 - shift))) /
            static_cast<long double>(1_uint64 << 63);
  }
  if (mult >= 10 - 5e-12) [[unlikely]] {
    ++power_of_10;
    mult /= 10;
  }
  return {mult, power_of_10};
}

[[nodiscard]] constexpr inline UBigInt Power(const UBigInt &base, size_t power) {
  switch (power) {
    case 0:
      return UBigInt(1);
    case 1:
      return base;
    case 2:
      return base * base;
  }
  if (base == 2) {
    return UBigInt(1) << power;
  }
  if consteval {
    std::vector<UBigInt> powers;
    powers.push_back(base);
    while ((1_uint64 << powers.size()) <= power) {
      powers.emplace_back(powers.back() * powers.back());
    }
    UBigInt result(1);
    for (size_t i = 0; i < powers.size(); ++i) {
      if (power >= (1_uint64 << (powers.size() - i - 1))) {
        result *= powers[powers.size() - i - 1];
        power -= (1_uint64 << (powers.size() - i - 1));
      }
    }
    return result;
  } else {
    static std::map<UBigInt, std::vector<UBigInt>> powers;
    auto location = powers.emplace(base, std::vector<UBigInt>(1, base)).first;
    while ((1_uint64 << location->second.size()) <= power) {
      location->second.push_back(location->second.back() * location->second.back());
    }
    UBigInt result(1);
    for (size_t i = 0; i < location->second.size(); ++i) {
      if (power >= (1_uint64 << (location->second.size() - i - 1))) {
        result *= location->second[location->second.size() - i - 1];
        power -= (1_uint64 << (location->second.size() - i - 1));
      }
    }
    return result;
  }
}

[[nodiscard]] constexpr inline UBigInt GCD(const UBigInt &first, const UBigInt &second) {
  if (first == second) {
    return first;
  }
  if (first == 1 || second == 1) {
    return UBigInt(1);
  }
  if (first == 0) {
    return second;
  }
  if (second == 0) {
    return first;
  }
  if (first <= std::numeric_limits<uint64_t>::max() && second <= std::numeric_limits<uint64_t>::max()) {
    return UBigInt(std::gcd(static_cast<uint64_t>(first), static_cast<uint64_t>(second)));
  }
  UBigInt f = first;
  UBigInt s = second;
  size_t power_of_2 = 0;
  while (f != 0 && s != 0) {
    auto f_mod2 = static_cast<uint32_t>(f) & 1;
    auto s_mod2 = static_cast<uint32_t>(s) & 1;
    if ((f_mod2 == 0) && (s_mod2 == 0)) {
      power_of_2++;
      f >>= 1;
      s >>= 1;
    } else if (f_mod2 == 0) {
      f >>= 1;
    } else if (s_mod2 == 0) {
      s >>= 1;
    } else {
      if (f < s) {
        s -= f;
      } else {
        f -= s;
      }
    }
  }
  if (f < s) {
    return s <<= power_of_2;
  }
  return f <<= power_of_2;
}

constexpr inline char *UBigInt::ToString(char *const start, size_t const size) const noexcept {
  if ((*this) < 1'000'000) {
    {
      auto [ptr, errc] = std::to_chars(start, start + size, static_cast<UInt32>(*this));
      if (errc != std::errc{}) {
        return nullptr;
      }
      return ptr;
    }
  }
  if (size < 9) {
    return nullptr;
  }
  auto approx = GetApproximation();
  if (approx.multiplier >= 10 - 5e-6) [[unlikely]] {
    approx.multiplier = 1;
    approx.power++;
  }
  char *ptr = DoublePrinter(start, 7, approx.multiplier);
  *ptr = 'e';
  ptr++;
  auto [eptr, errc] = std::to_chars(ptr, start + size, approx.power);
  if (errc != std::errc{}) {
    return nullptr;
  }
  return eptr;
}
[[nodiscard]] constexpr inline std::string UBigInt::ToString() const {
  char result[32]{};
  ToString(result, 31);
  return result;
}

constexpr inline UBigInt &UBigInt::FromString(std::string_view str, UBigInt &result) {
  auto exp_pos = str.find('e');
  size_t exp = 0;
  if (exp_pos != std::string_view::npos) {
    auto [ptr, errc] = std::from_chars(str.begin() + exp_pos + 1, str.end(), exp);
    if (ptr != str.end() || errc != std::errc{}) [[unlikely]] {
      throw BigIntInvalidString{};
    }
    str = str.substr(0, exp_pos);
  }
  result = 0;
  auto dot_pos = str.find('.');
  size_t end = std::min(dot_pos, str.size());
  ParseInteger(result, str.substr(0, end));
  if (dot_pos != std::string_view::npos) {
    str.remove_prefix(dot_pos + 1);
    if (str.size() >= exp) {
      ParseInteger(result, str.substr(0, exp));
      if (str.size() > exp && str[exp] >= '5') {
        result += 1;
      }
      return result;
    }
    ParseInteger(result, str);
    if (exp - str.size() <= 19) {
      return result *= TenToIntPower(exp - str.size());
    }
    return result *= Power(UBigInt(10), exp - str.size());
  }
  if (exp <= 19) {
    return result *= TenToIntPower(exp);
  }
  return result *= Power(UBigInt(10), exp);
}

[[nodiscard]] constexpr inline UBigInt UBigInt::FromString(std::string_view str) {
  UBigInt result;
  FromString(str, result);
  return result;
}

constexpr inline void UBigInt::ParseInteger(UBigInt &bigint, std::string_view str) {
  uint64_t buffer = 0;
  while (str.size() >= 19) {
    auto [ptr, errc] = std::from_chars(str.begin(), str.begin() + 19, buffer);
    if (ptr != str.begin() + 19 || errc != std::errc{}) [[unlikely]] {
      throw BigIntInvalidString{};
    }
    str = str.substr(19);
    constexpr auto kMultiplier = TenToIntPower(19);
    bigint *= kMultiplier;
    bigint += buffer;
  }
  if (str.size() > 0) {
    auto [ptr, errc] = std::from_chars(str.begin(), str.end(), buffer);
    if (ptr != str.end() || errc != std::errc{}) [[unlikely]] {
      throw BigIntInvalidString{};
    }
    bigint *= TenToIntPower(str.size());
    bigint += buffer;
  }
}

[[nodiscard]] constexpr inline std::string UBigInt::Exact() && {
  std::string buffer;
  if (*this == 0) {
    buffer.push_back('0');
    return buffer;
  }
  while (*this > 0) {
    buffer.push_back(static_cast<char>('0' + static_cast<uint32_t>(*this % 10)));
    *this /= 10;
  }
  std::reverse(buffer.begin(), buffer.end());
  return buffer;
}

[[nodiscard]] constexpr inline std::string UBigInt::Exact() const & {
  return auto{*this}.Exact();
}

constexpr inline long double UBigInt::TenToDoublePower(long double power) {
  if consteval {
    if (power < 0 || power >= 1) {
      std::unreachable();
    }
    long double y = power * std::numbers::ln10_v<long double>;
    long double result = 1.0L + y;
    long double term = (y * y) / 2.0L;
    result += term;
    for (size_t i = 3; i < 30; i++) {
      (term *= y) /= i;
      result += term;
    }
    return result;
  }
  return std::pow(10, power);
}

constexpr inline std::ostream &operator<<(std::ostream &out, const UBigInt &bigint) {
  return out << bigint.ToString();
}

constexpr inline std::istream &operator>>(std::istream &in, UBigInt &bigint) {
  std::string input;
  in >> input;
  UBigInt::FromString(input, bigint);
  return in;
}
