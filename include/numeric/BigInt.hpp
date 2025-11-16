#pragma once
#include "UBigInt.hpp"
#include <sstream>

class BigInt {
 public:
  BigInt() : pos_(false), abs_() {
  }
  template <std::integral Sub64Int>
  explicit BigInt(Sub64Int integer) : BigInt() {
    *this = integer;
  }
  BigInt(const BigInt &) = default;
  BigInt(BigInt &&) noexcept = default;
  explicit BigInt(const UBigInt &bigint) : pos_(true), abs_(bigint) {
  }
  explicit BigInt(UBigInt &&bigint) noexcept : pos_(true), abs_(std::move(bigint)) {
  }

  BigInt &operator=(const BigInt &) = default;
  BigInt &operator=(BigInt &&) noexcept = default;
  template <std::integral Sub64Int>
  BigInt &operator=(Sub64Int integer) {
    if (integer < 0) {
      pos_ = false;
      if (integer == std::numeric_limits<int64_t>::min()) {
        abs_ = static_cast<uint64_t>(1_uint64 << 63);
      } else {
        abs_ = static_cast<uint64_t>(-static_cast<int64_t>(integer));
      }
      return *this;
    }
    pos_ = true;
    abs_ = static_cast<uint64_t>(integer);
    return *this;
  }

  friend inline BigInt &operator+=(BigInt &, const UBigInt &);
  friend inline BigInt &operator-=(BigInt &, const UBigInt &);
  friend inline BigInt &operator*=(BigInt &, const UBigInt &);
  friend inline BigInt &operator/=(BigInt &, const UBigInt &);
  friend inline BigInt &operator%=(BigInt &, const UBigInt &);
  friend inline BigInt &operator+=(BigInt &, uint64_t);
  friend inline BigInt &operator-=(BigInt &, uint64_t);
  friend inline BigInt &operator*=(BigInt &, uint64_t);
  friend inline BigInt &operator/=(BigInt &, uint64_t);
  friend inline BigInt &operator%=(BigInt &, uint64_t);
  friend inline BigInt &operator+=(BigInt &, const BigInt &);
  friend inline BigInt &operator-=(BigInt &, const BigInt &);
  friend inline BigInt &operator*=(BigInt &, const BigInt &);
  friend inline BigInt &operator/=(BigInt &, const BigInt &);
  friend inline BigInt &operator%=(BigInt &, const BigInt &);
  friend inline UBigInt &operator+=(UBigInt &, const BigInt &);
  friend inline UBigInt &operator-=(UBigInt &, const BigInt &);
  friend inline UBigInt &operator*=(UBigInt &, const BigInt &);
  friend inline UBigInt &operator/=(UBigInt &, const BigInt &);
  friend inline UBigInt &operator%=(UBigInt &, const BigInt &);

  friend inline BigInt operator-(const BigInt &);
  friend inline BigInt operator-(BigInt &&) noexcept;

  friend inline BigInt operator+(const BigInt &, const BigInt &);
  friend inline BigInt operator+(BigInt &&, const BigInt &);
  friend inline BigInt operator+(const BigInt &, BigInt &&);
  friend inline BigInt operator+(BigInt &&, BigInt &&);
  friend inline BigInt operator-(const BigInt &, const BigInt &);
  friend inline BigInt operator-(BigInt &&, const BigInt &);
  friend inline BigInt operator-(const BigInt &, BigInt &&);
  friend inline BigInt operator-(BigInt &&, BigInt &&);
  friend inline BigInt operator*(const BigInt &, const BigInt &);
  friend inline BigInt operator*(BigInt &&, const BigInt &);
  friend inline BigInt operator*(const BigInt &, BigInt &&);
  friend inline BigInt operator*(BigInt &&, BigInt &&);
  friend inline BigInt operator/(const BigInt &, const BigInt &);
  friend inline BigInt operator/(BigInt &&, const BigInt &);
  friend inline BigInt operator/(const BigInt &, BigInt &&);
  friend inline BigInt operator/(BigInt &&, BigInt &&);
  friend inline BigInt operator%(const BigInt &, const BigInt &);
  friend inline BigInt operator%(BigInt &&, const BigInt &);
  friend inline BigInt operator%(const BigInt &, BigInt &&);
  friend inline BigInt operator%(BigInt &&, BigInt &&);

