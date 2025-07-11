#include "BigInt.hpp"
#include <limits>
#include <sstream>

BigInt::BigInt() : pos_(false), abs_() {}
BigInt::BigInt(int64_t number) : pos_(number > 0), abs_() {
  if (number == std::numeric_limits<int64_t>::min()) {
    abs_ = (1ull << 63);
  } else {
    abs_ = std::abs(number);
  }
}

BigInt &BigInt::operator=(int64_t number) {
  pos_ = number > 0;
  if (number == std::numeric_limits<int64_t>::min()) {
    abs_ = (1ull << 63);
  } else {
    abs_ = std::abs(number);
  }
}

BigInt &operator+=(BigInt &first, const BigInt &second) {
  if (second.pos_) {
    return first += second.abs_;
  }
  return first -= second.abs_;
}
BigInt &operator-=(BigInt &first, const BigInt &second) {
  if (second.pos_) {
    return first -= second.abs_;
  }
  return first += second.abs_;
}
BigInt &operator*=(BigInt &first, const BigInt &second) {
  if (!second.pos_) {
    first.pos_ = !first.pos_;
  }
  return first *= second.abs_;
}
BigInt &operator/=(BigInt &first, const BigInt &second) {
  if (!second.pos_) {
    first.pos_ = !first.pos_;
  }
  return first /= second.abs_;
}
BigInt &operator%=(BigInt &first, const BigInt &second) {
  return first %= second.abs_;
}

