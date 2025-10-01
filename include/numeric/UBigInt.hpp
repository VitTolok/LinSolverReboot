#pragma once
#include "definitions.hpp"
#include <deque>
#include <iostream>
#include <cstdint>
#include <limits>

uint64_t constexpr operator""_uint64(unsigned long long number) {
  return static_cast<uint64_t>(number);
}

class UBigIntUnderflow : public MachineError {
 public:
  [[nodiscard]] const char *what() const noexcept override {
    return message_.c_str();
  }

 private:
  std::string message_ = "The unsigned big integer cannot go negative";
};

class BigIntInvalidString : public MachineError {
 public:
  [[nodiscard]] const char *what() const noexcept override {
    return message_.c_str();
  }

 private:
  std::string message_ = "Use either full integers or '1.01612e288' form";
};

class BigIntDivisionByZero : public MachineError {
 public:
  [[nodiscard]] const char *what() const noexcept override {
    return message_.c_str();
  }

 private:
  std::string message_ = "Can't divide by 0";
};

class UBigInt {
 public:
  UBigInt();
  explicit UBigInt(uint64_t);
  template <std::integral Sub64Int>
  explicit UBigInt(Sub64Int integer) : UBigInt() {
    if (integer < 0) {
      throw UBigIntUnderflow{};
    }
    *this = static_cast<uint64_t>(integer);
  }
  UBigInt(const UBigInt &) = default;
  UBigInt(UBigInt &&) = default;

  UBigInt &operator=(const UBigInt &) = default;
  UBigInt &operator=(UBigInt &&) = default;
  UBigInt &operator=(uint64_t);
  template <std::integral Sub64Int>
  UBigInt &operator=(Sub64Int integer) {
    if (integer < 0) {
      throw UBigIntUnderflow{};
    }
    return *this = static_cast<uint64_t>(integer);
  }

  friend UBigInt &operator+=(UBigInt &, const UBigInt &);
  friend UBigInt &operator-=(UBigInt &, const UBigInt &);
  friend UBigInt &operator*=(UBigInt &, const UBigInt &);
  friend UBigInt &operator/=(UBigInt &, const UBigInt &);
  friend UBigInt &operator%=(UBigInt &, const UBigInt &);
  friend UBigInt &operator+=(UBigInt &, uint64_t);
  friend UBigInt &operator-=(UBigInt &, uint64_t);
  friend UBigInt &operator*=(UBigInt &, uint64_t);
  friend UBigInt &operator/=(UBigInt &, uint64_t);
  friend UBigInt &operator%=(UBigInt &, uint64_t);

  friend bool operator<(const UBigInt &, const UBigInt &);
  friend bool operator==(const UBigInt &, const UBigInt &);

  friend bool operator<(const UBigInt &, uint64_t);
  friend bool operator==(const UBigInt &, uint64_t);
  friend bool operator>(const UBigInt &, uint64_t);

  friend UBigInt &operator<<=(UBigInt &, size_t);
  friend UBigInt &operator>>=(UBigInt &, size_t);

  explicit operator uint64_t() const;
  explicit operator uint32_t() const;

  friend UBigInt Power(const UBigInt &, size_t);
  friend UBigInt GCD(const UBigInt &, const UBigInt &);

  friend std::ostream &operator<<(std::ostream &, const UBigInt &);
  friend std::istream &operator>>(std::istream &, UBigInt &);

  struct Approximation {
    long double multiplier;
    size_t power;
  };
  Approximation GetApproximation() const;
  std::string Exact() const;

  ~UBigInt() = default;

 private:
  std::deque<uint32_t> storage_;
  void Shrink() noexcept;
};

// operator overloads

