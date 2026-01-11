#pragma once
#include "UBigInt.hpp"

class BigInt {
 public:
  constexpr inline BigInt() : abs_(), pos_(false) {
  }
  template <std::integral Sub64Int>
  explicit constexpr inline BigInt(Sub64Int integer) : BigInt() {
    *this = integer;
  }
  constexpr inline BigInt(const BigInt &) = default;
  constexpr inline BigInt(BigInt &&) noexcept = default;
  explicit constexpr inline BigInt(const UBigInt &bigint) : abs_(bigint), pos_(true) {
  }
  explicit constexpr inline BigInt(UBigInt &&bigint) noexcept : abs_(std::move(bigint)), pos_(true) {
  }

  constexpr inline BigInt &operator=(const BigInt &) = default;
  constexpr inline BigInt &operator=(BigInt &&) noexcept = default;
  template <std::integral Sub64Int>
  constexpr inline BigInt &operator=(Sub64Int integer) {
    if constexpr (std::is_signed_v<Sub64Int>) {
      if (integer < 0) {
        pos_ = false;
        if (integer == std::numeric_limits<int64_t>::min()) {
          abs_ = static_cast<uint64_t>(1_uint64 << 63);
        } else {
          abs_ = static_cast<uint64_t>(-static_cast<int64_t>(integer));
        }
        return *this;
      }
    }
    pos_ = true;
    abs_ = static_cast<uint64_t>(integer);
    return *this;
  }

  friend constexpr inline BigInt &operator+=(BigInt &, const UBigInt &);
  friend constexpr inline BigInt &operator-=(BigInt &, const UBigInt &);
  friend constexpr inline BigInt &operator*=(BigInt &, const UBigInt &);
  friend constexpr inline BigInt &operator/=(BigInt &, const UBigInt &);
  friend constexpr inline BigInt &operator%=(BigInt &, const UBigInt &);
  friend constexpr inline BigInt &operator+=(BigInt &, uint64_t);
  friend constexpr inline BigInt &operator-=(BigInt &, uint64_t);
  friend constexpr inline BigInt &operator*=(BigInt &, uint64_t);
  friend constexpr inline BigInt &operator/=(BigInt &, uint64_t);
  friend constexpr inline BigInt &operator%=(BigInt &, uint64_t);
  friend constexpr inline BigInt &operator+=(BigInt &, const BigInt &);
  friend constexpr inline BigInt &operator-=(BigInt &, const BigInt &);
  friend constexpr inline BigInt &operator*=(BigInt &, const BigInt &);
  friend constexpr inline BigInt &operator/=(BigInt &, const BigInt &);
  friend constexpr inline BigInt &operator%=(BigInt &, const BigInt &);
  friend constexpr inline UBigInt &operator+=(UBigInt &, const BigInt &);
  friend constexpr inline UBigInt &operator-=(UBigInt &, const BigInt &);
  friend constexpr inline UBigInt &operator*=(UBigInt &, const BigInt &);
  friend constexpr inline UBigInt &operator/=(UBigInt &, const BigInt &);
  friend constexpr inline UBigInt &operator%=(UBigInt &, const BigInt &);

  friend constexpr inline BigInt operator-(const BigInt &);
  friend constexpr inline BigInt operator-(BigInt &&) noexcept;

  friend constexpr inline BigInt operator+(const BigInt &, const BigInt &);
  friend constexpr inline BigInt operator+(BigInt &&, const BigInt &);
  friend constexpr inline BigInt operator+(const BigInt &, BigInt &&);
  friend constexpr inline BigInt operator+(BigInt &&, BigInt &&);
  friend constexpr inline BigInt operator-(const BigInt &, const BigInt &);
  friend constexpr inline BigInt operator-(BigInt &&, const BigInt &);
  friend constexpr inline BigInt operator-(const BigInt &, BigInt &&);
  friend constexpr inline BigInt operator-(BigInt &&, BigInt &&);
  friend constexpr inline BigInt operator*(const BigInt &, const BigInt &);
  friend constexpr inline BigInt operator*(BigInt &&, const BigInt &);
  friend constexpr inline BigInt operator*(const BigInt &, BigInt &&);
  friend constexpr inline BigInt operator*(BigInt &&, BigInt &&);
  friend constexpr inline BigInt operator/(const BigInt &, const BigInt &);
  friend constexpr inline BigInt operator/(BigInt &&, const BigInt &);
  friend constexpr inline BigInt operator/(const BigInt &, BigInt &&);
  friend constexpr inline BigInt operator/(BigInt &&, BigInt &&);
  friend constexpr inline BigInt operator%(const BigInt &, const BigInt &);
  friend constexpr inline BigInt operator%(BigInt &&, const BigInt &);
  friend constexpr inline BigInt operator%(const BigInt &, BigInt &&);
  friend constexpr inline BigInt operator%(BigInt &&, BigInt &&);

