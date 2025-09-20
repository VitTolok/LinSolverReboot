#include "rational.hpp"
#include <cstring>
#include <numeric>
#include <sstream>
#include <cmath>

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
  if (number == 0) {
    return out << 0;
  }
  if (number.num_.Abs() <= 1'000'000 && number.den_ <= 1'000'000) {
    std::string numer = (std::ostringstream() << number.num_).str();
    if (number.den_ == 1) {
      return out << numer;
    }
    std::string denom = (std::ostringstream() << number.den_).str();
    if (numer.size() + denom.size() <= 6) {
      return out << numer << '/' << denom;
    }
  }
  auto approx = number.GetApproximation();
  if (std::abs(approx.power) <= 2) {
    return out << (approx.multiplier * powl(10, approx.power));
  }
  return out << approx.multiplier << 'e' << approx.power;
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
  int64_t mult = 0;
  if (exp < input.size()) {
    if (exp < input.size() - 1) {
      std::istringstream(input.c_str() + exp + 1) >> mult;
    }
    input.resize(exp);
  }
  if (dot < input.size()) {
    mult -= input.size() - dot - 1;
    input.erase(dot, 1);
  }
  std::istringstream(input) >> number.num_;
  if (mult > 0) {
    number.num_ *= Power(UBigInt(10), mult);
    number.den_ = 1;
  } else if (mult == 0) {
    number.den_ = 1;
  } else {
    number.den_ = Power(UBigInt(10), -mult);
    number.Normalize();
  }
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
  return std::move(num_.Abs() / den_ * den_);
}

Rational &Rational::Floor() {
  num_ /= den_;
  den_ = 1;
  return *this;
}

Rational::Approximation Rational::GetApproximation() const {
  auto num_approx = num_.GetApproximation();
  auto den_approx = den_.GetApproximation();
  Approximation approx{num_approx.multiplier / den_approx.multiplier,
                       static_cast<int64_t>(num_approx.power) - static_cast<int64_t>(den_approx.power)};
  if (approx.multiplier < 1 - 5e-12) {
    approx.multiplier *= 10;
    ++approx.power;
  }
  return approx;
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