inline UBigInt operator+(const UBigInt &first, const UBigInt &second) {
  UBigInt copy(first);
  return std::move(copy += second);
}
inline UBigInt operator+(UBigInt &&first, const UBigInt &second) {
  return std::move(first += second);
}
inline UBigInt operator+(const UBigInt &first, UBigInt &&second) {
  return std::move(second += first);
}
inline UBigInt operator+(UBigInt &&first, UBigInt &&second) {
  return std::move(first += second);
}
inline UBigInt operator-(const UBigInt &first, const UBigInt &second) {
  if (first == second) {
    return UBigInt(0);
  }
  UBigInt copy(first);
  return std::move(copy -= second);
}
inline UBigInt operator-(UBigInt &&first, const UBigInt &second) {
  return std::move(first -= second);
}
inline UBigInt operator-(UBigInt &&first, UBigInt &&second) {
  return std::move(first -= second);
}
inline UBigInt operator*(const UBigInt &first, const UBigInt &second) {
  UBigInt copy(first);
  return std::move(copy *= second);
}
inline UBigInt operator*(UBigInt &&first, const UBigInt &second) {
  return std::move(first *= second);
}
inline UBigInt operator*(UBigInt &&first, UBigInt &&second) {
  return std::move(first *= second);
}
inline UBigInt operator*(const UBigInt &first, UBigInt &&second) {
  return std::move(second *= first);
}
inline UBigInt operator/(const UBigInt &first, const UBigInt &second) {
  if (first == second) {
    return UBigInt(1);
  }
  UBigInt copy(first);
  return std::move(copy /= second);
}
inline UBigInt operator/(UBigInt &&first, const UBigInt &second) {
  return std::move(first /= second);
}
inline UBigInt operator/(UBigInt &&first, UBigInt &&second) {
  return std::move(first /= second);
}
inline UBigInt operator%(const UBigInt &first, const UBigInt &second) {
  if (first == second) {
    return UBigInt(0);
  }
  UBigInt copy(first);
  return std::move(copy %= second);
}
inline UBigInt operator%(UBigInt &&first, const UBigInt &second) {
  return std::move(first %= second);
}
inline UBigInt operator%(UBigInt &&first, UBigInt &&second) {
  return std::move(first %= second);
}

inline bool operator>(const UBigInt &first, const UBigInt &second) {
  return second < first;
}
inline bool operator<=(const UBigInt &first, const UBigInt &second) {
  return !(second < first);
}
inline bool operator>=(const UBigInt &first, const UBigInt &second) {
  return !(first < second);
}
inline bool operator!=(const UBigInt &first, const UBigInt &second) {
  return !(first == second);
}

inline UBigInt operator<<(const UBigInt &bigint, size_t shift) {
  UBigInt copy(bigint);
  return std::move(copy <<= shift);
}
inline UBigInt operator<<(UBigInt &&bigint, size_t shift) {
  return std::move(bigint <<= shift);
}
inline UBigInt operator>>(const UBigInt &bigint, size_t shift) {
  UBigInt copy(bigint);
  return std::move(copy >>= shift);
}
inline UBigInt operator>>(UBigInt &&bigint, size_t shift) {
  return std::move(bigint >>= shift);
}

// Loads of convenience operator templates

template <std::integral Sub64Int>
inline UBigInt &operator+=(UBigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint -= static_cast<uint64_t>(1_uint64 << 63);
    }
    return bigint -= static_cast<uint64_t>(-static_cast<int64_t>(integer));
  }
  return bigint += static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
inline UBigInt &operator-=(UBigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint += static_cast<uint64_t>(1_uint64 << 63);
    }
    return bigint += static_cast<uint64_t>(-static_cast<int64_t>(integer));
  }
  return bigint -= static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
inline UBigInt &operator*=(UBigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    throw UBigIntUnderflow{};
  }
  return bigint *= static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
inline UBigInt &operator/=(UBigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    throw UBigIntUnderflow{};
  }
  return bigint /= static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
inline UBigInt &operator%=(UBigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    if (integer == std::numeric_limits<int64_t>::min()) {
      return bigint %= static_cast<uint64_t>(1_uint64 << 63);
    }
    return bigint %= static_cast<uint64_t>(-static_cast<int64_t>(integer));
  }
  return bigint %= static_cast<uint64_t>(integer);
}

