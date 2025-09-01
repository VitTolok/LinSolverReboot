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
    }
    pos_ = true;
    abs_ = static_cast<uint64_t>(integer);
  }

  friend BigInt &operator+=(BigInt &, const UBigInt &);
  friend BigInt &operator-=(BigInt &, const UBigInt &);
  friend BigInt &operator*=(BigInt &, const UBigInt &);
  friend BigInt &operator/=(BigInt &, const UBigInt &);
  friend BigInt &operator%=(BigInt &, const UBigInt &);
  friend BigInt &operator+=(BigInt &first, const BigInt &second) {
    if (second < 0) {
      return first -= second.abs_;
    }
    return first += second.abs_;
  }
  friend BigInt &operator-=(BigInt &first, const BigInt &second) {
    if (second < 0) {
      return first += second.abs_;
    }
    return first -= second.abs_;
  }
  friend BigInt &operator*=(BigInt &first, const BigInt &second) {
    if (second < 0) {
      first.pos_ = !first.pos_;
    }
    return first *= second.abs_;
  }
  friend BigInt &operator/=(BigInt &first, const BigInt &second) {
    if (second < 0) {
      first.pos_ = !first.pos_;
    }
    return first /= second.abs_;
  }
  friend BigInt &operator%=(BigInt &first, const BigInt &second) {
    return first %= second.abs_;
  }
  friend UBigInt &operator+=(UBigInt &first, const BigInt &second) {
    if (second.pos_) {
      return first += second.abs_;
    }
    return first -= second.abs_;
  }
  friend UBigInt &operator-=(UBigInt &first, const BigInt &second) {
    if (second.pos_) {
      return first -= second.abs_;
    }
    return first += second.abs_;
  }
  friend UBigInt &operator*=(UBigInt &first, const BigInt &second) {
    if (second >= 0) {
      return first *= second.abs_;
    }
    throw UBigIntUnderflow{};
  }
  friend UBigInt &operator/=(UBigInt &first, const BigInt &second) {
    if (second >= 0) {
      return first /= second.abs_;
    }
    throw UBigIntUnderflow{};
  }
  friend UBigInt &operator%=(UBigInt &first, const BigInt &second) {
    return first %= second.abs_;
  }
  friend BigInt operator+(const BigInt &first, const BigInt &second) {
    BigInt copy(first);
    return copy += second;
  }
  friend BigInt operator-(const BigInt &, const BigInt &);
  friend BigInt operator*(const BigInt &, const BigInt &);
  friend BigInt operator/(const BigInt &, const BigInt &);
  friend BigInt operator%(const BigInt &, const BigInt &);
  friend BigInt operator+(const BigInt &, const UBigInt &);
  friend BigInt operator-(const BigInt &, const UBigInt &);
  friend BigInt operator*(const BigInt &, const UBigInt &);
  friend BigInt operator/(const BigInt &, const UBigInt &);
  friend BigInt operator%(const BigInt &, const UBigInt &);
  friend BigInt &operator+=(BigInt &, int64_t);
  friend BigInt &operator-=(BigInt &, int64_t);
  friend BigInt &operator*=(BigInt &, int64_t);
  friend BigInt &operator/=(BigInt &, int64_t);
  friend BigInt &operator%=(BigInt &, int64_t);
  friend BigInt operator+(const BigInt &, int64_t);
  friend BigInt operator-(const BigInt &, int64_t);
  friend BigInt operator*(const BigInt &, int64_t);
  friend BigInt operator/(const BigInt &, int64_t);
  friend BigInt operator%(const BigInt &, int64_t);
  friend UBigInt operator+(const UBigInt &, const BigInt &);
  friend UBigInt operator-(const UBigInt &, const BigInt &);
  friend UBigInt operator*(const UBigInt &, const BigInt &);
  friend UBigInt operator/(const UBigInt &, const BigInt &);
  friend UBigInt operator%(const UBigInt &, const BigInt &);

  friend bool operator<(const BigInt &, const BigInt &);
  friend bool operator>(const BigInt &, const BigInt &);
  friend bool operator<=(const BigInt &, const BigInt &);
  friend bool operator>=(const BigInt &, const BigInt &);
  friend bool operator==(const BigInt &, const BigInt &);
  friend bool operator!=(const BigInt &, const BigInt &);
  friend bool operator<(const BigInt &, const UBigInt &);
  friend bool operator>(const BigInt &, const UBigInt &);
  friend bool operator<=(const BigInt &, const UBigInt &);
  friend bool operator>=(const BigInt &, const UBigInt &);
  friend bool operator==(const BigInt &, const UBigInt &);
  friend bool operator!=(const BigInt &, const UBigInt &);
  friend bool operator<(const BigInt &, int64_t);
  friend bool operator>(const BigInt &, int64_t);
  friend bool operator<=(const BigInt &, int64_t);
  friend bool operator>=(const BigInt &, int64_t);
  friend bool operator==(const BigInt &, int64_t);
  friend bool operator!=(const BigInt &, int64_t);
  friend bool operator<(const UBigInt &, const BigInt &);
  friend bool operator>(const UBigInt &, const BigInt &);
  friend bool operator<=(const UBigInt &, const BigInt &);
  friend bool operator>=(const UBigInt &, const BigInt &);
  friend bool operator==(const UBigInt &, const BigInt &);
  friend bool operator!=(const UBigInt &, const BigInt &);
  friend bool operator<(int64_t, const BigInt &);
  friend bool operator>(int64_t, const BigInt &);
  friend bool operator<=(int64_t, const BigInt &);
  friend bool operator>=(int64_t, const BigInt &);
  friend bool operator==(int64_t, const BigInt &);
  friend bool operator!=(int64_t, const BigInt &);

  friend BigInt &operator<<=(BigInt &, size_t);
  friend BigInt &operator>>=(BigInt &, size_t);
  friend BigInt operator<<(const BigInt &, size_t);
  friend BigInt operator>>(const BigInt &, size_t);

  explicit operator int64_t() const;
  const UBigInt &Abs() const noexcept;

  friend std::ostream &operator<<(std::ostream &, const BigInt &);
  friend std::istream &operator>>(std::istream &, BigInt &);
  std::string Exact() const;

  using Approximation = UBigInt::Approximation;
  Approximation GetApproximation() const;
  ~BigInt() = default;

 private:
  UBigInt abs_;
  bool pos_;
};