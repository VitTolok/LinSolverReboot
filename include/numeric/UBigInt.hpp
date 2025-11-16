#pragma once
#include "definitions.hpp"
#include <deque>
#include <iostream>
#include <cstdint>
#include <limits>
#include <compare>

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
  UBigInt() : storage_() {
  }
  explicit UBigInt(uint64_t number) : storage_() {
    if (number > 0) {
      storage_.resize(2, 0);
      storage_[0] = static_cast<uint32_t>(number % (1_uint64 << 32));
      storage_[1] = static_cast<uint32_t>(number >> 32);
      Shrink();
    }
  }
  template <std::integral Sub64Int>
  explicit UBigInt(Sub64Int integer) : UBigInt() {
    if (integer < 0) {
      throw UBigIntUnderflow{};
    }
    *this = static_cast<uint64_t>(integer);
  }
  UBigInt(const UBigInt &) = default;
  UBigInt(UBigInt &&) noexcept = default;

  UBigInt &operator=(const UBigInt &) = default;
  UBigInt &operator=(UBigInt &&) noexcept = default;
  inline UBigInt &operator=(uint64_t number) {
    storage_.clear();
    if (number > 0) {
      storage_.resize(2, 0);
      storage_[0] = static_cast<uint32_t>(number);
      storage_[1] = static_cast<uint32_t>(number >> 32);
      Shrink();
    }
    return *this;
  }
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

  friend std::strong_ordering operator<=>(const UBigInt &, const UBigInt &) noexcept;
  inline friend bool operator==(const UBigInt &, const UBigInt &) noexcept;

  inline friend std::strong_ordering operator<=>(const UBigInt &, uint64_t) noexcept;
  inline friend bool operator==(const UBigInt &, uint64_t) noexcept;

  friend UBigInt &operator<<=(UBigInt &, size_t);
  friend UBigInt &operator>>=(UBigInt &, size_t) noexcept;

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
  Approximation GetApproximation() const noexcept;
  std::string Exact() const;

  ~UBigInt() = default;

 private:
  std::deque<uint32_t> storage_;

  void Shrink() noexcept {
    while (!storage_.empty() && storage_.back() == 0u) {
      storage_.pop_back();
    }
    storage_.shrink_to_fit();
  }
};

inline bool operator==(const UBigInt &first, const UBigInt &second) noexcept {
  return (first <=> second) == std::strong_ordering::equal;
}
inline std::strong_ordering operator<=>(const UBigInt &bigint, uint64_t number) noexcept {
  if (bigint.storage_.size() > 2) {
    return std::strong_ordering::greater;
  }
  return static_cast<uint64_t>(bigint) <=> number;
}
inline bool operator==(const UBigInt &bigint, uint64_t number) noexcept {
  return (bigint <=> number) == std::strong_ordering::equal;
}

inline UBigInt::operator uint64_t() const {
  if (storage_.size() == 0) {
    return 0;
  }
  if (storage_.size() == 1) {
    return storage_[0];
  }
  return (static_cast<uint64_t>(storage_[1]) << 32) + static_cast<uint64_t>(storage_[0]);
}

inline UBigInt::operator uint32_t() const {
  if (storage_.size() == 0) {
    return 0;
  }
  return storage_[0];
}
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
    return UBigInt();
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
  if (first < second) {
    return UBigInt();
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
inline std::strong_ordering operator<=>(const UBigInt &bigint, Sub64Int integer) noexcept {
  if (integer < 0) {
    return std::strong_ordering::greater;
  }
  return bigint <=> static_cast<uint64_t>(integer);
}
template <std::integral Sub64Int>
inline bool operator==(const UBigInt &bigint, Sub64Int integer) noexcept {
  return (bigint <=> static_cast<uint64_t>(integer)) == std::strong_ordering::equal;
}

template <std::integral Sub64Int>
inline std::strong_ordering operator<=>(Sub64Int integer, const UBigInt &bigint) noexcept {
  return 0 <=> (bigint <=> integer);
}
template <std::integral Sub64Int>
inline bool operator==(Sub64Int integer, const UBigInt &bigint) noexcept {
  return bigint == integer;
}

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
  if (bigint > integer) {
    throw UBigIntUnderflow{};
  }
  if (bigint == integer) {
    return UBigInt();
  }
  UBigInt copy(integer);
  return std::move(copy -= bigint);
}
template <std::integral Sub64Int>
inline UBigInt operator*(Sub64Int integer, const UBigInt &bigint) {
  return bigint * integer;
}
template <std::integral Sub64Int>
inline UBigInt operator/(Sub64Int integer, const UBigInt &bigint) {
  if (bigint > integer) {
    return UBigInt();
  }
  UBigInt copy(integer);
  return std::move(copy /= bigint);
}
template <std::integral Sub64Int>
inline UBigInt operator%(Sub64Int integer, const UBigInt &bigint) {
  UBigInt copy(integer);
  return std::move(copy %= bigint);
}