template <std::integral Sub64Int>
inline UBigInt operator+(const UBigInt &bigint, Sub64Int integer) {
  UBigInt copy(bigint);
  return std::move(copy += integer);
}
template <std::integral Sub64Int>
inline UBigInt operator+(UBigInt &&bigint, Sub64Int integer) {
  return std::move(bigint += integer);
}
template <std::integral Sub64Int>
inline UBigInt operator-(const UBigInt &bigint, Sub64Int integer) {
  UBigInt copy(bigint);
  return std::move(copy -= integer);
}
template <std::integral Sub64Int>
inline UBigInt operator-(UBigInt &&bigint, Sub64Int integer) {
  return std::move(bigint -= integer);
}
template <std::integral Sub64Int>
inline UBigInt operator*(const UBigInt &bigint, Sub64Int integer) {
  UBigInt copy(bigint);
  return std::move(copy *= integer);
}
template <std::integral Sub64Int>
inline UBigInt operator*(UBigInt &&bigint, Sub64Int integer) {
  return std::move(bigint *= integer);
}
template <std::integral Sub64Int>
inline UBigInt operator/(const UBigInt &bigint, Sub64Int integer) {
  UBigInt copy(bigint);
  return std::move(copy /= integer);
}
template <std::integral Sub64Int>
inline UBigInt operator/(UBigInt &&bigint, Sub64Int integer) {
  return std::move(bigint /= integer);
}
template <std::integral Sub64Int>
inline UBigInt operator%(const UBigInt &bigint, Sub64Int integer) {
  UBigInt copy(bigint);
  return std::move(copy %= integer);
}
template <std::integral Sub64Int>
inline UBigInt operator%(UBigInt &&bigint, Sub64Int integer) {
  return std::move(bigint %= integer);
}

template <std::integral Sub64Int>
inline UBigInt operator+(Sub64Int integer, const UBigInt &bigint) {
  return bigint + integer;
}
template <std::integral Sub64Int>
inline UBigInt operator-(Sub64Int integer, const UBigInt &bigint) {
  UBigInt copy(integer);
  return std::move(copy -= bigint);
}
template <std::integral Sub64Int>
inline UBigInt operator*(Sub64Int integer, const UBigInt &bigint) {
  return bigint * integer;
}
template <std::integral Sub64Int>
inline UBigInt operator/(Sub64Int integer, const UBigInt &bigint) {
  UBigInt copy(integer);
  return std::move(copy /= bigint);
}
template <std::integral Sub64Int>
inline UBigInt operator%(Sub64Int integer, const UBigInt &bigint) {
  UBigInt copy(integer);
  return std::move(copy %= bigint);
}

template <std::integral Sub64Int>
inline bool operator<(const UBigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    return false;
  }
  return bigint < static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
inline bool operator>(const UBigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    return true;
  }
  return bigint > static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
inline bool operator<=(const UBigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    return false;
  }
  return !(bigint > static_cast<uint64_t>(integer));
}
template <std::integral Sub64Int>
inline bool operator>=(const UBigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    return true;
  }
  return !(bigint < static_cast<uint64_t>(integer));
}
template <std::integral Sub64Int>
inline bool operator==(const UBigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    return false;
  }
  return bigint == static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
inline bool operator!=(const UBigInt &bigint, Sub64Int integer) {
  if (integer < 0) {
    return true;
  }
  return !(bigint == static_cast<uint64_t>(integer));
}

template <std::integral Sub64Int>
inline bool operator<(Sub64Int integer, const UBigInt &bigint) {
  return bigint > integer;
}
template <std::integral Sub64Int>
inline bool operator>(Sub64Int integer, const UBigInt &bigint) {
  return bigint < integer;
}
template <std::integral Sub64Int>
inline bool operator<=(Sub64Int integer, const UBigInt &bigint) {
  return bigint >= integer;
}
template <std::integral Sub64Int>
inline bool operator>=(Sub64Int integer, const UBigInt &bigint) {
  return bigint <= integer;
}
template <std::integral Sub64Int>
inline bool operator==(Sub64Int integer, const UBigInt &bigint) {
  return bigint == integer;
}
template <std::integral Sub64Int>
inline bool operator!=(Sub64Int integer, const UBigInt &bigint) {
  return bigint != integer;
}