  friend inline BigInt operator+(const BigInt &, const UBigInt &);
  friend inline BigInt operator+(BigInt &&, const UBigInt &);
  friend inline BigInt operator+(const BigInt &, UBigInt &&);
  friend inline BigInt operator+(BigInt &&, UBigInt &&);
  friend inline BigInt operator-(const BigInt &, const UBigInt &);
  friend inline BigInt operator-(BigInt &&, const UBigInt &);
  friend inline BigInt operator-(const BigInt &, UBigInt &&);
  friend inline BigInt operator-(BigInt &&, UBigInt &&);
  friend inline BigInt operator*(const BigInt &, const UBigInt &);
  friend inline BigInt operator*(BigInt &&, const UBigInt &);
  friend inline BigInt operator*(const BigInt &, UBigInt &&);
  friend inline BigInt operator*(BigInt &&, UBigInt &&);
  friend inline BigInt operator/(const BigInt &, const UBigInt &);
  friend inline BigInt operator/(BigInt &&, const UBigInt &);
  friend inline BigInt operator/(const BigInt &, UBigInt &&);
  friend inline BigInt operator/(BigInt &&, UBigInt &&);
  friend inline BigInt operator%(const BigInt &, const UBigInt &);
  friend inline BigInt operator%(BigInt &&, const UBigInt &);
  friend inline BigInt operator%(const BigInt &, UBigInt &&);
  friend inline BigInt operator%(BigInt &&, UBigInt &&);

  friend inline BigInt operator+(const UBigInt &, const BigInt &);
  friend inline BigInt operator+(UBigInt &&, const BigInt &);
  friend inline BigInt operator+(const UBigInt &, BigInt &&);
  friend inline BigInt operator+(UBigInt &&, BigInt &&);
  friend inline BigInt operator-(const UBigInt &, const BigInt &);
  friend inline BigInt operator-(UBigInt &&, const BigInt &);
  friend inline BigInt operator-(const UBigInt &, BigInt &&);
  friend inline BigInt operator-(UBigInt &&, BigInt &&);
  friend inline BigInt operator*(const UBigInt &, const BigInt &);
  friend inline BigInt operator*(UBigInt &&, const BigInt &);
  friend inline BigInt operator*(const UBigInt &, BigInt &&);
  friend inline BigInt operator*(UBigInt &&, BigInt &&);
  friend inline BigInt operator/(const UBigInt &, const BigInt &);
  friend inline BigInt operator/(UBigInt &&, const BigInt &);
  friend inline BigInt operator/(const UBigInt &, BigInt &&);
  friend inline BigInt operator/(UBigInt &&, BigInt &&);
  friend inline BigInt operator%(const UBigInt &, const BigInt &);
  friend inline BigInt operator%(UBigInt &&, const BigInt &);
  friend inline BigInt operator%(const UBigInt &, BigInt &&);
  friend inline BigInt operator%(UBigInt &&, BigInt &&);

  friend inline std::strong_ordering operator<=>(const BigInt &, const UBigInt &) noexcept;
  friend inline bool operator==(const BigInt &, const UBigInt &) noexcept;
  friend inline std::strong_ordering operator<=>(const BigInt &, const BigInt &) noexcept;
  friend inline bool operator==(const BigInt &, const BigInt &) noexcept;
  friend inline std::strong_ordering operator<=>(const UBigInt &, const BigInt &) noexcept;
  friend inline bool operator==(const UBigInt &, const BigInt &) noexcept;

  friend inline BigInt &operator<<=(BigInt &, size_t);
  friend inline BigInt &operator>>=(BigInt &, size_t) noexcept;
  friend inline BigInt operator<<(const BigInt &, size_t);
  friend inline BigInt operator>>(const BigInt &, size_t);
  friend inline BigInt operator<<(BigInt &&, size_t);
  friend inline BigInt operator>>(BigInt &&, size_t);

  explicit inline operator int64_t() const;
  const UBigInt &Abs() const & noexcept {
    return abs_;
  }
  UBigInt &Abs() & noexcept {
    return abs_;
  }
  const UBigInt &&Abs() const && noexcept {
    return std::move(abs_);
  }
  UBigInt &&Abs() && noexcept {
    return std::move(abs_);
  }

