#pragma once
#include "UBigInt.hpp"

class BigInt {
 public:
  BigInt();
  template <std::integral Sub64Int>
  explicit BigInt(Sub64Int integer) : BigInt() {
    *this = integer;
  }
  BigInt(const BigInt &) = default;
  BigInt(BigInt &&) = default;
  explicit BigInt(const UBigInt &bigint) : pos_(true), abs_(bigint) {
  }
  explicit BigInt(UBigInt &&bigint) : pos_(true), abs_(std::move(bigint)) {
  }

  BigInt &operator=(const BigInt &) = default;
  BigInt &operator=(BigInt &&) = default;
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

  friend BigInt &operator+=(BigInt &, const UBigInt &);
  friend BigInt &operator-=(BigInt &, const UBigInt &);
  friend BigInt &operator*=(BigInt &, const UBigInt &);
  friend BigInt &operator/=(BigInt &, const UBigInt &);
  friend BigInt &operator%=(BigInt &, const UBigInt &);
  friend BigInt &operator+=(BigInt &, uint64_t);
  friend BigInt &operator-=(BigInt &, uint64_t);
  friend BigInt &operator*=(BigInt &, uint64_t);
  friend BigInt &operator/=(BigInt &, uint64_t);
  friend BigInt &operator%=(BigInt &, uint64_t);
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
  friend inline BigInt operator-(BigInt &&);

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

  friend bool operator<(const BigInt &, const UBigInt &);
  friend bool operator>(const BigInt &, const UBigInt &);
  friend bool operator<=(const BigInt &, const UBigInt &);
  friend bool operator>=(const BigInt &, const UBigInt &);
  friend bool operator==(const BigInt &, const UBigInt &);
  friend bool operator!=(const BigInt &, const UBigInt &);

  friend bool operator<(const BigInt &, const BigInt &);
  friend bool operator>(const BigInt &, const BigInt &);
  friend bool operator<=(const BigInt &, const BigInt &);
  friend bool operator>=(const BigInt &, const BigInt &);
  friend bool operator==(const BigInt &, const BigInt &);
  friend bool operator!=(const BigInt &, const BigInt &);
  friend bool operator<(const UBigInt &, const BigInt &);
  friend bool operator>(const UBigInt &, const BigInt &);
  friend bool operator<=(const UBigInt &, const BigInt &);
  friend bool operator>=(const UBigInt &, const BigInt &);
  friend bool operator==(const UBigInt &, const BigInt &);
  friend bool operator!=(const UBigInt &, const BigInt &);

  friend BigInt &operator<<=(BigInt &, size_t);
  friend BigInt &operator>>=(BigInt &, size_t);
  friend BigInt operator<<(const BigInt &, size_t);
  friend BigInt operator>>(const BigInt &, size_t);

  explicit operator int64_t() const;
  const UBigInt &Abs() const noexcept {
    return abs_;
  }
  UBigInt &Abs() noexcept {
    return abs_;
  }

  friend std::ostream &operator<<(std::ostream &, const BigInt &);
  friend std::istream &operator>>(std::istream &, BigInt &);
  std::string Exact() const;

  using Approximation = UBigInt::Approximation;
  Approximation GetApproximation() const;
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
  friend inline bool operator<(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline bool operator>(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline bool operator<=(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline bool operator>=(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline bool operator==(const BigInt &, Sub64Int);
  template <std::integral Sub64Int>
  friend inline bool operator!=(const BigInt &, Sub64Int);

  template <std::integral Sub64Int>
  friend inline bool operator<(Sub64Int, const BigInt &);
  template <std::integral Sub64Int>
  friend inline bool operator>(Sub64Int, const BigInt &);
  template <std::integral Sub64Int>
  friend inline bool operator<=(Sub64Int, const BigInt &);
  template <std::integral Sub64Int>
  friend inline bool operator>=(Sub64Int, const BigInt &);
  template <std::integral Sub64Int>
  friend inline bool operator==(Sub64Int, const BigInt &);
  template <std::integral Sub64Int>
  friend inline bool operator!=(Sub64Int, const BigInt &);
};

// inline function definitions

inline BigInt operator-(const BigInt &bigint) {
  BigInt copy(bigint);
  copy.pos_ = !copy.pos_;
  return std::move(copy);
}
inline BigInt operator-(BigInt &&bigint) {
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
inline bool operator<(const BigInt &bigint, Sub64Int integer) {
  if (!bigint.pos_ && bigint.Abs() > 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint.Abs() > (1_uint64 << 63);
    }
    return bigint.Abs() > -integer;
  }
  return bigint.Abs() < integer;
}
template <std::integral Sub64Int>
inline bool operator>(const BigInt &bigint, Sub64Int integer) {
  return integer < bigint;
}
template <std::integral Sub64Int>
inline bool operator<=(const BigInt &bigint, Sub64Int integer) {
  return !(integer < bigint);
}
template <std::integral Sub64Int>
inline bool operator>=(const BigInt &bigint, Sub64Int integer) {
  return !(bigint < integer);
}
template <std::integral Sub64Int>
inline bool operator==(const BigInt &bigint, Sub64Int integer) {
  if (integer == std::numeric_limits<int64_t>::min()) {
    return bigint.Abs() == (1_uint64 << 63);
  }
  if (integer < 0) {
    return bigint.Abs() == -integer;
  }
  return bigint.Abs() == integer;
}
template <std::integral Sub64Int>
inline bool operator!=(const BigInt &bigint, Sub64Int integer) {
  return !(bigint == integer);
}

template <std::integral Sub64Int>
inline bool operator<(Sub64Int integer, const BigInt &bigint) {
  if (bigint < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint.Abs() < (1_uint64 << 63);
    }
    return bigint.Abs() < -integer;
  }
  return bigint.Abs() > integer;
}
template <std::integral Sub64Int>
inline bool operator>(Sub64Int integer, const BigInt &bigint) {
  return bigint < integer;
}
template <std::integral Sub64Int>
inline bool operator<=(Sub64Int integer, const BigInt &bigint) {
  return !(bigint < integer);
}
template <std::integral Sub64Int>
inline bool operator>=(Sub64Int integer, const BigInt &bigint) {
  return !(integer < bigint);
}
template <std::integral Sub64Int>
inline bool operator==(Sub64Int integer, const BigInt &bigint) {
  return bigint == integer;
}
template <std::integral Sub64Int>
inline bool operator!=(Sub64Int integer, const BigInt &bigint) {
  return bigint != integer;
}

inline BigInt operator<<(BigInt &&bigint, size_t shift) {
  return std::move(bigint <<= shift);
}
inline BigInt operator>>(BigInt &&bigint, size_t shift) {
  return std::move(bigint >>= shift);
}