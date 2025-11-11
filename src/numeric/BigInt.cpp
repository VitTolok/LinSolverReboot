#include "BigInt.hpp"
#include <limits>
#include <sstream>

BigInt::BigInt() : pos_(false), abs_() {
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

BigInt &operator+=(BigInt &bigint, uint64_t number) {
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
BigInt &operator-=(BigInt &bigint, uint64_t number) {
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
BigInt &operator*=(BigInt &bigint, uint64_t number) {
  bigint.abs_ *= number;
  return bigint;
}
BigInt &operator/=(BigInt &bigint, uint64_t number) {
  bigint.abs_ /= number;
  return bigint;
}
BigInt &operator%=(BigInt &bigint, uint64_t number) {
  bigint.abs_ %= number;
  return bigint;
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
  uint64_t modulus = static_cast<uint64_t>(abs_);
  if (modulus == (1ull << 63) && !pos_) {
    return std::numeric_limits<int64_t>::min();
  }
  modulus %= (1ull << 63);
  return static_cast<int64_t>(modulus) * (pos_ ? 1 : -1);
}

std::ostream &operator<<(std::ostream &out, const BigInt &bigint) {
  if (!bigint.pos_ && bigint.abs_ != 0) {
    out << '-';
  }
  return out << bigint.abs_;
}

std::istream &operator>>(std::istream &in, BigInt &bigint) {
  std::string input;
  in >> input;
  if (!input.empty() && input[0] == '-') {
    bigint.pos_ = false;
    input.erase(0, 1);
  } else {
    bigint.pos_ = true;
  }
  std::istringstream(input) >> bigint.abs_;
  return in;
}

std::string BigInt::Exact() const {
  return ((pos_ || (abs_ == 0)) ? "" : "-") + abs_.Exact();
}

BigInt::Approximation BigInt::GetApproximation() const {
  auto approx = abs_.GetApproximation();
  if (!pos_ && abs_ > 0) {
    approx.multiplier *= -1;
  }
  return std::move(approx);
}