  friend inline std::ostream &operator<<(std::ostream &, const BigInt &);
  friend inline std::istream &operator>>(std::istream &, BigInt &);
  inline std::string Exact() const;

  using Approximation = UBigInt::Approximation;
  inline Approximation GetApproximation() const noexcept;
  ~BigInt() = default;

 private:
  UBigInt abs_;
  bool pos_;

 public:  // template declarations
  template <std::integral Sub64Int>
  friend inline BigInt &operator+=(BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt &operator-=(BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt &operator*=(BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt &operator/=(BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt &operator%=(BigInt &, Sub64Int);

  template <std::integral Sub64Int>
  friend inline BigInt operator+(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt operator+(BigInt &&, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt operator-(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt operator-(BigInt &&, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt operator*(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt operator*(BigInt &&, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt operator/(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt operator/(BigInt &&, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt operator%(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline BigInt operator%(BigInt &&, Sub64Int);

  template <std::integral Sub64Int>
  friend inline std::strong_ordering operator<=>(const BigInt &, Sub64Int) noexcept;
  template <std::integral Sub64Int>
  friend inline bool operator==(const BigInt &, Sub64Int) noexcept;

  template <std::integral Sub64Int>
  friend inline std::strong_ordering operator<=>(Sub64Int, const BigInt &) noexcept;
  template <std::integral Sub64Int>
  friend inline bool operator==(Sub64Int, const BigInt &) noexcept;
};

// inline function definitions

inline std::ostream &operator<<(std::ostream &out, const BigInt &bigint) {
  if (!bigint.pos_ && bigint.abs_ != 0) {
    out << '-';
  }
  return out << bigint.abs_;
}

inline std::istream &operator>>(std::istream &in, BigInt &bigint) {
  std::string input;
  in >> input;
  if (!input.empty() && input[0] == '-') {
    bigint.pos_ = false;
    input.erase(0, 1);
  } else {
    bigint.pos_ = true;
  }
  std::istringstream(std::move(input)) >> bigint.abs_;
  return in;
}

inline std::string BigInt::Exact() const {
  return ((pos_ || (abs_ == 0)) ? "" : "-") + abs_.Exact();
}

inline BigInt::Approximation BigInt::GetApproximation() const noexcept {
  auto approx = abs_.GetApproximation();
  if (!pos_ && abs_ > 0) {
    approx.multiplier *= -1;
  }
  return approx;
}

inline BigInt &operator<<=(BigInt &bigint, size_t shift) {
  bigint.abs_ <<= shift;
  return bigint;
}
inline BigInt &operator>>=(BigInt &bigint, size_t shift) noexcept {
  bigint.abs_ >>= shift;
  return bigint;
}
inline BigInt operator<<(const BigInt &bigint, size_t shift) {
  BigInt copy = bigint;
  copy.abs_ <<= shift;
  return std::move(copy);
}
inline BigInt operator>>(const BigInt &bigint, size_t shift) {
  BigInt copy = bigint;
  copy.abs_ >>= shift;
  return std::move(copy);
}
inline BigInt operator<<(BigInt &&bigint, size_t shift) {
  bigint.abs_ <<= shift;
  return std::move(bigint);
}
inline BigInt operator>>(BigInt &&bigint, size_t shift) {
  bigint.abs_ >>= shift;
  return std::move(bigint);
}

inline BigInt::operator int64_t() const {
  uint64_t modulus = static_cast<uint64_t>(abs_);
  if (modulus == (1ull << 63) && !pos_) {
    return std::numeric_limits<int64_t>::min();
  }
  modulus %= (1ull << 63);
  return static_cast<int64_t>(modulus) * (pos_ ? 1 : -1);
}

inline std::strong_ordering operator<=>(const BigInt &first, const BigInt &second) noexcept {
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
inline bool operator==(const BigInt &first, const BigInt &second) noexcept {
  return (first <=> second) == std::strong_ordering::equal;
}
inline std::strong_ordering operator<=>(const UBigInt &first, const BigInt &second) noexcept {
  if (second.abs_ == 0) {
    return first <=> 0;
  }
  if (!second.pos_) {
    return std::strong_ordering::greater;
  }
  return first <=> second.abs_;
}
inline bool operator==(const UBigInt &first, const BigInt &second) noexcept {
  return (first <=> second) == std::strong_ordering::equal;
}
inline std::strong_ordering operator<=>(const BigInt &first, const UBigInt &second) noexcept {
  return 0 <=> (second <=> first);
}
inline bool operator==(const BigInt &first, const UBigInt &second) noexcept {
  return second == first;
}

inline BigInt operator-(const BigInt &bigint) {
  BigInt copy(bigint);
  copy.pos_ = !copy.pos_;
  return std::move(copy);
}
inline BigInt operator-(BigInt &&bigint) noexcept {
  bigint.pos_ = !bigint.pos_;
  return std::move(bigint);
}

template <std::integral Sub64Int>
inline BigInt &operator+=(BigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint -= (1_uint64 << 63);
    }
    return bigint -= static_cast<uint64_t>(-static_cast<int64_t>(integer));
  }
  return bigint += static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
inline BigInt &operator-=(BigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint += (1_uint64 << 63);
    }
    return bigint += static_cast<uint64_t>(-static_cast<int64_t>(integer));
  }
  return bigint -= static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
inline BigInt &operator*=(BigInt &bigint, Sub64Int integer) {
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
inline BigInt &operator/=(BigInt &bigint, Sub64Int integer) {
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
inline BigInt &operator%=(BigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint %= (1_uint64 << 63);
    }
    return bigint %= static_cast<uint64_t>(-static_cast<int64_t>(integer));
  }
  return bigint %= static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
inline BigInt operator+(const BigInt &bigint, Sub64Int integer) {
  BigInt copy(bigint);
  return std::move(copy += integer);
}
template <std::integral Sub64Int>
inline BigInt operator+(BigInt &&bigint, Sub64Int integer) {
  return std::move(bigint += integer);
}
template <std::integral Sub64Int>
inline BigInt operator-(const BigInt &bigint, Sub64Int integer) {
  if (bigint == integer) {
    return BigInt();
  }
  BigInt copy(bigint);
  return std::move(copy -= integer);
}
template <std::integral Sub64Int>
inline BigInt operator-(BigInt &&bigint, Sub64Int integer) {
  return std::move(bigint -= integer);
}
template <std::integral Sub64Int>
inline BigInt operator*(const BigInt &bigint, Sub64Int integer) {
  BigInt copy(bigint);
  return std::move(copy *= integer);
}
template <std::integral Sub64Int>
inline BigInt operator*(BigInt &&bigint, Sub64Int integer) {
  return std::move(bigint *= integer);
}
template <std::integral Sub64Int>
inline BigInt operator/(const BigInt &bigint, Sub64Int integer) {
  if (integer == 0) {
    throw BigIntDivisionByZero{};
  }
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      if (bigint.Abs() < (1_uint64 << 63)) {
        return BigInt();
      }
      if (bigint.Abs() == (1_uint64 << 63)) {
        return BigInt(1) * ((bigint < 0) ? 1 : -1);
      }
    }
    if (bigint.Abs() < -integer) {
      return BigInt();
    }
    if (bigint.Abs() == -integer) {
      return BigInt(1) * ((bigint < 0) ? 1 : -1);
    }
  }
  if (bigint.Abs() < integer) {
    return BigInt();
  }
  if (bigint.Abs() == integer) {
    return BigInt(1) * ((bigint < 0) ? -1 : 1);
  }
  BigInt copy(bigint);
  return std::move(copy /= integer);
}
template <std::integral Sub64Int>
inline BigInt operator/(BigInt &&bigint, Sub64Int integer) {
  return std::move(bigint /= integer);
}
template <std::integral Sub64Int>
inline BigInt operator%(const BigInt &bigint, Sub64Int integer) {
  if (integer == 0) {
    throw BigIntDivisionByZero{};
  }
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      if (bigint.Abs() == (1_uint64 << 63)) {
        return BigInt();
      }
    }
    if (bigint.Abs() == -integer) {
      return BigInt();
    }
  }
  if (bigint.Abs() == integer) {
    return BigInt();
  }
  BigInt copy(bigint);
  return std::move(copy %= integer);
}
template <std::integral Sub64Int>
inline BigInt operator%(BigInt &&bigint, Sub64Int integer) {
  return std::move(bigint %= integer);
}

inline BigInt &operator+=(BigInt &first, const BigInt &second) {
  if (second < 0) {
    return first -= second.Abs();
  }
  return first += second.Abs();
}
inline BigInt &operator-=(BigInt &first, const BigInt &second) {
  if (second < 0) {
    return first += second.Abs();
  }
  return first -= second.Abs();
}
inline BigInt &operator*=(BigInt &first, const BigInt &second) {
  if (second < 0) {
    first *= -1;
  }
  return first *= second.Abs();
}
inline BigInt &operator/=(BigInt &first, const BigInt &second) {
  if (second < 0) {
    first *= -1;
  }
  return first /= second.Abs();
}
inline BigInt &operator%=(BigInt &first, const BigInt &second) {
  return first %= second.Abs();
}

inline UBigInt &operator+=(UBigInt &first, const BigInt &second) {
  if (second > 0) {
    return first += second.Abs();
  }
  return first -= second.Abs();
}
inline UBigInt &operator-=(UBigInt &first, const BigInt &second) {
  if (second > 0) {
    return first -= second.Abs();
  }
  return first += second.Abs();
}
inline UBigInt &operator*=(UBigInt &first, const BigInt &second) {
  if (second >= 0) {
    return first *= second.Abs();
  }
  throw UBigIntUnderflow{};
}
inline UBigInt &operator/=(UBigInt &first, const BigInt &second) {
  if (second >= 0) {
    return first /= second.Abs();
  }
  throw UBigIntUnderflow{};
}
inline UBigInt &operator%=(UBigInt &first, const BigInt &second) {
  return first %= second.Abs();
}

inline BigInt operator+(const BigInt &first, const BigInt &second) {
  BigInt copy(first);
  return std::move(copy += second);
}
inline BigInt operator+(BigInt &&first, const BigInt &second) {
  return std::move(first += second);
}
inline BigInt operator+(const BigInt &first, BigInt &&second) {
  return std::move(second += first);
}
inline BigInt operator+(BigInt &&first, BigInt &&second) {
  return std::move(first += second);
}
inline BigInt operator-(const BigInt &first, const BigInt &second) {
  if (first == second) {
    return BigInt();
  }
  BigInt copy(first);
  return std::move(copy -= second);
}
inline BigInt operator-(BigInt &&first, const BigInt &second) {
  return std::move(first -= second);
}
inline BigInt operator-(const BigInt &first, BigInt &&second) {
  return std::move((second -= first) *= -1);
}
inline BigInt operator-(BigInt &&first, BigInt &&second) {
  return std::move(first -= second);
}
inline BigInt operator*(const BigInt &first, const BigInt &second) {
  if (first == 0 || second == 0) {
    return BigInt();
  }
  BigInt copy(first);
  return std::move(copy *= second);
}
inline BigInt operator*(BigInt &&first, const BigInt &second) {
  return std::move(first *= second);
}
inline BigInt operator*(const BigInt &first, BigInt &&second) {
  return std::move(second *= first);
}
inline BigInt operator*(BigInt &&first, BigInt &&second) {
  return std::move(first *= second);
}
inline BigInt operator/(const BigInt &first, const BigInt &second) {
  if (first.Abs() < second.Abs()) {
    return BigInt();
  }
  if (second.Abs() == 0) {
    throw BigIntDivisionByZero{};
  }
  if (first.Abs() == second.Abs()) {
    return ((first > 0) != (second > 0)) ? BigInt(-1) : BigInt(1);
  }
  BigInt copy(first);
  return std::move(copy /= second);
}
inline BigInt operator/(BigInt &&first, const BigInt &second) {
  return std::move(first /= second);
}
inline BigInt operator/(const BigInt &first, BigInt &&second) {
  return first / second;
}
inline BigInt operator/(BigInt &&first, BigInt &&second) {
  return std::move(first /= second);
}
inline BigInt operator%(const BigInt &first, const BigInt &second) {
  if (second.Abs() == 0) {
    throw BigIntDivisionByZero{};
  }
  if (first.Abs() == second.Abs()) {
    return BigInt();
  }
  BigInt copy(first);
  return std::move(copy %= second);
}
inline BigInt operator%(BigInt &&first, const BigInt &second) {
  return std::move(first %= second);
}
inline BigInt operator%(const BigInt &first, BigInt &&second) {
  return std::move(second % first);
}
inline BigInt operator%(BigInt &&first, BigInt &&second) {
  return std::move(first %= second);
}

inline BigInt operator+(const BigInt &first, const UBigInt &second) {
  BigInt copy(first);
  return std::move(copy += second);
}
inline BigInt operator+(BigInt &&first, const UBigInt &second) {
  return std::move(first += second);
}
inline BigInt operator+(const BigInt &first, UBigInt &&second) {
  if (first < 0 && first.Abs() > second) {
    return first + second;
  }
  return BigInt(std::move(second += first));
}
inline BigInt operator+(BigInt &&first, UBigInt &&second) {
  return std::move(first += second);
}
inline BigInt operator-(const BigInt &first, const UBigInt &second) {
  if (first == second) {
    return BigInt();
  }
  BigInt copy(first);
  return std::move(copy -= second);
}
inline BigInt operator-(BigInt &&first, const UBigInt &second) {
  return std::move(first -= second);
}
inline BigInt operator-(const BigInt &first, UBigInt &&second) {
  if (first > 0 && first.Abs() > second) {
    return first - second;
  }
  return BigInt(std::move(second -= first)) * -1;
}
inline BigInt operator-(BigInt &&first, UBigInt &&second) {
  return std::move(first -= second);
}
inline BigInt operator*(const BigInt &first, const UBigInt &second) {
  if (first.Abs() == 0 || second == 0) {
    return BigInt();
  }
  BigInt copy(first);
  return std::move(copy *= second);
}
inline BigInt operator*(BigInt &&first, const UBigInt &second) {
  return std::move(first *= second);
}
inline BigInt operator*(const BigInt &first, UBigInt &&second) {
  return BigInt(std::move(second *= first.Abs())) * ((first > 0) ? 1 : -1);
}
inline BigInt operator*(BigInt &&first, UBigInt &&second) {
  return std::move(first *= second);
}
inline BigInt operator/(const BigInt &first, const UBigInt &second) {
  if (second == 0) {
    throw BigIntDivisionByZero{};
  }
  if (first.Abs() < second) {
    return BigInt();
  }
  if (first.Abs() == second) {
    return first > 0 ? BigInt(1) : BigInt(-1);
  }
  BigInt copy(first);
  return std::move(copy /= second);
}
inline BigInt operator/(BigInt &&first, const UBigInt &second) {
  return std::move(first /= second);
}
inline BigInt operator/(const BigInt &first, UBigInt &&second) {
  return first / second;
}
inline BigInt operator/(BigInt &&first, UBigInt &&second) {
  return std::move(first /= second);
}
inline BigInt operator%(const BigInt &first, const UBigInt &second) {
  if (second == 0) {
    throw BigIntDivisionByZero{};
  }
  if (first.Abs() == second) {
    return BigInt();
  }
  BigInt copy(first);
  return std::move(copy %= second);
}
inline BigInt operator%(BigInt &&first, const UBigInt &second) {
  return std::move(first %= second);
}
inline BigInt operator%(const BigInt &first, UBigInt &&second) {
  return first % second;
}
inline BigInt operator%(BigInt &&first, UBigInt &&second) {
  return std::move(first %= second);
}

inline BigInt operator+(const UBigInt &first, const BigInt &second) {
  BigInt copy(second);
  return std::move(copy += first);
}
inline BigInt operator+(UBigInt &&first, const BigInt &second) {
  return std::move(BigInt(std::move(first)) + second);
}
inline BigInt operator+(const UBigInt &first, BigInt &&second) {
  return std::move(second += first);
}
inline BigInt operator+(UBigInt &&first, BigInt &&second) {
  return std::move(second += first);
}
inline BigInt operator-(const UBigInt &first, const BigInt &second) {
  BigInt copy(second);
  return std::move((copy -= first) *= -1);
}
inline BigInt operator-(UBigInt &&first, const BigInt &second) {
  return std::move(BigInt(std::move(first)) - second);
}
inline BigInt operator-(const UBigInt &first, BigInt &&second) {
  return std::move((second -= first) *= -1);
}
inline BigInt operator-(UBigInt &&first, BigInt &&second) {
  return std::move((second -= first) *= -1);
}
inline BigInt operator*(const UBigInt &first, const BigInt &second) {
  if (first == 0 || second == 0) {
    return BigInt();
  }
  BigInt copy(first);
  return std::move(copy *= second);
}
inline BigInt operator*(UBigInt &&first, const BigInt &second) {
  return std::move(BigInt(std::move(first)) * second);
}
inline BigInt operator*(const UBigInt &first, BigInt &&second) {
  return std::move(second *= first);
}
inline BigInt operator*(UBigInt &&first, BigInt &&second) {
  return std::move(second *= first);
}
inline BigInt operator/(const UBigInt &first, const BigInt &second) {
  if (second.Abs() == 0) {
    throw BigIntDivisionByZero{};
  }
  if (first < second.Abs()) {
    return BigInt();
  }
  if (first == second.Abs()) {
    return BigInt(1) * (second > 0 ? 1 : -1);
  }
  BigInt copy(first);
  return std::move(copy /= second);
}
inline BigInt operator/(UBigInt &&first, const BigInt &second) {
  return std::move(BigInt(std::move(first)) / second);
}
inline BigInt operator/(const UBigInt &first, BigInt &&second) {
  return std::move(first / second);
}
inline BigInt operator/(UBigInt &&first, BigInt &&second) {
  return std::move(BigInt(std::move(first)) / second);
}
inline BigInt operator%(const UBigInt &first, const BigInt &second) {
  if (second.Abs() == 0) {
    throw BigIntDivisionByZero{};
  }
  if (first == second.Abs()) {
    return BigInt();
  }
  BigInt copy(first);
  return std::move(copy %= second);
}
inline BigInt operator%(UBigInt &&first, const BigInt &second) {
  return std::move(BigInt(std::move(first)) % second);
}
inline BigInt operator%(const UBigInt &first, BigInt &&second) {
  return std::move(first % second);
}
inline BigInt operator%(UBigInt &&first, BigInt &&second) {
  return std::move(BigInt(std::move(first)) % second);
}

template <std::integral Sub64Int>
inline std::strong_ordering operator<=>(const BigInt &bigint, Sub64Int integer) noexcept {
  if (!bigint.pos_ && bigint.Abs() > 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      return 0 <=> (bigint.Abs() <=> (1_uint64 << 63));
    }
    return 0 <=> (bigint.Abs() <=> -integer);
  }
  return bigint.Abs() <=> integer;
}
template <std::integral Sub64Int>
inline bool operator==(const BigInt &bigint, Sub64Int integer) noexcept {
  return (bigint <=> integer) == std::strong_ordering::equal;
}

template <std::integral Sub64Int>
inline std::strong_ordering operator<=>(Sub64Int integer, const BigInt &bigint) noexcept {
  return 0 <=> (bigint <=> integer);
}
template <std::integral Sub64Int>
inline bool operator==(Sub64Int integer, const BigInt &bigint) noexcept {
  return bigint == integer;
}

inline BigInt &operator+=(BigInt &first, const UBigInt &second) {
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
inline BigInt &operator-=(BigInt &first, const UBigInt &second) {
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
inline BigInt &operator*=(BigInt &first, const UBigInt &second) {
  first.abs_ *= second;
  return first;
}
inline BigInt &operator/=(BigInt &first, const UBigInt &second) {
  first.abs_ /= second;
  return first;
}
inline BigInt &operator%=(BigInt &first, const UBigInt &second) {
  first.abs_ %= second;
  return first;
}

inline BigInt &operator+=(BigInt &bigint, uint64_t number) {
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
inline BigInt &operator-=(BigInt &bigint, uint64_t number) {
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
inline BigInt &operator*=(BigInt &bigint, uint64_t number) {
  bigint.abs_ *= number;
  return bigint;
}
inline BigInt &operator/=(BigInt &bigint, uint64_t number) {
  bigint.abs_ /= number;
  return bigint;
}
inline BigInt &operator%=(BigInt &bigint, uint64_t number) {
  bigint.abs_ %= number;
  return bigint;
}