BigInt &operator+=(BigInt &first, const UBigInt &second) {
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
BigInt &operator-=(BigInt &first, const UBigInt &second) {
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
BigInt &operator*=(BigInt &first, const UBigInt &second) {
  first.abs_ *= second;
  return first;
}
BigInt &operator/=(BigInt &first, const UBigInt &second) {
  first.abs_ /= second;
  return first;
}
BigInt &operator%=(BigInt &first, const UBigInt &second) {
  first.abs_ %= second;
  return first;
}

BigInt &operator+=(BigInt &bigint, int64_t number) {
  if (number >= 0) {
    if (bigint.pos_) {
      bigint.abs_ += static_cast<u_int64_t>(number);
    } else if (bigint.abs_ >= static_cast<u_int64_t>(number)) {
      bigint.abs_ -= static_cast<u_int64_t>(number);
    } else {
      bigint.pos_ = true;
      bigint.abs_ = UBigInt(static_cast<u_int64_t>(number)) - bigint.abs_;
    }
    return bigint;
  }
  if (number != std::numeric_limits<int64_t>::min()) {
    return bigint -= std::abs(number);
  }
  if (!bigint.pos_) {
    bigint.abs_ += (1ull << 63);
  } else if (bigint.abs_ >= (1ull << 63)) {
    bigint.abs_ -= (1ull << 63);
  } else {
    bigint.pos_ = false;
    bigint.abs_ = UBigInt(1ull << 63) - bigint.abs_;
  }
  return bigint;
}
BigInt &operator-=(BigInt &bigint, int64_t number) {
  if (number >= 0) {
    if (!bigint.pos_) {
      bigint.abs_ += static_cast<u_int64_t>(number);
    } else if (bigint.abs_ >= static_cast<u_int64_t>(number)) {
      bigint.abs_ -= static_cast<u_int64_t>(number);
    } else {
      bigint.pos_ = false;
      bigint.abs_ = UBigInt(static_cast<u_int64_t>(number)) - bigint.abs_;
    }
    return bigint;
  }
  if (number != std::numeric_limits<int64_t>::min()) {
    return bigint += std::abs(number);
  }
  if (bigint.pos_) {
    bigint.abs_ += (1ull << 63);
  } else if (bigint.abs_ >= (1ull << 63)) {
    bigint.abs_ -= (1ull << 63);
  } else {
    bigint.pos_ = true;
    bigint.abs_ = UBigInt(1ull << 63) - bigint.abs_;
  }
  return bigint;
}
BigInt &operator*=(BigInt &bigint, int64_t number) {
  if (number >= 0) {
    bigint.abs_ *= static_cast<u_int64_t>(number);
    return bigint;
  }
  bigint.pos_ = !bigint.pos_;
  if (number != std::numeric_limits<int64_t>::min()) {
    bigint.abs_ *= static_cast<u_int64_t>(std::abs(number));
    return bigint;
  }
  bigint.abs_ <<= 63;
  return bigint;
}
BigInt &operator/=(BigInt &bigint, int64_t number) {
  if (number >= 0) {
    bigint.abs_ /= static_cast<u_int64_t>(number);
    return bigint;
  }
  bigint.pos_ = !bigint.pos_;
  if (number != std::numeric_limits<int64_t>::min()) {
    bigint.abs_ /= static_cast<u_int64_t>(std::abs(number));
    return bigint;
  }
  bigint.abs_ >>= 63;
  return bigint;
}
BigInt &operator%=(BigInt &bigint, int64_t number) {
  if (number != std::numeric_limits<int64_t>::min()) {
    bigint.abs_ %= static_cast<u_int64_t>(std::abs(number));
    return bigint;
  }
  bigint.abs_ %= (1ull << 63);
  return bigint;
}

UBigInt &operator+=(UBigInt &first, const BigInt &second) {
  if (second.pos_) {
    return first += second.abs_;
  }
  return first -= second.abs_;
}
UBigInt &operator-=(UBigInt &first, const BigInt &second) {
  if (second.pos_) {
    return first -= second.abs_;
  }
  return first += second.abs_;
}
UBigInt &operator*=(UBigInt &first, const BigInt &second) {
  if (second.pos_) {
    return first *= second.abs_;
  }
  if (second == 0) {
    return first = 0;
  }
  throw UBigIntUnderflow();
}
UBigInt &operator/=(UBigInt &first, const BigInt &second) {
  if (second.pos_) {
    return first /= second.abs_;
  }
  if (second == 0) {
    throw BigIntDivisionByZero();
  }
  throw UBigIntUnderflow();
}
UBigInt &operator%=(UBigInt &first, const BigInt &second) {
  return first %= second.abs_;
}

BigInt operator+(const BigInt &first, const BigInt &second) {
  BigInt copy = first;
  return copy += second;
}
BigInt operator-(const BigInt &first, const BigInt &second) {
  BigInt copy = first;
  return copy -= second;
}
BigInt operator*(const BigInt &first, const BigInt &second) {
  BigInt copy = first;
  return copy *= second;
}
BigInt operator/(const BigInt &first, const BigInt &second) {
  BigInt copy = first;
  return copy /= second;
}
BigInt operator%(const BigInt &first, const BigInt &second) {
  BigInt copy = first;
  return copy %= second;
}

BigInt operator+(const BigInt &first, const UBigInt &second) {
  BigInt copy = first;
  return copy += second;
}
BigInt operator-(const BigInt &first, const UBigInt &second) {
  BigInt copy = first;
  return copy -= second;
}
BigInt operator*(const BigInt &first, const UBigInt &second) {
  BigInt copy = first;
  return copy *= second;
}
BigInt operator/(const BigInt &first, const UBigInt &second) {
  BigInt copy = first;
  return copy /= second;
}
BigInt operator%(const BigInt &first, const UBigInt &second) {
  BigInt copy = first;
  return copy %= second;
}

BigInt operator+(const BigInt &bigint, int64_t number) {
  BigInt copy = bigint;
  return copy += number;
}
BigInt operator-(const BigInt &bigint, int64_t number) {
  BigInt copy = bigint;
  return copy -= number;
}
BigInt operator*(const BigInt &bigint, int64_t number) {
  BigInt copy = bigint;
  return copy *= number;
}
BigInt operator/(const BigInt &bigint, int64_t number) {
  BigInt copy = bigint;
  return copy /= number;
}
BigInt operator%(const BigInt &bigint, int64_t number) {
  BigInt copy = bigint;
  return copy %= number;
}

UBigInt operator+(const UBigInt &first, const BigInt &second) {
  UBigInt copy = first;
  return copy += second;
}
UBigInt operator-(const UBigInt &first, const BigInt &second) {
  UBigInt copy = first;
  return copy -= second;
}
UBigInt operator*(const UBigInt &first, const BigInt &second) {
  UBigInt copy = first;
  return copy *= second;
}
UBigInt operator/(const UBigInt &first, const BigInt &second) {
  UBigInt copy = first;
  return copy /= second;
}
UBigInt operator%(const UBigInt &first, const BigInt &second) {
  UBigInt copy = first;
  return copy %= second;
}

bool operator<(const BigInt &first, const BigInt &second) {
  if (first == 0 && second == 0) {
    return false;
  }
  if (first.pos_ != second.pos_) {
    return !first.pos_;
  }
  if (first.pos_) {
    return first.abs_ < second.abs_;
  }
  return first.abs_ > second.abs_;
}
bool operator>(const BigInt &first, const BigInt &second) {
  return second < first;
}
bool operator<=(const BigInt &first, const BigInt &second) {
  return !(second < first);
}
bool operator>=(const BigInt &first, const BigInt &second) {
  return !(first < second);
}
bool operator==(const BigInt &first, const BigInt &second) {
  if (first.abs_ != second.abs_) {
    return false;
  }
  return first.pos_ == second.pos_ || first.abs_ == 0;
}
bool operator!=(const BigInt &first, const BigInt &second) {
  return !(first == second);
}

bool operator<(const BigInt &first, const UBigInt &second) {
  if (first == 0) {
    return 0 < second;
  }
  if (!first.pos_) {
    return true;
  }
  return first.abs_ < second;
}
bool operator>(const BigInt &first, const UBigInt &second) {
  return second < first;
}
bool operator<=(const BigInt &first, const UBigInt &second) {
  return !(second < first);
}
bool operator>=(const BigInt &first, const UBigInt &second) {
  return !(first < second);
}
bool operator==(const BigInt &first, const UBigInt &second) {
  if (first == 0) {
    return second == 0;
  }
  if (!first.pos_) {
    return false;
  }
  return first.abs_ == second;
}
bool operator!=(const BigInt &first, const UBigInt &second) {
  return !(first == second);
}

bool operator<(const BigInt &bigint, int64_t number) {
  if (static_cast<int64_t>(bigint) == bigint) {
    return static_cast<int64_t>(bigint) < number;
  }
  return !bigint.pos_;
}
bool operator>(const BigInt &bigint, int64_t number) { return number < bigint; }
bool operator<=(const BigInt &bigint, int64_t number) {
  return !(number < bigint);
}
bool operator>=(const BigInt &bigint, int64_t number) {
  return !(bigint < number);
}
bool operator==(const BigInt &bigint, int64_t number) {
  if (bigint.abs_ > (1ull << 63)) {
    return false;
  }
  if (bigint.abs_ == (1ull << 63)) {
    if (!bigint.pos_) {
      return number == std::numeric_limits<int64_t>::min();
    }
    return false;
  }
  return static_cast<int64_t>(bigint) == number;
}
bool operator!=(const BigInt &bigint, int64_t number) {
  return !(bigint == number);
}

bool operator<(const UBigInt &first, const BigInt &second) {
  if (second == 0) {
    return false;
  }
  if (!second.pos_) {
    return false;
  }
  return first < second.abs_;
}
bool operator>(const UBigInt &first, const BigInt &second) {
  return second < first;
}
bool operator<=(const UBigInt &first, const BigInt &second) {
  return !(second < first);
}
bool operator>=(const UBigInt &first, const BigInt &second) {
  return !(first < second);
}
bool operator==(const UBigInt &first, const BigInt &second) {
  return second == first;
}
bool operator!=(const UBigInt &first, const BigInt &second) {
  return !(second == first);
}

bool operator<(int64_t number, const BigInt &bigint) {
  if (static_cast<int64_t>(bigint) == bigint) {
    return number < static_cast<int64_t>(bigint);
  }
  return bigint.pos_;
}
bool operator>(int64_t number, const BigInt &bigint) { return bigint < number; }
bool operator<=(int64_t number, const BigInt &bigint) {
  return !(bigint < number);
}
bool operator>=(int64_t number, const BigInt &bigint) {
  return !(number < bigint);
}
bool operator==(int64_t number, const BigInt &bigint) {
  if (bigint.abs_ > (1ull << 63)) {
    return false;
  }
  if (bigint.abs_ == (1ull << 63)) {
    if (!bigint.pos_) {
      return number == std::numeric_limits<int64_t>::min();
    }
    return false;
  }
  return static_cast<int64_t>(bigint) == number;
}
bool operator!=(int64_t number, const BigInt &bigint) {
  return !(number == bigint);
}

BigInt &operator<<=(BigInt &bigint, size_t shift) {
  bigint.abs_ <<= shift;
  return bigint;
}
BigInt &operator>>=(BigInt &bigint, size_t shift) {
  bigint.abs_ >>= shift;
  return bigint;
}
BigInt operator<<(const BigInt &bigint, size_t shift) {
  BigInt copy = bigint;
  copy.abs_ <<= shift;
  return copy;
}
BigInt operator>>(const BigInt &bigint, size_t shift) {
  BigInt copy = bigint;
  copy.abs_ >>= shift;
  return copy;
}

BigInt::operator int64_t() const {
  u_int64_t modulus = static_cast<u_int64_t>(abs_);
  if (modulus == (1ull << 63) && !pos_) {
    return std::numeric_limits<int64_t>::min();
  }
  modulus %= (1ull << 63);
  return static_cast<int64_t>(modulus) * (pos_ ? 1 : -1);
}

const UBigInt &BigInt::Abs() const noexcept { return abs_; }

std::ostream &operator<<(std::ostream &out, const BigInt &bigint) {
  if (!bigint.pos_) {
    out << '-';
  }
  return out << bigint.abs_;
}

std::istream &operator>>(std::istream &in, BigInt &bigint) {
  std::string input;
  in >> input;
  if (!input.empty() && input[0] == '-') {
    bigint.pos_ = false;
    input.erase(0);
  } else {
    bigint.pos_ = true;
  }
  std::istringstream(input) >> bigint.abs_;
  return in;
}

std::string BigInt::Exact() const { return (pos_ ? "" : "-") + abs_.Exact(); }