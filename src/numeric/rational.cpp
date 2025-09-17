#include "rational.hpp"
#include <cstring>
#include <numeric>
#include <sstream>

const BigInt &Rational::Num() const {
  return this->num_;
}
const UBigInt &Rational::Den() const {
  return this->den_;
}

Rational &operator+=(Rational &base, const Rational &mod) {
  base.num_ = (std::move(base.num_) * mod.den_) + (mod.num_ * base.den_);
  base.den_ *= mod.den_;
  base.Normalize();
  return base;
}
Rational &operator-=(Rational &base, const Rational &mod) {
  base.num_ = (std::move(base.num_) * mod.den_) - (mod.num_ * base.den_);
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
    base.num_ = -std::move(base.num_);
  }
  base.Normalize();
  return base;
}
Rational &operator%=(Rational &base, const Rational &mod) {
  return base -= std::move((base / mod).Floor()) * mod;
}

Rational &operator+=(Rational &first, Rational &&second) {
  first.num_ = (std::move(first.num_) * second.den_) + (first.den_ * std::move(second.num_));
  first.den_ *= second.den_;
  first.Normalize();
  return first;
}
Rational &operator-=(Rational &first, Rational &&second) {
  first.num_ = (std::move(first.num_) * second.den_) - (first.den_ * std::move(second.num_));
  first.den_ *= second.den_;
  first.Normalize();
  return first;
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
  std::istringstream(input) >> number.num_;
  number.den_ = 1_uint64;
  return in;
} catch (...) {
  throw InvalidRationalString();
}

Rational::operator BigInt() const {
  return num_ / den_ * den_;
}
Rational::operator UBigInt() const {
  if (num_ < 0) {
    throw UBigIntUnderflow{};
  }
  return std::move(const_cast<UBigInt &>((num_ / den_ * den_).Abs()));
}

Rational &Rational::Floor() {
  (num_ /= den_) *= den_;
  return *this;
}

void Rational::Normalize() {
  if (this->den_ == 1_uint64) {
    return;
  }
  if (this->den_ == 0_uint64) {
    throw InvalidRational();
  }
  UBigInt gcd = GCD(this->num_.Abs(), this->den_);
  this->num_ /= gcd;
  this->den_ /= gcd;
  return;
}