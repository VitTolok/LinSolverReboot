#include "rational.hpp"
#include <cstring>
#include <numeric>
#include <sstream>

const int64_t &Rational::Num() const { return this->num_; }
const uint64_t &Rational::Den() const { return this->den_; }

Rational &operator+=(Rational &base, const Rational &mod) {
  base.num_ = (base.num_ * mod.den_) + (mod.num_ * base.den_);
  base.den_ *= mod.den_;
  base.Normalize();
  return base;
}
Rational &operator-=(Rational &base, const Rational &mod) {
  base.num_ = (base.num_ * mod.den_) - (mod.num_ * base.den_);
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
  base.den_ *= mod.num_;
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
  return out << number.num_ << '/' << number.den_;
}
std::istream &operator>>(std::istream &in, Rational &number) {
  std::string input;
  in >> input;
  size_t sep = input.find('/');
  if (sep < input.size()) {
    std::istringstream(input.substr(0, sep)) >> number.num_;
    std::istringstream(input.substr(sep + 1, input.size())) >> number.den_;
    number.Normalize();
    return in;
  }
  std::istringstream(input) >> number.num_;
  number.den_ = 1u;
  return in;
}

void Rational::Normalize() {
  if (this->den_ == 1u) {
    return;
  }
  if (this->den_ == 0u) {
    throw InvalidRational();
  }
  uint64_t gcd = std::gcd(this->num_, this->den_);
  this->num_ /= gcd;
  this->den_ /= gcd;
  return;
}