  friend constexpr inline BigInt operator+(const BigInt &, const UBigInt &);
  friend constexpr inline BigInt operator+(BigInt &&, const UBigInt &);
  friend constexpr inline BigInt operator+(const BigInt &, UBigInt &&);
  friend constexpr inline BigInt operator+(BigInt &&, UBigInt &&);
  friend constexpr inline BigInt operator-(const BigInt &, const UBigInt &);
  friend constexpr inline BigInt operator-(BigInt &&, const UBigInt &);
  friend constexpr inline BigInt operator-(const BigInt &, UBigInt &&);
  friend constexpr inline BigInt operator-(BigInt &&, UBigInt &&);
  friend constexpr inline BigInt operator*(const BigInt &, const UBigInt &);
  friend constexpr inline BigInt operator*(BigInt &&, const UBigInt &);
  friend constexpr inline BigInt operator*(const BigInt &, UBigInt &&);
  friend constexpr inline BigInt operator*(BigInt &&, UBigInt &&);
  friend constexpr inline BigInt operator/(const BigInt &, const UBigInt &);
  friend constexpr inline BigInt operator/(BigInt &&, const UBigInt &);
  friend constexpr inline BigInt operator/(const BigInt &, UBigInt &&);
  friend constexpr inline BigInt operator/(BigInt &&, UBigInt &&);
  friend constexpr inline BigInt operator%(const BigInt &, const UBigInt &);
  friend constexpr inline BigInt operator%(BigInt &&, const UBigInt &);
  friend constexpr inline BigInt operator%(const BigInt &, UBigInt &&);
  friend constexpr inline BigInt operator%(BigInt &&, UBigInt &&);

  friend constexpr inline BigInt operator+(const UBigInt &, const BigInt &);
  friend constexpr inline BigInt operator+(UBigInt &&, const BigInt &);
  friend constexpr inline BigInt operator+(const UBigInt &, BigInt &&);
  friend constexpr inline BigInt operator+(UBigInt &&, BigInt &&);
  friend constexpr inline BigInt operator-(const UBigInt &, const BigInt &);
  friend constexpr inline BigInt operator-(UBigInt &&, const BigInt &);
  friend constexpr inline BigInt operator-(const UBigInt &, BigInt &&);
  friend constexpr inline BigInt operator-(UBigInt &&, BigInt &&);
  friend constexpr inline BigInt operator*(const UBigInt &, const BigInt &);
  friend constexpr inline BigInt operator*(UBigInt &&, const BigInt &);
  friend constexpr inline BigInt operator*(const UBigInt &, BigInt &&);
  friend constexpr inline BigInt operator*(UBigInt &&, BigInt &&);
  friend constexpr inline BigInt operator/(const UBigInt &, const BigInt &);
  friend constexpr inline BigInt operator/(UBigInt &&, const BigInt &);
  friend constexpr inline BigInt operator/(const UBigInt &, BigInt &&);
  friend constexpr inline BigInt operator/(UBigInt &&, BigInt &&);
  friend constexpr inline BigInt operator%(const UBigInt &, const BigInt &);
  friend constexpr inline BigInt operator%(UBigInt &&, const BigInt &);
  friend constexpr inline BigInt operator%(const UBigInt &, BigInt &&);
  friend constexpr inline BigInt operator%(UBigInt &&, BigInt &&);

  friend constexpr inline std::strong_ordering operator<=>(const BigInt &, const UBigInt &) noexcept;
  friend constexpr inline bool operator==(const BigInt &, const UBigInt &) noexcept;
  friend constexpr inline std::strong_ordering operator<=>(const BigInt &, const BigInt &) noexcept;
  friend constexpr inline bool operator==(const BigInt &, const BigInt &) noexcept;
  friend constexpr inline std::strong_ordering operator<=>(const UBigInt &, const BigInt &) noexcept;
  friend constexpr inline bool operator==(const UBigInt &, const BigInt &) noexcept;

  friend constexpr inline BigInt &operator<<=(BigInt &, size_t);
  friend constexpr inline BigInt &operator>>=(BigInt &, size_t) noexcept;
  friend constexpr inline BigInt operator<<(const BigInt &, size_t);
  friend constexpr inline BigInt operator>>(const BigInt &, size_t);
  friend constexpr inline BigInt operator<<(BigInt &&, size_t);
  friend constexpr inline BigInt operator>>(BigInt &&, size_t);

