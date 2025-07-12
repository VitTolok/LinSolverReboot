#include "rational.hpp"
#include <cstring>
#include <numeric>
#include <sstream>

const BigInt &Rational::Num() const { return this->num_; }
const UBigInt &Rational::Den() const { return this->den_; }

Rational &operator+=(Rational &base, const Rational &mod) {
  (base.num_ *= mod.den_) += (mod.num_ * base.den_);
  base.den_ *= mod.den_;
  base.Normalize();
  return base;
}
Rational &operator-=(Rational &base, const Rational &mod) {
  (base.num_ *= mod.den_) -= (mod.num_ * base.den_);
  base.den_ *= mod.den_;
  base.Normalize();
  return base;
}
Rational &operator*=(Rational &base, const Rational &mod) {
  base.num_ *= mod.num_;
  base.den_ *= mod.den_;
  base.Normalize();
  return base;
}
Rational &operator/=(Rational &base, const Rational &mod) {
  base.num_ *= mod.den_;
  base.den_ *= mod.num_.Abs();
  if (mod.num_ < 0) {
    base.num_ *= -1;
  }
  base.Normalize();
  return base;
}

Rational operator+(const Rational &first, const Rational &second) {
  Rational clone = first;
  return clone += second;
}
Rational operator-(const Rational &first, const Rational &second) {
  Rational clone = first;
  return clone -= second;
}
Rational operator-(const Rational &first) {
  Rational clone = first;
  clone.num_ *= -1;
  return clone;
}
Rational operator*(const Rational &first, const Rational &second) {
  Rational clone = first;
  return clone *= second;
}
Rational operator/(const Rational &first, const Rational &second) {
  Rational clone = first;
  return clone /= second;
}

Rational::Rational() : num_(0), den_(1u) {}
Rational::Rational(int64_t number) : num_(number), den_(1u) {}
Rational::Rational(int64_t num, int64_t den) {
  if (den < 0) {
    num_ = -num;
    den_ = static_cast<uint64_t>(-den);
  } else {
    num_ = num;
    den_ = static_cast<uint64_t>(den);
  }
  this->Normalize();
}
Rational::Rational(int64_t num, uint64_t den) : num_(num), den_(den) {
  this->Normalize();
}

Rational::Rational(const BigInt &number) : num_(number), den_(1u) {}
Rational::Rational(BigInt &&number) : num_(std::move(number)), den_(1u) {}
Rational::Rational(const BigInt &num, const BigInt &den) {
  if (den < 0) {
    num_ = num * (-1);
    den_ = den.Abs();
  } else {
    num_ = num;
    den_ = den.Abs();
  }
  this->Normalize();
}
Rational::Rational(const BigInt &num, const UBigInt &den)
    : num_(num), den_(den) {
  this->Normalize();
}

bool operator<(const Rational &first, const Rational &second) {
  return (first.num_ * second.den_) < (second.num_ * first.den_);
}
bool operator>(const Rational &first, const Rational &second) {
  return second < first;
}
bool operator<=(const Rational &first, const Rational &second) {
  return !(second < first);
}
bool operator>=(const Rational &first, const Rational &second) {
  return !(first < second);
}
bool operator==(const Rational &first, const Rational &second) {
  return (first.num_ == second.num_) && (first.den_ == second.den_);
}
bool operator!=(const Rational &first, const Rational &second) {
  return !(first == second);
}

std::ostream &operator<<(std::ostream &out, const Rational &number) {
  out << number.num_;
  if (number.den_ != 1u) {
    out << '/' << number.den_;
  }
  return out;
}
std::istream &operator>>(std::istream &in, Rational &number) try {
  std::string input;
  in >> input;
  size_t sep = input.find('/');
  if (sep < input.size()) {
    std::istringstream(input.substr(0, sep)) >> number.num_;
    std::istringstream(input.substr(sep + 1, input.size())) >> number.den_;
    number.Normalize();
    return in;
  }
  size_t dot = input.find('.');
  size_t exp = input.find('e');
  size_t mult = 0;
  if (exp < input.size() && dot < exp) {
    std::istringstream(input.substr(exp)) >> mult;
    if (mult >= (exp - dot) - 1) {
      std::istringstream(input) >> number.num_;
      number.den_ = 1;
      return in;
    }
  }
  if (dot >= input.size()) {
    std::istringstream(input) >> number.num_;
    number.den_ = 1u;
    return in;
  }
  if (exp >= input.size()) {
    exp = input.size();
  }
  std::istringstream(input.substr(0, dot)) >> number.num_;
  number.num_ *= Power(UBigInt(10), (exp - dot) - 1);
  std::istringstream(input.substr(dot + 1, (exp - dot) - 1)) >> number.den_;
  if (input[0] == '-') {
    number.num_ -= number.den_;
  } else {
    number.num_ += number.den_;
  }
  number.den_ = Power(UBigInt(10), (exp - dot) - 1);
  number.num_ *= Power(UBigInt(10), mult);
  number.Normalize();
  return in;
} catch (...) {
  throw InvalidRationalString();
}

void Rational::Normalize() {
  if (this->den_ == 1u) {
    return;
  }
  if (this->den_ == 0u) {
    throw InvalidRational();
  }
  UBigInt gcd = GCD(num_.Abs(), den_);
  this->num_ /= gcd;
  this->den_ /= gcd;
  return;
}