  explicit constexpr inline operator int64_t() const;
  constexpr inline const UBigInt &Abs() const & noexcept {
    return abs_;
  }
  constexpr inline UBigInt &Abs() & noexcept {
    return abs_;
  }
  constexpr inline const UBigInt &&Abs() const && noexcept {
    return std::move(abs_);
  }
  constexpr inline UBigInt &&Abs() && noexcept {
    return std::move(abs_);
  }

  constexpr inline char *ToString(char *, size_t) const noexcept;
  [[nodiscard]] constexpr inline std::string ToString() const;
  [[nodiscard]] static constexpr inline BigInt FromString(std::string_view);
  static constexpr inline BigInt &FromString(std::string_view, BigInt &);

  friend constexpr inline std::ostream &operator<<(std::ostream &, const BigInt &);
  friend constexpr inline std::istream &operator>>(std::istream &, BigInt &);
  [[nodiscard]] constexpr inline std::string Exact() const &;
  [[nodiscard]] constexpr inline std::string Exact() &&;

  using Approximation = UBigInt::Approximation;
  [[nodiscard]] constexpr inline Approximation GetApproximation() const noexcept;
  constexpr inline ~BigInt() = default;

 private:
  UBigInt abs_;
  bool pos_;

 public:  // template declarations
  template <std::integral Sub64Int>
  friend constexpr inline BigInt &operator+=(BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt &operator-=(BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt &operator*=(BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt &operator/=(BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt &operator%=(BigInt &, Sub64Int);

  template <std::integral Sub64Int>
  friend constexpr inline BigInt operator+(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt operator+(BigInt &&, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt operator-(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt operator-(BigInt &&, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt operator*(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt operator*(BigInt &&, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt operator/(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt operator/(BigInt &&, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt operator%(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend constexpr inline BigInt operator%(BigInt &&, Sub64Int);

  template <std::integral Sub64Int>
  friend constexpr inline std::strong_ordering operator<=>(const BigInt &, Sub64Int) noexcept;
  template <std::integral Sub64Int>
  friend constexpr inline bool operator==(const BigInt &, Sub64Int) noexcept;

  template <std::integral Sub64Int>
  friend constexpr inline std::strong_ordering operator<=>(Sub64Int, const BigInt &) noexcept;
  template <std::integral Sub64Int>
  friend constexpr inline bool operator==(Sub64Int, const BigInt &) noexcept;
};

// template definitions

template <std::integral Sub64Int>
constexpr inline BigInt &operator+=(BigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint -= (1_uint64 << 63);
    }
    return bigint -= static_cast<uint64_t>(-static_cast<int64_t>(integer));
  }
  return bigint += static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
constexpr inline BigInt &operator-=(BigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint += (1_uint64 << 63);
    }
    return bigint += static_cast<uint64_t>(-static_cast<int64_t>(integer));
  }
  return bigint -= static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
constexpr inline BigInt &operator*=(BigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    bigint.pos_ = !bigint.pos_;
    if (integer == -1) {
      return bigint;
    }
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint *= (1_uint64 << 63);
    }
    return bigint *= static_cast<uint64_t>(-static_cast<int64_t>(integer));
  }
  if (integer == 1) {
    return bigint;
  }
  return bigint *= static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
constexpr inline BigInt &operator/=(BigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    bigint.pos_ = !bigint.pos_;
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint /= (1_uint64 << 63);
    }
    return bigint /= static_cast<uint64_t>(-static_cast<int64_t>(integer));
  }
  return bigint /= static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
constexpr inline BigInt &operator%=(BigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint %= (1_uint64 << 63);
    }
    return bigint %= static_cast<uint64_t>(-static_cast<int64_t>(integer));
  }
  return bigint %= static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline BigInt operator+(const BigInt &bigint, Sub64Int integer) {
  BigInt copy(bigint);
  return std::move(copy += integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline BigInt operator+(BigInt &&bigint, Sub64Int integer) {
  return std::move(bigint += integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline BigInt operator-(const BigInt &bigint, Sub64Int integer) {
  if (bigint == integer) {
    return BigInt{};
  }
  BigInt copy(bigint);
  return std::move(copy -= integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline BigInt operator-(BigInt &&bigint, Sub64Int integer) {
  return std::move(bigint -= integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline BigInt operator*(const BigInt &bigint, Sub64Int integer) {
  BigInt copy(bigint);
  return std::move(copy *= integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline BigInt operator*(BigInt &&bigint, Sub64Int integer) {
  return std::move(bigint *= integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline BigInt operator/(const BigInt &bigint, Sub64Int integer) {
  if (integer == 0) {
    throw BigIntDivisionByZero{};
  }
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      if (bigint.Abs() < (1_uint64 << 63)) {
        return BigInt{};
      }
      if (bigint.Abs() == (1_uint64 << 63)) {
        return BigInt(1) * ((bigint < 0) ? 1 : -1);
      }
    }
    if (bigint.Abs() < -integer) {
      return BigInt{};
    }
    if (bigint.Abs() == -integer) {
      return BigInt(1) * ((bigint < 0) ? 1 : -1);
    }
  }
  if (bigint.Abs() < integer) {
    return BigInt{};
  }
  if (bigint.Abs() == integer) {
    return BigInt(1) * ((bigint < 0) ? -1 : 1);
  }
  BigInt copy(bigint);
  return std::move(copy /= integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline BigInt operator/(BigInt &&bigint, Sub64Int integer) {
  return std::move(bigint /= integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline BigInt operator%(const BigInt &bigint, Sub64Int integer) {
  if (integer == 0) {
    throw BigIntDivisionByZero{};
  }
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      if (bigint.Abs() == (1_uint64 << 63)) {
        return BigInt{};
      }
    }
    if (bigint.Abs() == -integer) {
      return BigInt{};
    }
  }
  if (bigint.Abs() == integer) {
    return BigInt{};
  }
  BigInt copy(bigint);
  return std::move(copy %= integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline BigInt operator%(BigInt &&bigint, Sub64Int integer) {
  return std::move(bigint %= integer);
}

template <std::integral Sub64Int>
[[nodiscard]] constexpr inline std::strong_ordering operator<=>(const BigInt &bigint, Sub64Int integer) noexcept {
  if constexpr (std::is_signed_v<Sub64Int>) {
    if (integer < 0) {
      if (bigint.pos_ || bigint.Abs() == 0) {
        return std::strong_ordering::greater;
      }
      if (integer == std::numeric_limits<int64_t>::min()) {
        return (1_uint64 << 63) <=> bigint.Abs();
      }
      return (-integer) <=> bigint.Abs();
    }
  }
  if (!bigint.pos_ && bigint.Abs() > 0) {
    return std::strong_ordering::less;
  }
  return bigint.Abs() <=> integer;
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline bool operator==(const BigInt &bigint, Sub64Int integer) noexcept {
  return (bigint <=> integer) == std::strong_ordering::equal;
}

template <std::integral Sub64Int>
[[nodiscard]] constexpr inline std::strong_ordering operator<=>(Sub64Int integer, const BigInt &bigint) noexcept {
  return 0 <=> (bigint <=> integer);
}
template <std::integral Sub64Int>
[[nodiscard]] constexpr inline bool operator==(Sub64Int integer, const BigInt &bigint) noexcept {
  return bigint == integer;
}

// constexpr inline function definitions
constexpr inline char *BigInt::ToString(char *const start, const size_t size) const noexcept {
  if (!pos_ && abs_ != 0) {
    if (size < 1) {
      return nullptr;
    }
    *start = '-';
    return abs_.ToString(start + 1, size - 1);
  }
  return abs_.ToString(start, size);
}
[[nodiscard]] constexpr inline std::string BigInt::ToString() const {
  char result[32]{};
  ToString(result, 31);
  return result;
}
[[nodiscard]] constexpr inline BigInt BigInt::FromString(std::string_view str) {
  BigInt result;
  FromString(str, result);
  return result;
}
constexpr inline BigInt &BigInt::FromString(std::string_view str, BigInt &result) {
  result = 0;
  if (str.empty()) {
    return result;
  }
  if (str[0] == '-') {
    str.remove_prefix(1);
    result.pos_ = false;
  } else {
    result.pos_ = true;
  }
  UBigInt::FromString(str, result.abs_);
  return result;
}

constexpr inline std::ostream &operator<<(std::ostream &out, const BigInt &bigint) {
  if (!bigint.pos_ && bigint.abs_ != 0) {
    out << '-';
  }
  return out << bigint.abs_;
}

constexpr inline std::istream &operator>>(std::istream &in, BigInt &bigint) {
  std::string input;
  in >> input;
  BigInt::FromString(input, bigint);
  return in;
}

[[nodiscard]] constexpr inline std::string BigInt::Exact() const & {
  return ((pos_ || (abs_ == 0)) ? "" : "-") + abs_.Exact();
}
[[nodiscard]] constexpr inline std::string BigInt::Exact() && {
  return ((pos_ || (abs_ == 0)) ? "" : "-") + std::move(abs_).Exact();
}

[[nodiscard]] constexpr inline BigInt::Approximation BigInt::GetApproximation() const noexcept {
  auto approx = abs_.GetApproximation();
  if (!pos_ && abs_ > 0) {
    approx.multiplier *= -1;
  }
  return approx;
}

constexpr inline BigInt &operator<<=(BigInt &bigint, size_t shift) {
  bigint.abs_ <<= shift;
  return bigint;
}
constexpr inline BigInt &operator>>=(BigInt &bigint, size_t shift) noexcept {
  bigint.abs_ >>= shift;
  return bigint;
}
[[nodiscard]] constexpr inline BigInt operator<<(const BigInt &bigint, size_t shift) {
  BigInt copy = bigint;
  copy.abs_ <<= shift;
  return copy;
}
[[nodiscard]] constexpr inline BigInt operator>>(const BigInt &bigint, size_t shift) {
  BigInt copy = bigint;
  copy.abs_ >>= shift;
  return copy;
}
[[nodiscard]] constexpr inline BigInt operator<<(BigInt &&bigint, size_t shift) {
  bigint.abs_ <<= shift;
  return std::move(bigint);
}
[[nodiscard]] constexpr inline BigInt operator>>(BigInt &&bigint, size_t shift) {
  bigint.abs_ >>= shift;
  return std::move(bigint);
}

[[nodiscard]] constexpr inline BigInt::operator int64_t() const {
  auto modulus = static_cast<uint64_t>(abs_);
  if (modulus == (1ull << 63) && !pos_) {
    return std::numeric_limits<int64_t>::min();
  }
  modulus %= (1ull << 63);
  return static_cast<int64_t>(modulus) * (pos_ ? 1 : -1);
}

[[nodiscard]] constexpr inline std::strong_ordering operator<=>(const BigInt &first, const BigInt &second) noexcept {
  if (first == 0 && second == 0) {
    return std::strong_ordering::equal;
  }
  if (first.pos_ != second.pos_) {
    return first.pos_ <=> second.pos_;
  }
  if (first.pos_) {
    return first.abs_ <=> second.abs_;
  }
  return 0 <=> (first.abs_ <=> second.abs_);
}
[[nodiscard]] constexpr inline bool operator==(const BigInt &first, const BigInt &second) noexcept {
  return (first <=> second) == std::strong_ordering::equal;
}
[[nodiscard]] constexpr inline std::strong_ordering operator<=>(const UBigInt &first, const BigInt &second) noexcept {
  if (second.abs_ == 0) {
    return first <=> 0;
  }
  if (!second.pos_) {
    return std::strong_ordering::greater;
  }
  return first <=> second.abs_;
}
[[nodiscard]] constexpr inline bool operator==(const UBigInt &first, const BigInt &second) noexcept {
  return (first <=> second) == std::strong_ordering::equal;
}
[[nodiscard]] constexpr inline std::strong_ordering operator<=>(const BigInt &first, const UBigInt &second) noexcept {
  return 0 <=> (second <=> first);
}
[[nodiscard]] constexpr inline bool operator==(const BigInt &first, const UBigInt &second) noexcept {
  return second == first;
}

[[nodiscard]] constexpr inline BigInt operator-(const BigInt &bigint) {
  BigInt copy(bigint);
  copy.pos_ = !copy.pos_;
  return copy;
}
[[nodiscard]] constexpr inline BigInt operator-(BigInt &&bigint) noexcept {
  bigint.pos_ = !bigint.pos_;
  return std::move(bigint);
}

constexpr inline BigInt &operator+=(BigInt &first, const BigInt &second) {
  if (second < 0) {
    return first -= second.Abs();
  }
  return first += second.Abs();
}
constexpr inline BigInt &operator-=(BigInt &first, const BigInt &second) {
  if (second < 0) {
    return first += second.Abs();
  }
  return first -= second.Abs();
}
constexpr inline BigInt &operator*=(BigInt &first, const BigInt &second) {
  if (second < 0) {
    first *= -1;
  }
  return first *= second.Abs();
}
constexpr inline BigInt &operator/=(BigInt &first, const BigInt &second) {
  if (second < 0) {
    first *= -1;
  }
  return first /= second.Abs();
}
constexpr inline BigInt &operator%=(BigInt &first, const BigInt &second) {
  return first %= second.Abs();
}

constexpr inline UBigInt &operator+=(UBigInt &first, const BigInt &second) {
  if (second > 0) {
    return first += second.Abs();
  }
  return first -= second.Abs();
}
constexpr inline UBigInt &operator-=(UBigInt &first, const BigInt &second) {
  if (second > 0) {
    return first -= second.Abs();
  }
  return first += second.Abs();
}
constexpr inline UBigInt &operator*=(UBigInt &first, const BigInt &second) {
  if (second >= 0) {
    return first *= second.Abs();
  }
  throw UBigIntUnderflow{};
}
constexpr inline UBigInt &operator/=(UBigInt &first, const BigInt &second) {
  if (second >= 0) {
    return first /= second.Abs();
  }
  throw UBigIntUnderflow{};
}
constexpr inline UBigInt &operator%=(UBigInt &first, const BigInt &second) {
  return first %= second.Abs();
}

[[nodiscard]] constexpr inline BigInt operator+(const BigInt &first, const BigInt &second) {
  BigInt copy(first);
  return std::move(copy += second);
}
[[nodiscard]] constexpr inline BigInt operator+(BigInt &&first, const BigInt &second) {
  return std::move(first += second);
}
[[nodiscard]] constexpr inline BigInt operator+(const BigInt &first, BigInt &&second) {
  return std::move(second += first);
}
[[nodiscard]] constexpr inline BigInt operator+(BigInt &&first, BigInt &&second) {
  return std::move(first += second);
}
[[nodiscard]] constexpr inline BigInt operator-(const BigInt &first, const BigInt &second) {
  if (first == second) {
    return BigInt{};
  }
  BigInt copy(first);
  return std::move(copy -= second);
}
[[nodiscard]] constexpr inline BigInt operator-(BigInt &&first, const BigInt &second) {
  return std::move(first -= second);
}
[[nodiscard]] constexpr inline BigInt operator-(const BigInt &first, BigInt &&second) {
  return std::move((second -= first) *= -1);
}
[[nodiscard]] constexpr inline BigInt operator-(BigInt &&first, BigInt &&second) {
  return std::move(first -= second);
}
[[nodiscard]] constexpr inline BigInt operator*(const BigInt &first, const BigInt &second) {
  if (first == 0 || second == 0) {
    return BigInt{};
  }
  BigInt copy(first);
  return std::move(copy *= second);
}
[[nodiscard]] constexpr inline BigInt operator*(BigInt &&first, const BigInt &second) {
  return std::move(first *= second);
}
[[nodiscard]] constexpr inline BigInt operator*(const BigInt &first, BigInt &&second) {
  return std::move(second *= first);
}
[[nodiscard]] constexpr inline BigInt operator*(BigInt &&first, BigInt &&second) {
  return std::move(first *= second);
}
[[nodiscard]] constexpr inline BigInt operator/(const BigInt &first, const BigInt &second) {
  if (first.Abs() < second.Abs()) {
    return BigInt{};
  }
  if (second.Abs() == 0) [[unlikely]] {
    throw BigIntDivisionByZero{};
  }
  if (first.Abs() == second.Abs()) {
    return ((first > 0) != (second > 0)) ? BigInt(-1) : BigInt(1);
  }
  BigInt copy(first);
  return std::move(copy /= second);
}
[[nodiscard]] constexpr inline BigInt operator/(BigInt &&first, const BigInt &second) {
  return std::move(first /= second);
}
[[nodiscard]] constexpr inline BigInt operator/(const BigInt &first, BigInt &&second) {
  return first / second;
}
[[nodiscard]] constexpr inline BigInt operator/(BigInt &&first, BigInt &&second) {
  return std::move(first /= second);
}
[[nodiscard]] constexpr inline BigInt operator%(const BigInt &first, const BigInt &second) {
  if (second.Abs() == 0) [[unlikely]] {
    throw BigIntDivisionByZero{};
  }
  if (first.Abs() == second.Abs()) {
    return BigInt{};
  }
  BigInt copy(first);
  return std::move(copy %= second);
}
[[nodiscard]] constexpr inline BigInt operator%(BigInt &&first, const BigInt &second) {
  return std::move(first %= second);
}
[[nodiscard]] constexpr inline BigInt operator%(const BigInt &first, BigInt &&second) {
  return second % first;
}
[[nodiscard]] constexpr inline BigInt operator%(BigInt &&first, BigInt &&second) {
  return std::move(first %= second);
}

[[nodiscard]] constexpr inline BigInt operator+(const BigInt &first, const UBigInt &second) {
  BigInt copy(first);
  return std::move(copy += second);
}
[[nodiscard]] constexpr inline BigInt operator+(BigInt &&first, const UBigInt &second) {
  return std::move(first += second);
}
[[nodiscard]] constexpr inline BigInt operator+(const BigInt &first, UBigInt &&second) {
  if (first < 0 && first.Abs() > second) {
    return first + second;
  }
  return BigInt(std::move(second += first));
}
[[nodiscard]] constexpr inline BigInt operator+(BigInt &&first, UBigInt &&second) {
  return std::move(first += second);
}
[[nodiscard]] constexpr inline BigInt operator-(const BigInt &first, const UBigInt &second) {
  if (first == second) {
    return BigInt{};
  }
  BigInt copy(first);
  return std::move(copy -= second);
}
[[nodiscard]] constexpr inline BigInt operator-(BigInt &&first, const UBigInt &second) {
  return std::move(first -= second);
}
[[nodiscard]] constexpr inline BigInt operator-(const BigInt &first, UBigInt &&second) {
  if (first > 0 && first.Abs() > second) {
    return first - second;
  }
  return BigInt(std::move(second -= first)) * -1;
}
[[nodiscard]] constexpr inline BigInt operator-(BigInt &&first, UBigInt &&second) {
  return std::move(first -= second);
}
[[nodiscard]] constexpr inline BigInt operator*(const BigInt &first, const UBigInt &second) {
  if (first.Abs() == 0 || second == 0) {
    return BigInt{};
  }
  BigInt copy(first);
  return std::move(copy *= second);
}
[[nodiscard]] constexpr inline BigInt operator*(BigInt &&first, const UBigInt &second) {
  return std::move(first *= second);
}
[[nodiscard]] constexpr inline BigInt operator*(const BigInt &first, UBigInt &&second) {
  return BigInt(std::move(second *= first.Abs())) * ((first > 0) ? 1 : -1);
}
[[nodiscard]] constexpr inline BigInt operator*(BigInt &&first, UBigInt &&second) {
  return std::move(first *= second);
}
[[nodiscard]] constexpr inline BigInt operator/(const BigInt &first, const UBigInt &second) {
  if (second == 0) {
    throw BigIntDivisionByZero{};
  }
  if (first.Abs() < second) {
    return BigInt{};
  }
  if (first.Abs() == second) {
    return first > 0 ? BigInt(1) : BigInt(-1);
  }
  BigInt copy(first);
  return std::move(copy /= second);
}
[[nodiscard]] constexpr inline BigInt operator/(BigInt &&first, const UBigInt &second) {
  return std::move(first /= second);
}
[[nodiscard]] constexpr inline BigInt operator/(const BigInt &first, UBigInt &&second) {
  return first / second;
}
[[nodiscard]] constexpr inline BigInt operator/(BigInt &&first, UBigInt &&second) {
  return std::move(first /= second);
}
[[nodiscard]] constexpr inline BigInt operator%(const BigInt &first, const UBigInt &second) {
  if (second == 0) {
    throw BigIntDivisionByZero{};
  }
  if (first.Abs() == second) {
    return BigInt{};
  }
  BigInt copy(first);
  return std::move(copy %= second);
}
[[nodiscard]] constexpr inline BigInt operator%(BigInt &&first, const UBigInt &second) {
  return std::move(first %= second);
}
[[nodiscard]] constexpr inline BigInt operator%(const BigInt &first, UBigInt &&second) {
  return first % second;
}
[[nodiscard]] constexpr inline BigInt operator%(BigInt &&first, UBigInt &&second) {
  return std::move(first %= second);
}

[[nodiscard]] constexpr inline BigInt operator+(const UBigInt &first, const BigInt &second) {
  BigInt copy(second);
  return std::move(copy += first);
}
[[nodiscard]] constexpr inline BigInt operator+(UBigInt &&first, const BigInt &second) {
  return BigInt(std::move(first)) + second;
}
[[nodiscard]] constexpr inline BigInt operator+(const UBigInt &first, BigInt &&second) {
  return std::move(second += first);
}
[[nodiscard]] constexpr inline BigInt operator+(UBigInt &&first, BigInt &&second) {
  return std::move(second += first);
}
[[nodiscard]] constexpr inline BigInt operator-(const UBigInt &first, const BigInt &second) {
  BigInt copy(second);
  return std::move((copy -= first) *= -1);
}
[[nodiscard]] constexpr inline BigInt operator-(UBigInt &&first, const BigInt &second) {
  return BigInt(std::move(first)) - second;
}
[[nodiscard]] constexpr inline BigInt operator-(const UBigInt &first, BigInt &&second) {
  return std::move((second -= first) *= -1);
}
[[nodiscard]] constexpr inline BigInt operator-(UBigInt &&first, BigInt &&second) {
  return std::move((second -= first) *= -1);
}
[[nodiscard]] constexpr inline BigInt operator*(const UBigInt &first, const BigInt &second) {
  if (first == 0 || second == 0) {
    return BigInt{};
  }
  BigInt copy(first);
  return std::move(copy *= second);
}
[[nodiscard]] constexpr inline BigInt operator*(UBigInt &&first, const BigInt &second) {
  return BigInt(std::move(first)) * second;
}
[[nodiscard]] constexpr inline BigInt operator*(const UBigInt &first, BigInt &&second) {
  return std::move(second *= first);
}
[[nodiscard]] constexpr inline BigInt operator*(UBigInt &&first, BigInt &&second) {
  return std::move(second *= first);
}
[[nodiscard]] constexpr inline BigInt operator/(const UBigInt &first, const BigInt &second) {
  if (second.Abs() == 0) {
    throw BigIntDivisionByZero{};
  }
  if (first < second.Abs()) {
    return BigInt{};
  }
  if (first == second.Abs()) {
    return BigInt(1) * (second > 0 ? 1 : -1);
  }
  BigInt copy(first);
  return std::move(copy /= second);
}
[[nodiscard]] constexpr inline BigInt operator/(UBigInt &&first, const BigInt &second) {
  return BigInt(std::move(first)) / second;
}
[[nodiscard]] constexpr inline BigInt operator/(const UBigInt &first, BigInt &&second) {
  return first / second;
}
[[nodiscard]] constexpr inline BigInt operator/(UBigInt &&first, BigInt &&second) {
  return BigInt(std::move(first)) / second;
}
[[nodiscard]] constexpr inline BigInt operator%(const UBigInt &first, const BigInt &second) {
  if (second.Abs() == 0) {
    throw BigIntDivisionByZero{};
  }
  if (first == second.Abs()) {
    return BigInt{};
  }
  BigInt copy(first);
  return std::move(copy %= second);
}
[[nodiscard]] constexpr inline BigInt operator%(UBigInt &&first, const BigInt &second) {
  return BigInt(std::move(first)) % second;
}
[[nodiscard]] constexpr inline BigInt operator%(const UBigInt &first, BigInt &&second) {
  return first % second;
}
[[nodiscard]] constexpr inline BigInt operator%(UBigInt &&first, BigInt &&second) {
  return BigInt(std::move(first)) % second;
}

constexpr inline BigInt &operator+=(BigInt &first, const UBigInt &second) {
  if (first.pos_) {
    first.abs_ += second;
    return first;
  }
  if (second > first.abs_) {
    first.pos_ = true;
    first.abs_ = second - first.abs_;
    return first;
  }
  first.abs_ -= second;
  return first;
}
constexpr inline BigInt &operator-=(BigInt &first, const UBigInt &second) {
  if (!first.pos_) {
    first.abs_ += second;
    return first;
  }
  if (second > first.abs_) {
    first.pos_ = false;
    first.abs_ = second - first.abs_;
    return first;
  }
  first.abs_ -= second;
  return first;
}
constexpr inline BigInt &operator*=(BigInt &first, const UBigInt &second) {
  first.abs_ *= second;
  return first;
}
constexpr inline BigInt &operator/=(BigInt &first, const UBigInt &second) {
  first.abs_ /= second;
  return first;
}
constexpr inline BigInt &operator%=(BigInt &first, const UBigInt &second) {
  first.abs_ %= second;
  return first;
}

constexpr inline BigInt &operator+=(BigInt &bigint, uint64_t number) {
  if (bigint.pos_) {
    bigint.abs_ += number;
  } else if (bigint.abs_ >= static_cast<uint64_t>(number)) {
    bigint.abs_ -= number;
  } else {
    bigint.pos_ = true;
    bigint.abs_ = UBigInt(number) - bigint.abs_;
  }
  return bigint;
}
constexpr inline BigInt &operator-=(BigInt &bigint, uint64_t number) {
  if (!bigint.pos_) {
    bigint.abs_ += number;
  } else if (bigint.abs_ >= number) {
    bigint.abs_ -= number;
  } else {
    bigint.pos_ = false;
    bigint.abs_ = UBigInt(number) - bigint.abs_;
  }
  return bigint;
}
constexpr inline BigInt &operator*=(BigInt &bigint, uint64_t number) {
  bigint.abs_ *= number;
  return bigint;
}
constexpr inline BigInt &operator/=(BigInt &bigint, uint64_t number) {
  bigint.abs_ /= number;
  return bigint;
}
constexpr inline BigInt &operator%=(BigInt &bigint, uint64_t number) {
  bigint.abs_ %= number;
  return bigint;
}
